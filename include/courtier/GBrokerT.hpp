/**
 * @file GBrokerT.hpp
 */

/*
 * Copyright (C) Gemfony scientific UG (haftungsbeschraenkt)
 *
 * See the AUTHORS file in the top-level directory for a list of authors.
 *
 * Contact: contact [at] gemfony (dot) eu
 *
 * This file is part of the Geneva library collection.
 *
 * Geneva was developed with kind support from Karlsruhe Institute of
 * Technology (KIT) and Steinbuch Centre for Computing (SCC). Further
 * information about KIT and SCC can be found at http://www.kit.edu/english
 * and http://scc.kit.edu .
 *
 * Geneva is free software: you can redistribute and/or modify it under
 * the terms of version 3 of the GNU Affero General Public License
 * as published by the Free Software Foundation.
 *
 * Geneva is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with the Geneva library. If not, see <http://www.gnu.org/licenses/>.
 *
 * For further information on Gemfony scientific and Geneva, visit
 * http://www.gemfony.eu .
 */

// Global checks, defines and includes needed for all of Geneva
#include "common/GGlobalDefines.hpp"

// Standard headers go here

#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <utility>
#include <limits>
#include <stdexcept>
#include <algorithm>
#include <functional>
#include <limits>
#include <chrono>
#include <mutex>
#include <condition_variable>

// Boost headers go here

#include <boost/utility.hpp>
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/thread.hpp>

#ifndef GBROKERT_HPP_
#define GBROKERT_HPP_

// Geneva headers go here
#include "common/GExceptions.hpp"
#include "common/GLogger.hpp"
#include "common/GBoundedBufferT.hpp"
#include "common/GSingletonT.hpp"
#include "common/GThreadGroup.hpp"
#include "courtier/GBaseConsumerT.hpp"
#include "courtier/GBufferPortT.hpp"
#include "courtier/GCourtierEnums.hpp"

namespace Gem {
namespace Courtier {

/******************************************************************************/
/** @brief Class to be thrown as a message in the case of a time-out in GBuffer */
class buffer_not_present : public std::exception {
public:
	buffer_not_present(void) throw() { }
	buffer_not_present(const buffer_not_present &) throw() { }
	~buffer_not_present() throw() { }
};

/******************************************************************************/

/** @brief The maximum allowed port id. Note that, if we have no 64 bit integer types,
 * we will only be able to count up to roughly 4 billion. PORTIDTYPE is defined in
 * GBoundedBufferT.hpp, based on whether BOOST_HAS_LONG_LONG is defined or not. */
const Gem::Common::PORTIDTYPE MAXPORTID = boost::numeric::bounds<Gem::Common::PORTIDTYPE>::highest() - 1;

/******************************************************************************/
/**
 * This class acts as the main interface between producers and consumers.
 */
template<typename carrier_type>
class GBrokerT
	: private boost::noncopyable
{
	typedef typename std::shared_ptr<Gem::Common::GBoundedBufferT<std::shared_ptr < carrier_type>> > GBoundedBufferT_Ptr;
	typedef typename std::list<GBoundedBufferT_Ptr> BufferPtrList;
	typedef typename std::map<Gem::Common::PORTIDTYPE, GBoundedBufferT_Ptr> BufferPtrMap;

public:
	/***************************************************************************/
	/**
	 * The default constructor.
	 */
	GBrokerT()
		: m_finalized(false)
	   , m_lastId(0)
  		, m_currentGetPosition(m_RawBuffers.begin())
	   , m_buffersPresent(false)
	{ /* nothing */ }

	/***************************************************************************/
	/**
	 * The standard destructor. Notifies all consumers that they should stop, then waits
	 * for their threads to terminate.
	 */
	virtual ~GBrokerT() {
		// Make sure the finalization code is executed
		// (if this hasn't happened already). Calling
		// finalize() multiple times is safe.
		finalize();
	}

	/***************************************************************************/
	/**
	 * Initializes the broker. This function does nothing. Its only purpose is to control
	 * initialization of the factory in the singleton.
	 */
	void init()
	{ /* nothing */ }

	/***************************************************************************/
	/**
	 * Shuts the broker down, together with all consumers.
	 */
	void finalize() {
		// Only allow one finalization action to be carried out
		if (m_finalized) return;

		// Shut down all consumers
		typename std::vector<std::shared_ptr<GBaseConsumerT<carrier_type>>>::iterator it;
		for (it = m_consumerCollection.begin(); it != m_consumerCollection.end(); ++it) {
			(*it)->shutdown();
		}

		{
			// Lock the access to our internal data
			// TODO: lock simultaneously
			std::unique_lock<std::mutex> rawBuffersPresentLock(m_RawBuffersPresentMutex);
			std::unique_lock<std::mutex> processedBuffersPresentLock(m_ProcessedBuffersPresentMutex);
			std::unique_lock<std::mutex> switchGetPositionLock(m_switchGetPositionMutex);
			std::unique_lock<std::mutex> findProcessedBufferLock(m_findProcesedBufferMutex);

			// Clear raw and processed buffers and the consumer lists
			m_RawBuffers.clear();
			m_ProcessedBuffers.clear();
			m_consumerCollection.clear();
			m_buffersPresent = false;

			// Make sure this function does not execute code a second time
			m_finalized = true;
		}
	}

	/***************************************************************************/
	/**
	 * This function is used by producers to register a new GBufferPortT object
	 * with the broker. A GBufferPortT object contains bounded buffers for raw (i.e.
	 * unprocessed) items and for processed items. A producer may at any time decide
	 * to drop a GBufferPortT. This is simply done by letting the shared_ptr<GBufferPortT>
	 * go out of scope. As the producer holds the only copy, the GBufferPortT will then be
	 * deleted. A BufferPort contains two shared_ptr<GBoundedBufferT> objects. A shared_ptr
	 * to these objects is saved upon enrollment with the broker, so that letting the
	 * shared_ptr<GBufferPortT> go out of scope will not drop the shared_ptr<GBoundedBufferT>
	 * objects immediately. This is important, as there may still be active connections
	 * with items being collected from or dropped into them by the consumers. It is the
	 * task of this function to remove the orphaned shared_ptr<GBoundedBufferT> objects.
	 * It thus needs to block access to the entire object during its operation. Note that one of
	 * the effects of this function is that the buffer collections will never run empty,
	 * once the first buffer has been registered.
	 *
	 * @param gbp A shared pointer to a new GBufferPortT object
	 */
	void enrol(std::shared_ptr <GBufferPortT<std::shared_ptr<carrier_type>>> gbp) {
		// Lock the access to our internal data
		// TODO: lock simultaneously
		std::unique_lock<std::mutex> rawBuffersPresentLock(m_RawBuffersPresentMutex);
		std::unique_lock<std::mutex> processedBuffersPresentLock(m_ProcessedBuffersPresentMutex);
		std::unique_lock<std::mutex> switchGetPositionLock(m_switchGetPositionMutex);
		std::unique_lock<std::mutex> findProcessedBufferLock(m_findProcesedBufferMutex);

		// Complain if the m_lastId is getting too large. m_lastId should
		// be replaced by a GUID/UUID, when it becomes available in Boost.
		// Note that, if this machine has no 64 bit integer types, we can
		// only count up to roughly 4 billion.
		if (m_lastId >= MAXPORTID) {
			glogger
			<< "In GBrokerT<T>::enrol(): m_lastId is getting too large: " << m_lastId << std::endl
			<< GEXCEPTION;
		}

		// Get new id for GBoundedBufferT classes and increment
		// the id afterwards for later use.
		// TODO: This should later be done with Boost::UUID
		Gem::Common::PORTIDTYPE portId = m_lastId++;

		// Retrieve the processed and original queues and tag them with
		// a suitable id. Increment the id for later use during other enrollments.
		std::shared_ptr < Gem::Common::GBoundedBufferT<std::shared_ptr < carrier_type>> >
		original = gbp->getOriginalQueue();
		std::shared_ptr < Gem::Common::GBoundedBufferT<std::shared_ptr < carrier_type>> >
		processed = gbp->getProcessedQueue();
		original->setId(portId);
		processed->setId(portId);

		// Find orphaned items in the two collections and remove them.
		typename BufferPtrList::iterator it;
		while ((it = std::find_if(m_RawBuffers.begin(), m_RawBuffers.end(),
										  [](GBoundedBufferT_Ptr p) { return p.unique(); })) != m_RawBuffers.end()) {
			m_RawBuffers.erase(it);
		}

		for (typename BufferPtrMap::iterator it = m_ProcessedBuffers.begin(); it != m_ProcessedBuffers.end();) {
			if ((it->second).unique()) { // Orphaned ? Get rid of it
				m_ProcessedBuffers.erase(it++);
			}
			else ++it;
		}

		// Attach the new items to the lists
		m_RawBuffers.push_back(original);
		m_ProcessedBuffers[portId] = processed;

		// Fix the current get-pointer. We simply attach it to the start of the list
		m_currentGetPosition = m_RawBuffers.begin();

		// If this was the first registered GBufferPortT object, we need to notify all
		// available consumer objects. We use notify_all here, as indeed all need to be informed
		// thst there is work to do. We only check one variable, as both are set
		// simultaneously.
		if (!m_buffersPresent) {
			m_buffersPresent = true;

			m_readyToGoRaw.notify_all();
			m_readyToGoProcessed.notify_all();
		}

		std::cout << "Buffer port with id " << portId << " successfully enrolled" << std::endl;
	}

	/***************************************************************************/
	/**
	 * Adds a new consumer to this class and starts its thread.
	 *
	 * @param gc A pointer to a GBaseConsumerT<carrier_type> object
	 */
	void enrol(std::shared_ptr <GBaseConsumerT<carrier_type>> gc) {
		std::unique_lock<std::mutex> consumerEnrolmentLock(m_consumerEnrolmentMutex);

		// Do nothing if a consumer of this type has already been registered
		if (std::find(m_consumerTypesPresent.begin(), m_consumerTypesPresent.end(), gc->getConsumerName()) != m_consumerTypesPresent.end()) {
			return;
		}

		// Archive the consumer and its name, then start its thread
		m_consumerCollection.push_back(gc);
		m_consumerTypesPresent.push_back(gc->getConsumerName());

		// Initiate processing in the consumer. This call will not block.
		gc->async_startProcessing();
	}

	/***************************************************************************/
	/**
	 * Retrieves a "raw" item from a GBufferPortT. This function will block
	 * if no item can be retrieved.
	 *
	 * @param p Holds the retrieved "raw" item
	 * @return A key that uniquely identifies the origin of p
	 */
	Gem::Common::PORTIDTYPE get(std::shared_ptr <carrier_type> &p) {
		// Locks access to our internal data until we have a copy of a buffer.
		// This will prevent the buffer from being removed, as the use count
		// is increased. Also fixes the iterator.
		{
			std::unique_lock<std::mutex> rawBuffersPresentLock(m_RawBuffersPresentMutex);

			// Do not let execution start before the first buffer has been enrolled
			m_readyToGoRaw.wait(rawBuffersPresentLock, rawBuffersPresent(m_RawBuffers));
		}

		{
			std::unique_lock<std::mutex> switchGetPositionLock(m_switchGetPositionMutex);

			// Save the current get position
			typename BufferPtrList::iterator currentGetPosition = m_currentGetPosition;

			// Switch to the next position, if any
			if ((m_RawBuffers.size() > 1) && (++m_currentGetPosition == m_RawBuffers.end())) {
				m_currentGetPosition = m_RawBuffers.begin();
			}

			// Retrieve the item. This function is thread-safe.
			(*currentGetPosition)->pop_back(p);

			// Return the id. The function is const, hence we should be
			// able to call it in a multi-threaded environment.
			return (*currentGetPosition)->getId();
		}
	}

	/***************************************************************************/
	/**
	 * Retrieves a "raw" item from a GBufferPortT, observing a timeout. Note that upon
	 * time-out an exception is thrown.
	 *
	 * @param p Holds the retrieved "raw" item
	 * @param timeout Time after which the function should time out
	 * @return A key that uniquely identifies the origin of p
	 */
	Gem::Common::PORTIDTYPE get(
		std::shared_ptr <carrier_type> &p
		, std::chrono::duration<double> timeout
	) {
		// Locks access to our internal data until we have a copy of a buffer.
		// This will prevent the buffer from being removed, as the use count
		// is increased. Also fixes the iterator.
		{
			std::unique_lock<std::mutex> rawBuffersPresentLock(m_RawBuffersPresentMutex);

			// Do not let execution start before the first buffer has been enrolled
			m_readyToGoRaw.wait(rawBuffersPresentLock, rawBuffersPresent(m_RawBuffers));
		}

		{
			std::unique_lock<std::mutex> switchGetPositionLock(m_switchGetPositionMutex);

			// Save the current get position
			typename BufferPtrList::iterator currentGetPosition = m_currentGetPosition;

			// Switch to the next position, if any
			if ((m_RawBuffers.size() > 1) && (++m_currentGetPosition == m_RawBuffers.end())) {
				m_currentGetPosition = m_RawBuffers.begin();
			}

			// Retrieve the item. This function is thread-safe.
			(*currentGetPosition)->pop_back(p, timeout);

			// Return the id. The function is const, hence we should be
			// able to call it in a multi-threaded environment.
			return (*currentGetPosition)->getId();
		}
	}

	/***************************************************************************/
	/**
	 * Retrieves a "raw" item from a GBufferPortT, observing a timeout. The function
	 * will indicate failure to retrieve a valid item by returning a boolean.
	 *
	 * @param id A key that identifies the origin of p
	 * @param p Holds the retrieved "raw" item
	 * @param timeout Time after which the function should time out
	 * @return A boolean that indicates whether the item retrieval was successful
	 */
	bool get(
		Gem::Common::PORTIDTYPE &id
		, std::shared_ptr <carrier_type> &p
		, std::chrono::duration<double> timeout
	) {
		// Locks access to our internal data until we have a copy of a buffer.
		// This will prevent the buffer from being removed, as the use count
		// is increased. Also fixes the iterator.
		{
			std::unique_lock<std::mutex> rawBuffersPresentLock(m_RawBuffersPresentMutex);

			// Do not let execution start before the first buffer has been enrolled
			m_readyToGoRaw.wait(rawBuffersPresentLock, rawBuffersPresent(m_RawBuffers));
		}

		{
			std::unique_lock<std::mutex> switchGetPositionLock(m_switchGetPositionMutex);

			// Save the current get position
			typename BufferPtrList::iterator currentGetPosition = m_currentGetPosition;

			// Save the id
			id = (*currentGetPosition)->getId();

			// Switch to the next position, if any
			if ((m_RawBuffers.size() > 1) && (++m_currentGetPosition == m_RawBuffers.end())) {
				m_currentGetPosition = m_RawBuffers.begin();
			}

			// Retrieve the item. This function is thread-safe.
			return (*currentGetPosition)->pop_back_bool(p, timeout);
		}
	}

	/***************************************************************************/
	/**
	 * Puts a processed item into the processed queue. Note that the item will simply
	 * be discarded if no target queue with the required id exists. The function will
	 * block otherwise, until it is again possible to submit the item.
	 *
	 * @param id A key that uniquely identifies the origin of p
	 * @param p Holds the "raw" item to be submitted to the processed queue
	 */
	void put(
		Gem::Common::PORTIDTYPE id
		, std::shared_ptr <carrier_type> p
	) {
		{
			std::unique_lock<std::mutex> processedBuffersPresentLock(m_ProcessedBuffersPresentMutex);

			// Do not let execution start before the first buffer has been enrolled
			m_readyToGoProcessed.wait(processedBuffersPresentLock, processedBuffersPresent(m_ProcessedBuffers));
		}

		{
			std::unique_lock<std::mutex> findProcessedBufferLock(m_findProcesedBufferMutex);

			// Cross-check that the id is indeed available and retrieve the buffer
			typename BufferPtrMap::iterator it;
			if ((it = m_ProcessedBuffers.find(id)) != m_ProcessedBuffers.end()) {
				// Add p to the correct buffer
				it->second->push_front(p);
			} else {
				glogger
				<< "In GBokerT<>::put(1): Warning!" << std::endl
				<< "Did not find buffer with id " << id << std::endl
				<< GWARNING;

				throw Gem::Courtier::buffer_not_present();
			}
		}
	}

	/***************************************************************************/
	/**
	 * Puts a processed item into the processed queue, observing a timeout. The function
	 * will throw a Gem::Courtier::buffer_not_present exception if the requested buffer
	 * isn't present. The function will return false if no item could be added to the buffer
	 * inside if the allowed time limits.
	 *
	 * @param id A key that uniquely identifies the origin of p
	 * @param p Holds the item to be submitted to the processed queue
	 * @param timeout Time after which the function should time out
	 * @param A boolean indicating whether the item could be added to the queue in time
	 */
	bool put(
		Gem::Common::PORTIDTYPE id
		, std::shared_ptr <carrier_type> p
	   , std::chrono::duration<double> timeout
	) {
		//------------------------------------------------------------------------
		// Make sure processing can start (impossible, before any buffer
		// port objects have been added)
		{
			std::unique_lock<std::mutex> processedBuffersPresentLock(m_ProcessedBuffersPresentMutex);

			// Do not let execution start before the first buffer has been enrolled
			m_readyToGoProcessed.wait(processedBuffersPresentLock, processedBuffersPresent(m_ProcessedBuffers));
		}

		//------------------------------------------------------------------------
		// Cross-check that the id is indeed available and retrieve the buffer
		{
			std::unique_lock<std::mutex> findProcessedBufferLock(m_findProcesedBufferMutex);

			typename BufferPtrMap::iterator it;
			if ((it = m_ProcessedBuffers.find(id)) != m_ProcessedBuffers.end()) {
				// Add p to the correct buffer, which we now assume to be valid. If this
				// cannot be done in time, let the audience know by returning false
				return it->second->push_front_bool(p, timeout);
			} else {
				glogger
				<< "In GBokerT<>::put(2): Warning!" << std::endl
				<< "Did not find buffer with id " << id << std::endl
				<< GWARNING;

				throw Gem::Courtier::buffer_not_present();
				return false; // Make the compiler happy
			}

			//------------------------------------------------------------------------
		}
	}

	/***************************************************************************/
	/**
	 * Checks whether any consumers have been enrolled at the time of calling. As
	 * consumers are maintained inside of a thread group and consumers may be added
	 * asynchronously. this function can only give a hint.
	 *
	 * @return A boolean indicating whether any consumers are registered
	 */
	bool hasConsumers() const {
		return m_consumerCollection.size() > 0 ? true : false;
	}

	/***************************************************************************/
	/**
	 * This function checks all registered consumers to see whether all of them
	 * are capable of full return. If so, it returns true. If at least one is
	 * found that is not capable of full return, it returns false.
	 */
	bool capableOfFullReturn() const {
#ifdef DEBUG
	   if(!hasConsumers()) {
	      glogger
	      << "In GBrokerT<carrier_type>::capableOfFullReturn(): Error!" << std::endl
         << "No consumers registered" << std::endl
         << GEXCEPTION;
	   }
#endif /* DEBUG */

		bool result = true;
		typename std::vector<std::shared_ptr < GBaseConsumerT<carrier_type>> > ::const_iterator
		cit;
		for (cit = m_consumerCollection.begin(); cit != m_consumerCollection.end(); ++cit) {
			if (!(*cit)->capableOfFullReturn()) {
				result = false;
				break; // stop the loop
			}
		}

		return result;
	}

private:
	/***************************************************************************/
	// Two function objects needed to check whether raw and processed buffers have been registered

	/**
	 * A function object that checks whether raw buffers have been registered.
	 * Note that this code is only called in a safe context, hence no protection
	 * is necessary.
	 */
	struct rawBuffersPresent {
	public:
		/* @brief Initializes the local container reference */
		rawBuffersPresent(
			const BufferPtrList &b
		) : m_b(b) { /* nothing */ }

		/** @brief Used for the actual test */
		bool operator()() const {
			return (!m_b.empty());
		}

	private:
		/** @brief Default constructor; intentionally private and undefined */
		rawBuffersPresent() = delete;

		const BufferPtrList &m_b; ///< Holds a reference to the actual container
	};

	/**
	  * A function object that checks whether processed buffers have been registered.
	  * Note that this code is only called in a safe context, hence no protection
	  * is necessary.
	  */
	struct processedBuffersPresent {
	public:
		/* @brief Initializes the local container reference */
		processedBuffersPresent(
			const BufferPtrMap &b
		) : m_b(b) { /* nothing */ }

		/** @brief Used for the actual test */
		bool operator()() const {
			return (!m_b.empty());
		}

	private:
		/** @brief Default constructor; intentionally private and undefined */
		processedBuffersPresent() = delete;

		const BufferPtrMap &m_b; ///< Holds a reference to the actual container
	};

	/***************************************************************************/
	GBrokerT(const GBrokerT<carrier_type> &) = delete; ///< Intentionally left undefined
	const GBrokerT& operator=(const GBrokerT<carrier_type> &) = delete; ///< Intentionally left undefined

	bool m_finalized; ///< Indicates whether the finalization code has already been executed

	mutable std::mutex m_RawBuffersPresentMutex; ///< Regulates access to the m_RawBuffers collection
	mutable std::mutex m_ProcessedBuffersPresentMutex; ///< Regulates access to the m_ProcessedBuffers collection

	mutable std::mutex m_consumerEnrolmentMutex; ///< Protects the enrolment of consumers

	mutable std::mutex m_switchGetPositionMutex; ///< Protects switches to the next get position
	mutable std::mutex m_findProcesedBufferMutex; ///< Protects finding a given processed buffer

	mutable std::condition_variable m_readyToGoRaw; ///< The get function will block until this condition variable is set
	mutable std::condition_variable m_readyToGoProcessed; ///< The put function will block until this condition variable is set

	BufferPtrList m_RawBuffers;       ///< Holds GBoundedBufferT objects with raw items
	BufferPtrMap m_ProcessedBuffers; ///< Holds GBoundedBufferT objects for processed items

	Gem::Common::PORTIDTYPE m_lastId; ///< The last id we've assigned to a buffer
	typename BufferPtrList::iterator m_currentGetPosition; ///< The current get position in the m_RawBuffers collection
	bool m_buffersPresent; ///< Set to true once the first buffers have been enrolled

	std::vector<std::shared_ptr<GBaseConsumerT<carrier_type>>> m_consumerCollection; ///< Holds the actual consumers
	std::vector<std::string> m_consumerTypesPresent; ///< Holds identifying strings for each consumer
};

/******************************************************************************/
/**
 * We require GBrokerT<T> to be a singleton. This ensures that, for a given T, one
 * and only one Broker object exists that is constructed before main begins. All
 * external communication should refer to GBROKER(T).
 */
#define GBROKER(T)      Gem::Common::GSingletonT<Gem::Courtier::GBrokerT<T>>::Instance(0)
#define RESETGBROKER(T) Gem::Common::GSingletonT<Gem::Courtier::GBrokerT<T>>::Instance(1)

/******************************************************************************/

} /* namespace Courtier */
} /* namespace Gem */

#endif /* GBROKERT_HPP_ */

/**
 * @file GStdThreadConsumerT.hpp
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
#include <type_traits>
#include <thread>
#include <mutex>

// Boost headers go here

#include <boost/cast.hpp>
#include <boost/lexical_cast.hpp>

#ifndef GSTDTHREADCONSUMERT_HPP_
#define GSTDTHREADCONSUMERT_HPP_

// Geneva headers go here
#include "common/GStdThreadGroup.hpp"
#include "common/GHelperFunctions.hpp"
#include "common/GLogger.hpp"
#include "courtier/GBrokerT.hpp"
#include "courtier/GBaseConsumerT.hpp"
#include "courtier/GProcessingContainerT.hpp"

namespace Gem {
namespace Courtier {

/** @brief The default number of threads per worker if the number of hardware threads cannot be determined */
const std::uint16_t DEFAULTTHREADSPERWORKER = 1;

/******************************************************************************/
/**
 * A derivative of GBaseConsumerT<>, that processes items in separate threads.
 * Objects of this class can exist alongside a networked consumer, as the broker
 * accepts more than one consumer. You can thus use this class to aid networked
 * optimization, if the server has spare CPU cores that would otherwise run idle.
 * The class makes use of the template arguments' process() function.
 */
template<class processable_type>
class GStdThreadConsumerT
	: public Gem::Courtier::GBaseConsumerT<processable_type> {
private:
	// Make sure processable_type adheres to the GProcessingContainerT interface
	static_assert(
		std::is_base_of<Gem::Courtier::GProcessingContainerT<processable_type>, processable_type>::value
		, "processable_type does not adhere to the GProcessingContainerT interface"
	);

public:
	class GWorker; ///< Forward declaration
	class GDefaultWorker; ///< Forward declaration

	/***************************************************************************/
	/**
	 * The default constructor.
	 */
	GStdThreadConsumerT()
		: Gem::Courtier::GBaseConsumerT<processable_type>()
		, m_threadsPerWorker(boost::numeric_cast<std::size_t>(Gem::Common::getNHardwareThreads(DEFAULTTHREADSPERWORKER)))
		, m_broker_ptr(GBROKER(processable_type))
		, m_workerTemplates(1, std::shared_ptr<GWorker>(new GDefaultWorker()))
	{ /* nothing */ }

	/***************************************************************************/
	/**
	* Standard destructor. Nothing - our threads receive the stop
	* signal from the broker and shouldn't exist at this point anymore.
	*/
	virtual ~GStdThreadConsumerT() { /* nothing */ }

	/***************************************************************************/
	/**
	* Sets the number of threads per worker. Note that this function
	* will only have an effect before the threads have been started.
	* If threadsPerWorker is set to 0, an attempt will be made to automatically
	* determine a suitable number of threads.
	*
	* @param tpw The maximum number of allowed threads
	*/
	void setNThreadsPerWorker(const std::size_t &tpw) {
		if (tpw == 0) {
			m_threadsPerWorker = boost::numeric_cast<std::size_t>(
				Gem::Common::getNHardwareThreads(DEFAULTTHREADSPERWORKER));
		}
		else {
			m_threadsPerWorker = tpw;
		}
	}

	/***************************************************************************/
	/**
	* Retrieves the maximum number of allowed threads
	*
	* @return The maximum number of allowed threads
	*/
	std::size_t getNThreadsPerWorker(void) const {
		return m_threadsPerWorker;
	}

	/***************************************************************************/
	/**
	* Finalization code. Sends all threads an interrupt signal.
	* process() then waits for them to join.
	*/
	virtual void shutdown() override {
		// Initiate the shutdown procedure
		GBaseConsumerT<processable_type>::shutdown();

		// Wait for local workers to terminate
		m_gtg.join_all();
		m_workers.clear();
	}

	/***************************************************************************/
	/**
	* A unique identifier for a given consumer
	*
	* @return A unique identifier for a given consumer
	*/
	virtual std::string getConsumerName() const override {
		return std::string("GStdThreadConsumerT");
	}

	/***************************************************************************/
	/**
	 * Returns a short identifier for this consumer
	 */
	virtual std::string getMnemonic() const override {
		return std::string("btc");
	}

   /***************************************************************************/
   /**
	  * Retrieves the number of workers registered with this class
	  */
   std::size_t getNWorkers() const {
	  return m_workerTemplates.size();
   }

	/***************************************************************************/
   /**
	  * Returns an indication whether full return can be expected from this
	  * consumer. Since evaluation is performed in threads, we assume that this
	  * is possible and return true. Note that it is up to the user to ensure
	  * that either his code does not crash or that exceptions are caught and
	  * the thread becomes available again.
	  */
   virtual bool capableOfFullReturn() const override {
	   return true;
	}

   /***************************************************************************/
 	/**
  	 * Returns the (possibly estimated) number of concurrent processing units.
    * A return value of 0 means "unknown". Note that this function does not
    * make any assumptions whether processing units are dedicated solely to a
    * given task.
    */
 	virtual std::size_t getNProcessingUnitsEstimate(bool& exact) const override {
		// Mark the answer as exact
		exact=true;
		// Return the result
		return boost::numeric_cast<std::size_t>(this->getNWorkers() * this->getNThreadsPerWorker());
 	}

	/***************************************************************************/
	/**
	* Starts the worker threads. This function will not block.
	* Termination of the threads is triggered by a call to GBaseConsumerT<processable_type>::shutdown().
	*/
	virtual void async_startProcessing() override {
#ifdef DEBUG
      if(m_workerTemplates.empty()) { // Is the template vector empty ?
         glogger
         << "In GStdThreadConsumerT<processable_type>::async_startProcessing(): Error!" << std::endl
         << "The workerTemplates_ vector is empty when it should not be empty" << std::endl
         << GEXCEPTION;
      }
#endif /* DEBUG */

		// Start m_threadsPerWorker threads for each registered worker template
		std::cout << "Starting " << m_threadsPerWorker << " processing threads for " << m_workerTemplates.size() << " worker(s) in GStdThreadConsumerT" << std::endl;
		for (std::size_t w = 0; w < m_workerTemplates.size(); w++) {
			for (std::size_t i = 0; i < m_threadsPerWorker; i++) {
				std::shared_ptr <GWorker> p_worker = (m_workerTemplates.at(w))->clone(i, this);
				m_gtg.create_thread(
					[p_worker]() { p_worker->run(); }
				);
				m_workers.push_back(p_worker);
			}
		}
	}

	/***************************************************************************/
	/**
	 * Allows to register a set of worker templates with this class. Note that all
	 * existing worker templates will be deleted. The class will not take ownership
	 * of the worker templates.
	 */
	void registerWorkerTemplates(
		const std::vector<std::shared_ptr < GWorker>>& workerTemplates
	) {
#ifdef DEBUG
      if(m_workerTemplates.empty()) { // Is the template vector empty ?
         glogger
         << "In GStdThreadConsumerT<processable_type>::registerWorkerTemplates(): Error!" << std::endl
         << "workerTemplates vector is empty when it should not be empty" << std::endl
         << GEXCEPTION;
      }

		std::size_t pos = 0;
		for(auto w_ptr: workerTemplates) { // std::shared_ptr may be copied
			if(!w_ptr) { // Does the template point somewhere ?
            glogger
            << "In GStdThreadConsumerT<processable_type>::registerWorkerTemplates(): Error!" << std::endl
            << "Found empty worker template pointer in position " << pos << std::endl
            << GEXCEPTION;
         }
         pos++;
		}
#endif /* DEBUG */

		m_workerTemplates.clear();
		m_workerTemplates = workerTemplates;

		assert(workerTemplates.size() == m_workerTemplates.size());
	}

	/***************************************************************************/
	/**
	 * Allows to register a single worker template with this class. Note that all
	 * existing worker templates will be deleted. The class will not take ownership
	 * of the worker template.
	 */
	void registerWorkerTemplate(
		std::shared_ptr<GWorker> workerTemplate
	) {
#ifdef DEBUG
      if(!workerTemplate) { // Does the template point somewhere ?
         glogger
         << "In GStdThreadConsumerT<processable_type>::registerWorkerTemplate(): Error!" << std::endl
         << "Found empty worker template pointer" << std::endl
         << GEXCEPTION;
      }
#endif /* DEBUG */

		m_workerTemplates.clear();
		m_workerTemplates.push_back(workerTemplate);
	}

	/***************************************************************************/
	/**
	 * Sets up a consumer and registers it with the broker. This function accepts
	 * a set of workers as argument.
	 */
	static void setup(
		const std::string &configFile
		, std::vector<std::shared_ptr <typename Gem::Courtier::GStdThreadConsumerT<processable_type>::GWorker>> workers
	) {
		std::shared_ptr <GStdThreadConsumerT<processable_type>> consumer_ptr(
			new GStdThreadConsumerT<processable_type>());
		consumer_ptr->registerWorkerTemplates(workers);
		consumer_ptr->parseConfigFile(configFile);
		GBROKER(processable_type)->enrol(consumer_ptr);
	}

	/***************************************************************************/
	/**
	 * Sets up a consumer and registers it with the broker. This function accepts
	 * a worker as argument.
	 */
	static void setup(
		const std::string &configFile,
		std::shared_ptr <typename Gem::Courtier::GStdThreadConsumerT<processable_type>::GWorker> worker_ptr
	) {
		std::shared_ptr <GStdThreadConsumerT<processable_type>> consumer_ptr(
			new GStdThreadConsumerT<processable_type>());
		consumer_ptr->registerWorkerTemplate(worker_ptr);
		consumer_ptr->parseConfigFile(configFile);
		GBROKER(processable_type)->enrol(consumer_ptr);
	}

	/***************************************************************************/
	/**
	 * Sets up a consumer and registers it with the broker. This function uses
	 * the default worker.
	 */
	static void setup(
		const std::string &configFile
	) {
		std::shared_ptr <GStdThreadConsumerT<processable_type>> consumer_ptr(new GStdThreadConsumerT<processable_type>());
		consumer_ptr->parseConfigFile(configFile);
		GBROKER(processable_type)->enrol(consumer_ptr);
	}

protected:
	/***************************************************************************/
	/**
	 * Adds local configuration options to a GParserBuilder object. We have only
	 * a single local option -- the number of threads
	 *
	 * @param gpb The GParserBuilder object, to which configuration options will be added
	 */
	virtual void addConfigurationOptions(
		Gem::Common::GParserBuilder &gpb
	) override {
		// Call our parent class'es function
		GBaseConsumerT<processable_type>::addConfigurationOptions(gpb);

		// Add local data
		gpb.registerFileParameter<std::size_t>(
			"threadsPerWorker" // The name of the variable
			, 0 // The default value
			, [this](std::size_t nt) { this->setNThreadsPerWorker(nt); }
		)
		<< "Indicates the number of threads used to process each worker." << std::endl
		<< "Setting threadsPerWorker to 0 will result in an attempt to" << std::endl
		<< "automatically determine the number of hardware threads.";
	}

	/***************************************************************************/
	/**
	 * Adds local command line options to a boost::program_options::options_description object.
	 *
	 * @param visible Command line options that should always be visible
	 * @param hidden Command line options that should only be visible upon request
	 */
	virtual void addCLOptions(
		boost::program_options::options_description &visible, boost::program_options::options_description &hidden
	) override {
		namespace po = boost::program_options;

		hidden.add_options()
			("threadsPerWorker", po::value<std::size_t>(&m_threadsPerWorker)->default_value(m_threadsPerWorker),
			 "\t[btc] The number of threads used to process each worker");
	}

	/***************************************************************************/
	/**
	 * Takes a boost::program_options::variables_map object and checks for supplied options.
	 */
	virtual void actOnCLOptions(const boost::program_options::variables_map &vm) override
	{ /* nothing */ }

private:
	/***************************************************************************/

	GStdThreadConsumerT(const GStdThreadConsumerT<processable_type> &); ///< Intentionally left undefined
	const GStdThreadConsumerT<processable_type> &operator=(const GStdThreadConsumerT<processable_type> &); ///< Intentionally left undefined

	std::size_t m_threadsPerWorker; ///< The maximum number of allowed threads in the pool
	Gem::Common::GStdThreadGroup m_gtg; ///< Holds the processing threads
	std::shared_ptr<GBrokerT<processable_type>> m_broker_ptr; ///< A shortcut to the broker so we do not have to go through the singleton
	std::vector<std::shared_ptr <GWorker>> m_workers; ///< Holds the current worker objects
	std::vector<std::shared_ptr <GWorker>> m_workerTemplates; ///< All workers will be created as a clone of these workers

public:
	/***************************************************************************/
	/**
	 * A nested class that performs the actual work inside of a thread.
	 * Classes derived from GStdThreadConsumerT may use their own derivative
	 * from this class and store complex information associated with the execution
	 * inside of the worker threads. Note that a GWorker(-derivative) must be
	 * copy-constructible and implement the clone() function.
	 */
	class GWorker {
	public:
		/************************************************************************/
		/**
		 * The default constructor
		 */
		GWorker()
			: m_thread_id(0), m_outer(nullptr), m_parsed(false), m_runLoopHasCommenced(false)
		{ /* nothing */ }

	protected:
		/************************************************************************/
		/**
		 * The copy constructor. We do not copy the thread id, as it is set by
		 * async_startprocessing().
		 */
		GWorker(
			const GWorker &cp
			, const std::size_t &thread_id
			, const GStdThreadConsumerT<processable_type> *c_ptr
		)
			: m_thread_id(thread_id)
		   , m_outer(c_ptr)
		   , m_parsed(cp.m_parsed)
		   , m_runLoopHasCommenced(false)
		{ /* nothing */ }

	public:
		/************************************************************************/
		/**
		 * The destructor
		 */
		virtual ~GWorker() { /* nothing */ }

		/************************************************************************/
		/**
		 * The main entry point for the execution
		 */
		void run() {
			try {
				m_runLoopHasCommenced = false;

				std::shared_ptr<processable_type> p;
				std::chrono::milliseconds timeout(200);

				while (true) {
					// Have we been asked to stop ?
					if (m_outer->stopped()) break;

					// If we didn't get a valid item, start again with the while loop
					if (!m_outer->m_broker_ptr->get(p, timeout)) {
						continue;
					}

#ifdef DEBUG
               // Check that we indeed got a valid item
               if(!p) { // We didn't get a valid item after all
                  glogger
                  << "In GStdThreadConsumerT<processable_type>::GWorker::run(): Error!" << std::endl
                  << "Got empty item when it shouldn't be empty!" << std::endl
                  << GEXCEPTION;
               }
#endif /* DEBUG */

					// Perform setup work once for the loop, as soon as we have
					// a processable item. Such setup work might require information
					// from that item, so we pass it to the function.
					if (!m_runLoopHasCommenced) {
						processInit(p);
						m_runLoopHasCommenced = true;
					}

					// Initiate the actual processing
					this->process(p);

					// Return the item to the broker. The item will be discarded
					// if the requested target queue cannot be found.
					try {
						while (!m_outer->m_broker_ptr->put(p, timeout)) { // This can lead to a loss of items
							// Terminate if we have been asked to stop
							if (m_outer->stopped()) break;
						}
					} catch (Gem::Courtier::buffer_not_present &) {
						continue;
					}
				}
			} catch(Gem::Common::gemfony_error_condition& e) {
				glogger
				<< "In GStdThreadConsumerT<processable_type>::GWorker::run(): Caught Gem::Common::gemfony_error_condition with message" << std::endl
				<< e.what() << std::endl
				<< GEXCEPTION;
			} catch (std::exception &e) {
				glogger
				<< "In GStdThreadConsumerT<processable_type>::GWorker::run():" << std::endl
				<< "Caught std::exception with message" << std::endl
				<< e.what() << std::endl
				<< GEXCEPTION;
			}
			catch (boost::exception &) {
				glogger
				<< "In GStdThreadConsumerT<processable_type>::GWorker::run():" << std::endl
				<< "Caught boost::exception with message" << std::endl
				<< GEXCEPTION;
			}
			catch (...) {
				glogger
				<< "In GStdThreadConsumerT<processable_type>::GWorker::run():" << std::endl
				<< "Caught unknown exception." << std::endl
				<< GEXCEPTION;
			}

			// Perform any final work
			processFinalize();
		}

		/************************************************************************/
		/**
		 * Retrieve this class'es id
		 */
		std::size_t getThreadId() const {
			return m_thread_id;
		}

		/************************************************************************/
		/**
		 * Parses a given configuration file. Note that parsing is done but once.
		 *
		 * @param configFile The name of a configuration file
		 */
		void parseConfigFile(const std::string &configFile) {
			if (m_parsed) return;

			// Create a parser builder object -- local options will be added to it
			Gem::Common::GParserBuilder gpb;

			// Add configuration options of this and of derived classes
			addConfigurationOptions(gpb);

			// Do the actual parsing. Note that this
			// will try to write out a default configuration file,
			// if no existing config file can be found
			gpb.parseConfigFile(configFile);

			m_parsed = true;
		}

	protected:
		/************************************************************************/
		/**
		 * Initialization code for processing. Can be specified in derived classes.
		 *
		 * @param p A pointer to a processable item meant to allow item-based setup
		 */
		virtual void processInit(std::shared_ptr <processable_type> p) { /* nothing */ }

		/************************************************************************/
		/**
		 * Finalization code for processing. Can be specified in derived classes.
		 */
		virtual void processFinalize() { /* nothing */ }

		/************************************************************************/
		/**
		 * Adds local configuration options to a GParserBuilder object. We have no local
		 * data, hence this function is empty. It could have been declared purely virtual,
		 * however, we do not want to force derived classes to implement this function,
		 * as it might not always be needed.
		 *
		 * @param gpb The GParserBuilder object, to which configuration options will be added
		 */
		virtual void addConfigurationOptions(
			Gem::Common::GParserBuilder &gpb
		) { /* nothing -- no local data */ }

		/************************************************************************/

		std::size_t m_thread_id; ///< The id of the thread running this class'es operator()
		const GStdThreadConsumerT<processable_type> *m_outer;

	private:
		/************************************************************************/

		bool m_parsed; ///< Indicates whether parsing has been completed
		bool m_runLoopHasCommenced; ///< Allows to check whether the while loop inside of the run function has started

	public:
		/************************************************************************/
		// Some purely virtual functions

		/** @brief Creation of deep clones of this object('s derivatives) */
		virtual std::shared_ptr <GWorker> clone(
			const std::size_t &, const GStdThreadConsumerT<processable_type> *
		) const = 0;

		/** @brief Actual per-item work is done here -- Implement this in derived classes */
		virtual void process(std::shared_ptr <processable_type> p) = 0;
	};

	/***************************************************************************/
	/**
	 * The default derivative of GWorker that is used when no other worker has
	 * been registered with our outer class.
	 */
	class GDefaultWorker
		: public GStdThreadConsumerT<processable_type>::GWorker {
	public:
		/************************************************************************/
		/**
		 * The default constructor
		 */
		GDefaultWorker() : GWorker() { /* nothing */ }

	protected:
		/************************************************************************/
		/**
		 * The copy constructor.
		 */
		GDefaultWorker(
			const GDefaultWorker &cp, const std::size_t &thread_id, const GStdThreadConsumerT<processable_type> *outer
		) : GWorker(cp, thread_id, outer) { /* nothing */ }

	public:
		/************************************************************************/
		/**
		 * The destructor
		 */
		virtual ~GDefaultWorker() { /* nothing */ }

		/************************************************************************/
		/**
		 * Create a deep clone of this object, camouflaged as a GWorker
		 */
		virtual std::shared_ptr <GWorker> clone(
			const std::size_t &thread_id, const GStdThreadConsumerT<processable_type> *outer
		) const {
#ifdef DEBUG
         if(!outer) {
            glogger
            << "In GStdThreadConsumerT<processable_type>::GWorker::clone(): Error!" << std::endl
            << "Got empty pointer!" << std::endl
            << GEXCEPTION;
         }
#endif /* DEBUG */

			return std::shared_ptr<GDefaultWorker>(new GDefaultWorker(*this, thread_id, outer));
		}

		/************************************************************************/
		/**
		 * Actual per-item work is done here. Overload this function if you want
		 * to do something different here.
		 */
		virtual void process(std::shared_ptr <processable_type> p) {
			// Do the actual work
#ifdef DEBUG
         if(p) p->process();
         else {
            glogger
            << "In GStdThreadConsumerT<processable_type>::GWorker::process(): Error!" << std::endl
            << "Received empty pointer for processable object" << std::endl
            << GEXCEPTION;
         }
   #else
			p->process();
#endif /* DEBUG */
		}

		/************************************************************************/
	};
};

/******************************************************************************/

} /* namespace Courtier */
} /* namespace Gem */

#endif /* GSTDTHREADCONSUMERT_HPP_ */
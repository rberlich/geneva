/**
 * @file GExecutorT.hpp
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

#include <cmath>
#include <sstream>
#include <algorithm>
#include <vector>
#include <utility>
#include <functional>
#include <memory>
#include <type_traits>
#include <tuple>
#include <chrono>

// Boost headers go here
#include <boost/lexical_cast.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/tracking.hpp>
#include <boost/serialization/split_member.hpp>

#ifndef GEXECUTOR_HPP_
#define GEXECUTOR_HPP_


// Geneva headers go here
#include "common/GExceptions.hpp"
#include "common/GLogger.hpp"
#include "common/GExpectationChecksT.hpp"
#include "common/GMathHelperFunctionsT.hpp"
#include "common/GHelperFunctionsT.hpp"
#include "common/GParserBuilder.hpp"
#include "common/GPlotDesigner.hpp"
#include "common/GThreadPool.hpp"
#include "courtier/GBufferPortT.hpp"
#include "courtier/GBrokerT.hpp"
#include "courtier/GCourtierEnums.hpp"
#include "courtier/GProcessingContainerT.hpp"

namespace Gem {
namespace Courtier {

/******************************************************************************/
/**
 * This class centralizes some functionality and data that is needed to perform
 * serial or parallel execution for a set of work items. Its main purpose is to
 * avoid duplication of code. Derived classes may deal with different types of
 * parallel execution, including connection to a broker and multi-threaded
 * execution. The serial mode is meant for debugging purposes only.
 */
template<typename processable_type>
class GBaseExecutorT {
	 // Make sure processable_type adheres to the GProcessingContainerT interface
	 static_assert(
		 std::is_base_of<Gem::Courtier::GProcessingContainerT<processable_type>, processable_type>::value
		 , "GBaseExecutorT: processable_type does not adhere to the GProcessingContainerT interface"
	 );

public:
	 /////////////////////////////////////////////////////////////////////////////

	 friend class boost::serialization::access;

	 template<typename Archive>
	 void serialize(Archive &ar, const unsigned int) {
		 using boost::serialization::make_nvp;
	 }

	 /////////////////////////////////////////////////////////////////////////////

	 /***************************************************************************/
	 /**
	  * The default constructor
	  */
	 GBaseExecutorT()
	 { /* nothing */ }

	 /***************************************************************************/
	 /**
	  * The copy constructor. No local data to be copied.
	  * m_submission_counter is just a temporary which always
	  * starts counting at 0.
	  *
	  * @param cp A copy of another GBrokerConnector object
	  */
	 GBaseExecutorT(const GBaseExecutorT<processable_type> &cp)
	 { /* nothing */ }

	 /***************************************************************************/
	 /**
	  * The standard destructor. We have no object-wide dynamically allocated data, hence
	  * this function is empty.
	  */
	 virtual ~GBaseExecutorT()
	 { /* nothing */ }

	 /***************************************************************************/
	 /**
	  * A standard assignment operator for GBaseExecutorT<processable_type> objects,
	  *
	  * @param cp A copy of another GBaseExecutorT<processable_type> object
	  * @return A constant reference to this object
	  */
	 const GBaseExecutorT<processable_type> &operator=(const GBaseExecutorT<processable_type> &cp) {
		 GBaseExecutorT<processable_type>::load(&cp);
		 return *this;
	 }

	 /***************************************************************************/
	 /**
	  * Loads the data of another GBaseExecutorT object
	  *
	  * @param cp A constant pointer to another GBaseExecutorT object
	  */
	 virtual void load(GBaseExecutorT<processable_type> const *const cp) BASE
	 { /* nothing */ }

	 /***************************************************************************/
	 /**
	  * Submits and retrieves a set of work items. You need to supply a vector
	  * of booleans of the same length indicating which items need to be submitted.
	  * "true" stands for "submit", "false" leads to the corresponding work items
	  * being ignored. After the function returns, some or all of the work items
	  * will have been processed. You can find out about this by querying the workItemPos
	  * vector. Item positions that have been processed will be set to "false".
	  * Positions remaining "true" have not been processed (but might still return in
	  * later iterations). It is thus also possible that returned items do not belong
	  * to the current submission cycle. They will be appended to the oldWorkItems vector.
	  * You might thus have to post-process the work items. Note that it is impossible
	  * to submit items that are not derived from GProcessingContainerT<processable_type>.
	  * This function will not alter the size of the workItems vector. It does not
	  * guarantee that all work items have indeed been processed. You can find out
	  * via the workItemPos vector.
	  *
	  * @param workItems A vector with work items to be evaluated beyond the broker
	  * @param workItemPos A vector of the item positions to be worked on
	  * @param oldWorkItems Will hold old work items after the job submission
	  * @param originator Optionally holds information on the caller
	  * @return A boolean indicating whether all expected items have returned
	  */
	 bool workOn(
		 std::vector<std::shared_ptr<processable_type>>& workItems
		 , std::vector<bool> &workItemPos
		 , std::vector<std::shared_ptr<processable_type>>& oldWorkItems
		 , bool removeUnprocessed = true
		 , const std::string &caller = std::string()
	 ) {
		 bool completed = false;

		 // Set the start time of the new iteration
		 m_submissionStartTime = std::chrono::system_clock::now();

		 // Check that both vectors have the same size
		 Gem::Common::assert_container_sizes_match(
			 workItems
			 , workItemPos
			 , "GBaseExecutorT<processable_type>::workOn()"
		 );

		 // The expected number of work items from the current iteration is
		 // equal to the number of unprocessed items
		 m_expectedNumber = std::count(workItemPos.begin(), workItemPos.end(), GBC_UNPROCESSED);
		 // Take care of a situation where no items have been submitted
		 if (m_expectedNumber == 0) {
			 return true;
		 }

		 // Make sure the vector of old work items is empty
		 oldWorkItems.clear();

		 // Perform necessary setup work for an iteration (a facility for derived classes)
		 this->iterationInit(workItems, workItemPos, oldWorkItems);

		 // Submit all work items
		 this->submitAllWorkItems(workItems, workItemPos);

		 // Wait for work items to complete. This function needs to
		 // be re-implemented in derived classes.
		 completed = waitForReturn(workItems, workItemPos, oldWorkItems);

		 // Perform necessary cleanup work for an iteration (a facility for derived classes)
		 this->iterationFinalize(workItems, workItemPos, oldWorkItems);

		 // Find out about the number of returned items
		 m_notReturnedLast = std::count(workItemPos.begin(), workItemPos.end(), GBC_UNPROCESSED);
		 m_returnedLast    = m_expectedNumber - m_notReturnedLast;

		 if (m_returnedLast == 0) { // Check whether any work items have returned at all
			 glogger
				 << "In GBaseExecutorT<processable_type>::iterationFinalize(): Warning!" << std::endl
				 << "No current items have returned with" << std::endl
				 << "m_expectedNumber  = " << m_expectedNumber << std::endl
				 << "m_notReturnedLast = " << m_notReturnedLast << std::endl
				 << "m_returnedLast    = " << m_returnedLast << std::endl
				 << "Got " << oldWorkItems.size() << " older work items" << std::endl
				 << GWARNING;
		 }

		 // Remove unprocessed items, if necessary (incomplete, and the removal of
		 // unprocessed items was requested by the user)
		 // TODO: Replace with custom erase_if function
		 if (!completed && removeUnprocessed) {
			 Gem::Common::erase_according_to_flags(
				 workItems
				 , workItemPos
				 , GBC_UNPROCESSED
				 , 0
				 , workItems.size()
			 );
		 }

		 // Sort old work items according to their ids so they can be readily used by the caller
		 std::sort(
			 oldWorkItems.begin()
			 , oldWorkItems.end()
			 , [](std::shared_ptr<processable_type> x_ptr, std::shared_ptr<processable_type> y_ptr) -> bool {
				 using namespace boost;
				 return x_ptr->getSubmissionPosition() < y_ptr->getSubmissionPosition();
			 }
		 );

		 // Give feedback to the audience (a facility for derived classes)
		 this->report();

		 // Update the submission counter
		 m_submission_counter++;

		 return completed;
	 }

	 /***************************************************************************/
	 /**
	  * Submits a set of work items in a range. There may be unprocessed work
	  * items. At your choice, these may be removed from the workItems vector
	  * or will be left there.
	  *
	  * @param workItems A vector with work items to be evaluated beyond the broker
	  * @param start The id of first item to be worked on in the vector
	  * @param end The id beyond the last item to be worked on in the vector
	  * @param oldWorkItems A vector holding work items from older iterations
	  * @param removeUnprocessed If set to true, unprocessed work items will be removed
	  * @param caller Optionally holds information on the caller
	  * @return A boolean indicating whether all expected items have returned
	  */
	 bool workOn(
		 std::vector<std::shared_ptr<processable_type>>& workItems
		 , const std::size_t &start
		 , const std::size_t &end
		 , std::vector<std::shared_ptr<processable_type>>& oldWorkItems
		 , bool removeUnprocessed = true
		 , const std::string &caller = std::string()
	 ) {
		 // Make sure the start/end positoons match
		 Gem::Common::assert_sizes_match_container(workItems, start, end, "GBaseExecutorT<processable_type>::workOn()");

		 // Assemble a position vector. Only items in fields marked
		 // unprocessed will be processed. Hence the vector is initialized
		 // with the GBC_PROCESSED-flag, and only items in positions start-end
		 // are marked as GBC_UNPROCESSED;
		 std::vector<bool> workItemPos(workItems.size(), GBC_PROCESSED);
		 for (std::size_t pos = start; pos != end; pos++) {
			 workItemPos[pos] = GBC_UNPROCESSED;
		 }

		 // Start the calculation. A return value of "true" indicates that all
		 // unprocessed items were processed
		 return this->workOn(
			 workItems
			 , workItemPos
			 , oldWorkItems
			 , removeUnprocessed
			 , caller
		 );
	 }

	 /***************************************************************************/
	 /**
	  * Submits and retrieves a set of work items in a range
	  *
	  * @param workItems A vector with work items to be evaluated beyond the broker
	  * @param range A std::tuple holding the boundaries of the submission range
	  * @param oldWorkItems A vector holding work items from older iterations
	  * @param removeUnprocessed If set to true, unprocessed work items will be removed
	  * @param caller Optionally holds information on the caller
	  * @return A boolean indicating whether all expected items have returned
	  */
	 bool workOn(
		 std::vector<std::shared_ptr<processable_type>>& workItems
		 , const std::tuple<std::size_t, std::size_t> &range
		 , std::vector<std::shared_ptr<processable_type>>& oldWorkItems
		 , const bool &removeUnprocessed = true
		 , const std::string &caller = std::string()
	 ) {
		 return this->workOn(
			 workItems
			 , std::get<0>(range)
			 , std::get<1>(range)
			 , oldWorkItems
			 , removeUnprocessed
			 , caller
		 );
	 }

	 /***************************************************************************/
	 /**
	  * Submits all work items in an array
	  *
	  * @param workItems A vector with work items to be evaluated beyond the broker
	  * @param oldWorkItems A vector holding work items from older iterations
	  * @param removeUnprocessed If set to true, unprocessed work items will be removed
	  * @param caller Optionally holds information on the caller
	  * @return A boolean indicating whether all expected items have returned
	  */
	 bool workOn(
		 std::vector<std::shared_ptr<processable_type>>& workItems
		 , std::vector<std::shared_ptr<processable_type>>& oldWorkItems
		 , const bool &removeUnprocessed = true
		 , const std::string &caller = std::string()
	 ) {
		 return this->workOn(
			 workItems
			 , 0
			 , workItems.size()
			 , oldWorkItems
			 , removeUnprocessed
			 , caller
		 );
	 }


	 /***************************************************************************/
	 /**
	  * Gives access to the value of the current submission id
	  */
	 SUBMISSION_COUNTER_TYPE getSubmissionId() const {
		 return m_submission_counter;
	 }

	 /***************************************************************************/
	 /**
	  * Adds local configuration options to a GParserBuilder object
	  *
	  * @param gpb The GParserBuilder object to which configuration options should be added
	  */
	 virtual void addConfigurationOptions(
		 Gem::Common::GParserBuilder &gpb
	 ) BASE {
		 /* no local data. hence empty */
	 }

	 /***************************************************************************/
	 /**
	  * General initialization function to be called prior to the first submission
	  */
	 virtual void init() BASE { /* nothing */ }

	 /***************************************************************************/
	 /**
	  * General finalization function to be called after the last submission
	  */
	 virtual void finalize() BASE { /* nothing */ }

 	 /***************************************************************************/
 	 /**
 	  * Retrieve the number of individuals returned during the last iteration
 	  */
	 std::size_t getNReturned() const {
 		 return m_returnedLast;
	 }

 	 /***************************************************************************/
 	 /**
 	  * Retrieve the number of individuals NOT returned during the last iteration
 	  */
	 std::size_t getNNotReturned() const {
		 return m_notReturnedLast;
	 }

	 /***************************************************************************/
	 /**
	  * Retrieves the current submission id
	  */
	 SUBMISSION_COUNTER_TYPE getCurrentSubmissionId() const {
		 return m_submission_counter;
	 }

protected:
	 /***************************************************************************/
	 /** @brief Submits a single work item */
	 virtual void submit(std::shared_ptr<processable_type>) = 0;

	 /** @brief Waits for work items to return */
	 virtual bool waitForReturn(
		 std::vector<std::shared_ptr<processable_type>>&
		 , std::vector<bool>&
		 , std::vector<std::shared_ptr<processable_type>>&
	 ) = 0;

	 /***************************************************************************/
	 /**
	  * Allow to perform necessary setup work for an iteration. Derived classes
	  * should make sure this base function is called first when they overload
	  * this function.
	  */
	 virtual void iterationInit(
		 std::vector<std::shared_ptr<processable_type>>& workItems
		 , std::vector<bool> &workItemPos
		 , std::vector<std::shared_ptr<processable_type>>& oldWorkItems
	 ) BASE { /* nothing */ }

	 /***************************************************************************/
	 /**
	  * Allows to perform necessary cleanup work for an iteration. Derived classes
	  * should make sure this base function is called last when they overload
	  * this function.
	  */
	 virtual void iterationFinalize(
		 std::vector<std::shared_ptr<processable_type>>& workItems
		 , std::vector<bool> &workItemPos
		 , std::vector<std::shared_ptr<processable_type>>& oldWorkItems
	 ) BASE { /* nothing */ }

	 /***************************************************************************/
	 /**
	  * Allows to emit information at the end of an iteration
	  */
	 virtual void report() BASE { /* nothing */ }

	 /***************************************************************************/
	 /**
	  * Submission of all work items in the list
	  */
	 void submitAllWorkItems(
		 std::vector<std::shared_ptr<processable_type>>& workItems
		 , std::vector<bool> &workItemPos
	 ) {
		 // Submit work items
		 SUBMISSION_POSITION_TYPE pos_cnt = 0;
		 for(auto w_ptr: workItems) { // std::shared_ptr may be copied
			 if (GBC_UNPROCESSED == workItemPos[pos_cnt]) { // Is the item due to be submitted ? We only submit items that are marked as "unprocessed"
#ifdef DEBUG
				 if(!w_ptr) {
					 glogger
						 << "In GBaseExecutorT<processable_type>::submitAllWorkItems(): Error" << std::endl
						 << "Received empty work item in position "  << pos_cnt << std::endl
						 << "m_submission_counter = " << m_submission_counter << std::endl
						 << GEXCEPTION;
				 }
#endif

				 w_ptr->setSubmissionCounter(m_submission_counter);
				 w_ptr->setSubmissionPosition(pos_cnt);

				 this->submit(w_ptr);
			 }
			 pos_cnt++;
		 }
	 }

	 /***************************************************************************/
	 // Local data
	 SUBMISSION_COUNTER_TYPE m_submission_counter = SUBMISSION_COUNTER_TYPE(0); ///< Counts the number of submissions initiated by this object. Note: not serialized!
	 std::size_t m_expectedNumber = 0; ///< The number of work items to be submitted (and expected back)
	 std::chrono::system_clock::time_point m_submissionStartTime = std::chrono::system_clock::now(); ///< Temporary that holds the start time for the retrieval of items in a given iteration

	 std::size_t m_returnedLast = 0; ///< The number of individuals returned in the last iteration cycle
	 std::size_t m_notReturnedLast = 0; ///< The number of individuals MOT returned in the last iteration cycle
};

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * This class executes work items serially. It is mostly meant for debugging
 * purposes
 */
template<typename processable_type>
class GSerialExecutorT
	: public GBaseExecutorT<processable_type>
{
	 ///////////////////////////////////////////////////////////////////////

	 friend class boost::serialization::access;

	 template<typename Archive>
	 void serialize(Archive &ar, const unsigned int) {
		 using boost::serialization::make_nvp;

		 ar & make_nvp("GBaseExecutorT", boost::serialization::base_object<GBaseExecutorT<processable_type>>(*this));
	 }

	 ///////////////////////////////////////////////////////////////////////

public:
	 /***************************************************************************/
	 /**
	  * The default constructor
	  */
	 GSerialExecutorT()
		 : GBaseExecutorT<processable_type>()
	 { /* nothing */ }

	 /***************************************************************************/
	 /**
	  * The copy constructor
	  *
	  * @param cp A copy of another GBrokerConnector object
	  */
	 GSerialExecutorT(const GSerialExecutorT<processable_type> &cp)
		 : GBaseExecutorT<processable_type>(cp)
	 { /* nothing */ }

	 /***************************************************************************/
	 /**
	  * The destructor
	  */
	 virtual ~GSerialExecutorT()
	 { /* nothing */ }

	 /***************************************************************************/
	 /**
	  * A standard assignment operator for GSerialExecutorT<processable_type> objects,
	  *
	  * @param cp A copy of another GSerialExecutorT<processable_type> object
	  * @return A constant reference to this object
	  */
	 const GSerialExecutorT<processable_type> &operator=(const GSerialExecutorT<processable_type> &cp) {
		 GSerialExecutorT<processable_type>::load(&cp);
		 return *this;
	 }

	 /***************************************************************************/
	 /**
	  * Loads the data of another GSerialExecutorT object
	  *
	  * @param cp A constant pointer to another GSerialExecutorT object
	  */
	 virtual void load(GBaseExecutorT<processable_type> const *const cp_base) override {
		 GSerialExecutorT<processable_type> const *const cp = dynamic_cast<GSerialExecutorT<processable_type> const *const>(cp_base);

		 if (!cp) { // nullptr
			 glogger
				 << "In GSerialExecutorT<processable_type>::load(): Conversion error!" << std::endl
				 << GEXCEPTION;
		 }

		 // Load our parent classes data
		 GBaseExecutorT<processable_type>::load(cp);
	 }

	 /***************************************************************************/
	 /**
	  * Adds local configuration options to a GParserBuilder object
	  *
	  * @param gpb The GParserBuilder object to which configuration options should be added
	  */
	 virtual void addConfigurationOptions(
		 Gem::Common::GParserBuilder &gpb
	 ) override {
		 // Call our parent class's function
		 GBaseExecutorT<processable_type>::addConfigurationOptions(gpb);

		 // No local data
	 }

protected:
	 /***************************************************************************/
	 /**
	  * Submits a single work item. In the case of serial execution, all work
	  * is done inside of this function. We rely on the process() function which
	  * is guaranteed to be part of the processable_type interface (note that
	  * our base class stipulates that processable_type is a derivative of
	  * GProcessingContainerT<processable_type> .
	  *
	  * @param w The work item to be processed
	  */
	 virtual void submit(std::shared_ptr <processable_type> w) override {
		 w->process();
	 }

	 /***************************************************************************/
	 /**
	  * Waits for work items to return. Mostlty empty, as all work is done inside
	  * of the submit() function.
	  */
	 virtual bool waitForReturn(
		 std::vector<std::shared_ptr < processable_type>>& workItems
		 , std::vector<bool> &workItemPos
		 , std::vector<std::shared_ptr<processable_type>>& oldWorkItems
	 ) override {
		 // Mark all positions as returned
		 for(auto it = workItemPos.begin(); it != workItemPos.end(); ++it) {
			 *it = GBC_PROCESSED;
		 }

		 return true;
	 }
};

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * This class executes a collection of work items in multiple threads
 */
template<typename processable_type>
class GMTExecutorT
	: public GBaseExecutorT<processable_type>
{
	 ///////////////////////////////////////////////////////////////////////

	 friend class boost::serialization::access;

	 template<typename Archive>
	 void serialize(Archive &ar, const unsigned int) {
		 using boost::serialization::make_nvp;

		 ar
		 & make_nvp("GBaseExecutorT", boost::serialization::base_object<GBaseExecutorT<processable_type>>(*this));
	 }

	 ///////////////////////////////////////////////////////////////////////

public:
	 /***************************************************************************/
	 /**
	  * The default constructor
	  */
	 GMTExecutorT()
		 : GBaseExecutorT<processable_type>(), m_gtp()
	 { /* nothing */ }

	 /***************************************************************************/
	 /**
	  * Initialization with the number of threads
	  */
	 GMTExecutorT(std::uint16_t nThreads)
		 : GBaseExecutorT<processable_type>(), m_gtp(nThreads)
	 { /* nothing */ }

	 /***************************************************************************/
	 /**
	  * The copy constructor
	  *
	  * @param cp A copy of another GBrokerConnector object
	  */
	 GMTExecutorT(const GMTExecutorT<processable_type> &cp)
		 : GBaseExecutorT<processable_type>(cp), m_gtp(cp.m_gtp.getNThreads())
	 { /* nothing */ }

	 /***************************************************************************/
	 /**
	  * The destructor
	  */
	 virtual ~GMTExecutorT()
	 { /* nothing */ }

	 /***************************************************************************/
	 /**
	  * A standard assignment operator for GMTExecutorT<processable_type> objects,
	  *
	  * @param cp A copy of another GMTExecutorT<processable_type> object
	  * @return A constant reference to this object
	  */
	 const GMTExecutorT<processable_type> &operator=(const GMTExecutorT<processable_type> &cp) {
		 GMTExecutorT<processable_type>::load(&cp);
		 return *this;
	 }

	 /***************************************************************************/
	 /**
	  * Loads the data of another GMTExecutorT object
	  *
	  * @param cp A constant pointer to another GMTExecutorT object
	  */
	 virtual void load(GBaseExecutorT<processable_type> const *const cp_base) override {
		 GMTExecutorT<processable_type> const *const cp = dynamic_cast<GMTExecutorT<processable_type> const *const>(cp_base);

		 if (!cp) { // nullptr
			 glogger
				 << "In GMTExecutorT<processable_type>::load(): Conversion error!" << std::endl
				 << GEXCEPTION;
		 }

		 // Load our parent classes data
		 GBaseExecutorT<processable_type>::load(cp);

		 // Adapt our local thread pool
		 m_gtp.setNThreads((cp->m_gtp).getNThreads());
	 }

	 /***************************************************************************/
	 /**
	  * Adds local configuration options to a GParserBuilder object
	  *
	  * @param gpb The GParserBuilder object to which configuration options should be added
	  */
	 virtual void addConfigurationOptions(
		 Gem::Common::GParserBuilder &gpb
	 ) override {
		 // Call our parent class's function
		 GBaseExecutorT<processable_type>::addConfigurationOptions(gpb);

		 // No local data
	 }

protected:
	 /***************************************************************************/
	 /**
	  * Submits a single work item. As we are dealing with multi-threaded
	  * execution, we simply push a worker into a thread pool.
	  *
	  * @param w The work item to be processed
	  */
	 virtual void submit(std::shared_ptr <processable_type> w) override {
		 m_gtp.async_schedule([w]() -> bool { return w->process(); });
	 }

	 /***************************************************************************/
	 /**
	  * Waits for the thread pool to run empty.
	  */
	 virtual bool waitForReturn(
		 std::vector<std::shared_ptr < processable_type>>& workItems
		 , std::vector<bool> &workItemPos
		 , std::vector<std::shared_ptr<processable_type>>& oldWorkItems
	 ) override {
		 m_gtp.wait();

		 // Mark all positions as "returned"
		 std::vector<bool>::iterator it;
		 for (it = workItemPos.begin(); it != workItemPos.end(); ++it) {
			 *it = false;
		 }

		 return true;
	 }

private:
	 /***************************************************************************/

	 Gem::Common::GThreadPool m_gtp; ///< Holds a thread pool
};

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * This class relays execution of work items to a broker, to which several
 * different consumers may be connected.
 */
template<typename processable_type>
class GBrokerExecutorT
	: public GBaseExecutorT<processable_type> {
	 ///////////////////////////////////////////////////////////////////////

	 friend class boost::serialization::access;

	 template<typename Archive>
	 void serialize(Archive &ar, const unsigned int) {
		 using boost::serialization::make_nvp;

		 ar
		 & make_nvp("GBaseExecutorT", boost::serialization::base_object<GBaseExecutorT<processable_type>>(*this))
		 & BOOST_SERIALIZATION_NVP(m_srm)
		 & BOOST_SERIALIZATION_NVP(m_maxResubmissions)
		 & BOOST_SERIALIZATION_NVP(m_waitFactor)
		 & BOOST_SERIALIZATION_NVP(m_initialWaitFactor)
		 & BOOST_SERIALIZATION_NVP(m_gpd)
		 & BOOST_SERIALIZATION_NVP(m_waiting_times_graph)
		 & BOOST_SERIALIZATION_NVP(m_returned_items_graph)
		 & BOOST_SERIALIZATION_NVP(m_waitFactorWarningEmitted);

		 // TODO: serialize m_lastReturnTime and m_lastAverage
	 }

	 ///////////////////////////////////////////////////////////////////////

	 using GBufferPortT_ptr = std::shared_ptr<Gem::Courtier::GBufferPortT<std::shared_ptr<processable_type>>>;

public:
	 /***************************************************************************/
	 /**
	  * The default constructor
	  */
	 GBrokerExecutorT()
		 : GBaseExecutorT<processable_type>()
		 , m_gpd("Maximum waiting times and returned items", 1, 2)
	 {
		 m_gpd.setCanvasDimensions(std::make_tuple<std::uint32_t,std::uint32_t>(1200,1600));

		 m_waiting_times_graph = std::shared_ptr<Gem::Common::GGraph2D>(new Gem::Common::GGraph2D());
		 m_waiting_times_graph->setXAxisLabel("Iteration");
		 m_waiting_times_graph->setYAxisLabel("Maximum waiting time [s]");
		 m_waiting_times_graph->setPlotMode(Gem::Common::graphPlotMode::CURVE);

		 m_returned_items_graph = std::shared_ptr<Gem::Common::GGraph2D>(new Gem::Common::GGraph2D());
		 m_returned_items_graph->setXAxisLabel("Iteration");
		 m_returned_items_graph->setYAxisLabel("Number of returned items");
		 m_returned_items_graph->setPlotMode(Gem::Common::graphPlotMode::CURVE);
	 }

	 /***************************************************************************/
	 /**
	  * Initialization with a given submission return mode.
	  *
	  * @param srm The submission-return mode to be used
	  */
	 explicit GBrokerExecutorT(submissionReturnMode srm)
		 : GBaseExecutorT<processable_type>()
		 , m_srm(srm)
		 , m_gpd("Maximum waiting times and returned items", 1, 2)
	 {
		 m_gpd.setCanvasDimensions(std::make_tuple<std::uint32_t,std::uint32_t>(1200,1600));

		 m_waiting_times_graph = std::shared_ptr<Gem::Common::GGraph2D>(new Gem::Common::GGraph2D());
		 m_waiting_times_graph->setXAxisLabel("Iteration");
		 m_waiting_times_graph->setYAxisLabel("Maximum waiting time [s]");
		 m_waiting_times_graph->setPlotMode(Gem::Common::graphPlotMode::CURVE);

		 m_returned_items_graph = std::shared_ptr<Gem::Common::GGraph2D>(new Gem::Common::GGraph2D());
		 m_returned_items_graph->setXAxisLabel("Iteration");
		 m_returned_items_graph->setYAxisLabel("Number of returned items");
		 m_returned_items_graph->setPlotMode(Gem::Common::graphPlotMode::CURVE);
	 }

	 /***************************************************************************/
	 /**
	  * The copy constructor
	  *
	  * @param cp A copy of another GBrokerConnector object
	  */
	 GBrokerExecutorT(const GBrokerExecutorT<processable_type> &cp)
		 : GBaseExecutorT<processable_type>(cp)
		 , m_srm(cp.m_srm)
		 , m_maxResubmissions(cp.m_maxResubmissions)
		 , m_waitFactor(cp.m_waitFactor)
		 , m_initialWaitFactor(cp.m_initialWaitFactor)
		 , m_gpd("Maximum waiting times and returned items", 1, 2) // Intentionally not copied
	 	 , m_waitFactorWarningEmitted(cp.m_waitFactorWarningEmitted)
	 	 , m_lastReturnTime(cp.m_lastReturnTime)
	 	 , m_lastAverage(cp.m_lastAverage)
	 	 , m_remainingTime(cp.m_remainingTime)
	 	 , m_maxTimeout(cp.m_maxTimeout)
	 {
		 m_gpd.setCanvasDimensions(std::make_tuple<std::uint32_t,std::uint32_t>(1200,1600));

		 m_waiting_times_graph = std::shared_ptr<Gem::Common::GGraph2D>(new Gem::Common::GGraph2D());
		 m_waiting_times_graph->setXAxisLabel("Iteration");
		 m_waiting_times_graph->setYAxisLabel("Maximum waiting time [s]");
		 m_waiting_times_graph->setPlotMode(Gem::Common::graphPlotMode::CURVE);

		 m_returned_items_graph = std::shared_ptr<Gem::Common::GGraph2D>(new Gem::Common::GGraph2D());
		 m_returned_items_graph->setXAxisLabel("Iteration");
		 m_returned_items_graph->setYAxisLabel("Number of returned items");
		 m_returned_items_graph->setPlotMode(Gem::Common::graphPlotMode::CURVE);
	 }

	 /***************************************************************************/
	 /**
	  * The destructor
	  *
	  * TODO: This is a hack. GBrokerExecutorT from factory will otherwise
	  * overwrite the file.
	  */
	 virtual ~GBrokerExecutorT()
	 {
		 // Register the plotter
		 m_gpd.registerPlotter(m_waiting_times_graph);
		 m_gpd.registerPlotter(m_returned_items_graph);

		 // Write out the result. This is a hack.
		 if(m_waiting_times_graph->currentSize() > 0.) {
			 m_gpd.writeToFile("maximumWaitingTimes.C");
		 }
	 }

	 /***************************************************************************/
	 /**
	  * A standard assignment operator for GBrokerExecutorT<processable_type> objects,
	  *
	  * @param cp A copy of another GBrokerExecutorT<processable_type> object
	  * @return A constant reference to this object
	  */
	 const GBrokerExecutorT<processable_type> &operator=(const GBrokerExecutorT<processable_type> &cp) {
		 GBrokerExecutorT<processable_type>::load(&cp);
		 return *this;
	 }

	 /***************************************************************************/
	 /**
	  * Loads the data of another GBrokerExecutorT object
	  *
	  * @param cp A constant pointer to another GBrokerExecutorT object
	  */
	 virtual void load(GBaseExecutorT<processable_type> const *const cp_base) override {
		 GBrokerExecutorT<processable_type> const *const cp = dynamic_cast<GBrokerExecutorT<processable_type> const *const>(cp_base);

		 if (!cp) { // nullptr
			 glogger
				 << "In GBrokerExecutorT<processable_type>::load(): Conversion error!" << std::endl
				 << GEXCEPTION;
		 }

		 // Load our parent classes data
		 GBaseExecutorT<processable_type>::load(cp);

		 // Local data
		 m_srm = cp->m_srm;
		 m_maxResubmissions = cp->m_maxResubmissions;
		 m_waitFactor = cp->m_waitFactor;
		 m_initialWaitFactor = cp->m_initialWaitFactor;
		 m_waitFactorWarningEmitted = cp->m_waitFactorWarningEmitted;
		 m_lastReturnTime = cp->m_lastReturnTime;
		 m_lastAverage = cp->m_lastAverage;
		 m_remainingTime = cp->m_remainingTime;
		 m_maxTimeout = cp->m_maxTimeout;
	 }

	 /***************************************************************************/
	 /**
	  * Adds local configuration options to a GParserBuilder object
	  *
	  * @param gpb The GParserBuilder object to which configuration options should be added
	  */
	 virtual void addConfigurationOptions(
		 Gem::Common::GParserBuilder &gpb
	 ) override {
		 // Call our parent class's function
		 GBaseExecutorT<processable_type>::addConfigurationOptions(gpb);

		 // Add local data

		 gpb.registerFileParameter<double>(
			 "waitFactor" // The name of the variable
			 , DEFAULTBROKERWAITFACTOR2 // The default value
			 , [this](double w) {
				 this->setWaitFactor(w);
			 }
		 )
			 << "A static double factor for timeouts" << std::endl
			 << "A wait factor <= 0 means \"no timeout\"." << std::endl
		    << "It is suggested to use values >= 1.";

		 gpb.registerFileParameter<double>(
			 "initialWaitFactor" // The name of the variable
			 , DEFAULTINITIALBROKERWAITFACTOR2 // The default value
			 , [this](double w) {
				 this->setInitialWaitFactor(w);
			 }
		 )
			 << "A static double factor for timeouts in the first iteration." << std::endl
		    << "Set this to the inverse of the number of parallel processing" << std::endl
			 << "units being used.";

		 gpb.registerFileParameter<std::size_t>(
			 "maxResubmissions" // The name of the variable
			 , DEFAULTMAXRESUBMISSIONS // The default value
			 , [this](std::size_t r) {
				 this->setMaxResubmissions(r);
			 }
		 )
			 << "The amount of resubmissions allowed if a full return of work" << std::endl
			 << "items was expected but only a subset has returned";
	 }

	 /***************************************************************************/
	 /**
	  * Allows to set the submission return mode. Depending on this setting,
	  * the object will wait indefinitely for items of the current submission to return,
	  * or will timeout and optionally resubmit unprocessed items.
	  */
	 void setSubmissionReturnMode(submissionReturnMode srm) {
		 m_srm = srm;
	 }

	 /***************************************************************************/
	 /**
	  * Allows to retrieve the current submission return mode
	  */
	 submissionReturnMode getSubmissionReturnMode() const {
		 return m_srm;
	 }

	 /***************************************************************************/
	 /**
	  * Specifies how often work items should be resubmitted in the case a full return
	  * of work items is expected.
	  *
	  * @param maxResubmissions The maximum number of allowed resubmissions
	  */
	 void setMaxResubmissions(std::size_t maxResubmissions) {
		 m_maxResubmissions = maxResubmissions;
	 }

	 /***************************************************************************/
	 /**
	  * Returns the maximum number of allowed resubmissions
	  *
	  * @return The maximum number of allowed resubmissions
	  */
	 std::size_t getMaxResubmissions() const {
		 return m_maxResubmissions;
	 }

	 /***************************************************************************/
	 /**
	  * Allows to set the wait factor to be applied to timeouts. A wait factor
	  * <= 0 indicates an indefinite waiting time.
	  */
	 void setWaitFactor(double waitFactor) {
		 m_waitFactor = waitFactor;
	 }

	 /***************************************************************************/
	 /**
	  * Allows to retrieve the wait factor variable
	  */
	 double getWaitFactor() const {
		 return m_waitFactor;
	 }

	 /***************************************************************************/
	 /**
	  * Allows to set the initial wait factor to be applied to timeouts. A wait factor
	  * <= 0 is not allowed.
	  */
	 void setInitialWaitFactor(double initialWaitFactor) {
		 if(initialWaitFactor <= 0.) {
			 glogger
				 << "In GBrokerExecutorT<processable_type>::setInitialWaitFactor(): Error!" << std::endl
				 << "Invalid wait factor " << initialWaitFactor << " supplied. Must be > 0."
				 << GEXCEPTION;
		 }
		 m_initialWaitFactor = initialWaitFactor;
	 }

	 /***************************************************************************/
	 /**
	  * Allows to retrieve the wait factor variable
	  */
	 double getInitialWaitFactor() const {
		 return m_initialWaitFactor;
	 }

	 /***************************************************************************/
	 /**
	  * General initialization function to be called prior to the first submission
	  */
	 virtual void init() override {
		 // To be called prior to all other initialization code
		 GBaseExecutorT<processable_type>::init();

		 // Make sure we have a valid buffer port
		 if (!m_CurrentBufferPort) {
			 m_CurrentBufferPort
				 = GBufferPortT_ptr(new Gem::Courtier::GBufferPortT<std::shared_ptr < processable_type>> ());
		 }

		 // Add the buffer port to the broker
		 GBROKER(processable_type)->enrol(m_CurrentBufferPort);
	 }

	 /***************************************************************************/
	 /**
	  * General finalization function to be called after the last submission
	  */
	 virtual void finalize() override {
		 // Get rid of the buffer port
		 m_CurrentBufferPort.reset();

		 // To be called after all other finalization code
		 GBaseExecutorT<processable_type>::finalize();
	 }

protected:
	 /***************************************************************************/
	 /**
	  * Allows to perform necessary setup work for an iteration
	  */
	 virtual void iterationInit(std::vector<std::shared_ptr<processable_type>>& workItems
		 , std::vector<bool> &workItemPos
		 , std::vector<std::shared_ptr<processable_type>>& oldWorkItems
	 ) override {
		 // Make sure the parent classes iterationInit function is executed first
		 // This function will also update the iteration start time
		 GBaseExecutorT<processable_type>::iterationInit(workItems, workItemPos, oldWorkItems);
	 }

	 /***************************************************************************/
	 /**
	  * Allows to perform necessary cleanup work for an iteration or do calculations
	  * for the next iteration.
	  */
	 virtual void iterationFinalize(
		 std::vector<std::shared_ptr<processable_type>>& workItems
		 , std::vector<bool> &workItemPos
		 , std::vector<std::shared_ptr<processable_type>>& oldWorkItems
	 ) override {
		 // Calculate average return times of work items.
		 m_lastAverage =
			 (GBaseExecutorT<processable_type>::m_returnedLast>0)
			 ? (m_lastReturnTime - GBaseExecutorT<processable_type>::m_submissionStartTime)/GBaseExecutorT<processable_type>::m_returnedLast
			 : (std::chrono::system_clock::now() - GBaseExecutorT<processable_type>::m_submissionStartTime)/GBaseExecutorT<processable_type>::m_expectedNumber; // this is an artificial number, as no items have returned

		 m_maxTimeout =
			 m_lastAverage
			 * boost::numeric_cast<double>(GBaseExecutorT<processable_type>::m_expectedNumber)
			 * m_waitFactor;
	 }

	 /***************************************************************************/
	 /**
	  * Waits for all items to return or possibly until a timeout has been reached.
	  */
	 virtual bool waitForReturn(
		 std::vector<std::shared_ptr<processable_type>>& workItems
		 , std::vector<bool> &workItemPos
		 , std::vector<std::shared_ptr<processable_type>>& oldWorkItems
	 ) override {
		 bool completed = false;

		 switch (m_srm) {
			 //------------------------------------------------------------------
			 // Wait for a given amount of time, decided upon by the function.
			 // Items that have not returned in time may return in a later iteration
			 case submissionReturnMode::INCOMPLETERETURN:
				 completed = this->waitForTimeOut(workItems, workItemPos, oldWorkItems);
				 break;

				 //------------------------------------------------------------------
				 // Wait for a given amount of time, decided upon by the function.
				 // If not all items have returned, re-submit work items up to a
				 // predefined number of times
			 case submissionReturnMode::RESUBMISSIONAFTERTIMEOUT:
				 completed = this->waitForTimeOutAndResubmit(workItems, workItemPos, oldWorkItems);
				 break;

				 //------------------------------------------------------------------
				 // Wait indefinitely, until all work items have returned
			 case submissionReturnMode::EXPECTFULLRETURN:
				 completed = this->waitForFullReturn(workItems, workItemPos, oldWorkItems);
				 break;

				 //------------------------------------------------------------------
				 // Fallback
			 default: {
				 glogger
					 << "In GBrokerExecutorT<>::waitForReturn(): Error!" << std::endl
					 << "Encountered an invalid submission return mode: " << m_srm << std::endl
					 << GEXCEPTION;
			 }
				 break;
				 //------------------------------------------------------------------
		 }

		 return completed;
	 }

private:
	 /***************************************************************************/
	 /**
	  * Submits a single work item.
	  *
	  * @param w The work item to be processed
	  */
	 virtual void submit(std::shared_ptr<processable_type> w_ptr) override {
#ifdef DEBUG
		 if(!w_ptr) {
			 glogger
				 << "In GBrokerExecutorT::submit(): Error!" << std::endl
				 << "Work item is empty" << std::endl
				 << GEXCEPTION;
		 }

		 if(!m_CurrentBufferPort) {
			 glogger
				 << "In GBrokerExecutorT::submit(): Error!" << std::endl
				 << "Current buffer port is empty when it shouldn't be" << std::endl
				 << GEXCEPTION;
		 }
#endif /* DEBUG */

		 // Store the id of the buffer port in the item
		 w_ptr->setBufferId(m_CurrentBufferPort->getUniqueTag());

	    // Perform the actual submission
		 m_CurrentBufferPort->push_raw(w_ptr);
	 }

	 /***************************************************************************/
	 /**
	  * Retrieves an item from the broker, waiting indefinitely for returns
	  *
	  * @return The obtained processed work item
	  */
	 std::shared_ptr<processable_type> retrieve() {
		 // Holds the retrieved item
		 std::shared_ptr<processable_type> w;
		 m_CurrentBufferPort->pop_processed(w);

		 return w;
	 }

	 /***************************************************************************/
	 /**
	  * Retrieves an item from the broker, waiting up to a given amount of time.
	  * The call will return earlier, if an item could already be retrieved.
	  *
	  * @return The obtained processed work item
	  */
	 std::shared_ptr<processable_type> retrieve(
		 const std::chrono::duration<double> &timeout
	 ) {
		 // Holds the retrieved item, if any
		 std::shared_ptr<processable_type> w_ptr;
		 m_CurrentBufferPort->pop_processed(w_ptr, timeout);
		 return w_ptr;
	 }

	 /***************************************************************************/
	 /**
	  * Updates the maximum allowed timeframe for calculations
	  */
	 void reviseMaxTime(std::size_t nReturnedCurrent) {
		 // Are we called for the first time in the first iteration)
		 if(nReturnedCurrent==0) {
			 std::chrono::duration<double> currentElapsed
				 = std::chrono::system_clock::now() - GBaseExecutorT<processable_type>::m_submissionStartTime;

			 if (this->getCurrentSubmissionId() == SUBMISSION_COUNTER_TYPE(0)) {
				 // Calculate a timeout for subsequent retrievals in this iteration. In the first iteration and for the first item,
				 // this timeout is the number of remaining items times the return time needed for the first item times a custom
				 // wait factor for the first submission. This may be very long, but takes care of a situation where there is only
				 // a single worker.
				 m_maxTimeout =
					 currentElapsed
					 * boost::numeric_cast<double>(GBaseExecutorT<processable_type>::m_expectedNumber)
					 * m_initialWaitFactor;
			 } else { // Not the first work item
				 std::chrono::duration<double> currentAverage = currentElapsed / ((std::max)(nReturnedCurrent, std::size_t(1))); // Avoid division by 0
				 m_maxTimeout =
					 currentAverage
					 * GBaseExecutorT<processable_type>::m_expectedNumber
					 * m_waitFactor;
			 }
		 } else {
#ifdef DEBUG
			 if(!m_waitFactorWarningEmitted) {
				 if(m_waitFactor > 0. && m_waitFactor < 1.) {
					 glogger
						 << "In GBrokerExecutorT::waitForTimeOut(): Warning" << std::endl
						 << "It is suggested not to use a wait time < 1. Current value: " << m_waitFactor << std::endl
						 << GWARNING;
				 }
			 }
#endif
		 }
	 }

	 /***************************************************************************/
	 /**
	  * Checks whether we have passed the maximum time frame. The function will
	  * also update the remaining time.
	  *
	  * @return A boolean indicating whether the maximum allowed time was passed
	  */
	 bool passedMaxTime(std::size_t nReturnedCurrent) {
		 std::chrono::duration<double> currentElapsed
		 	= std::chrono::system_clock::now() - GBaseExecutorT<processable_type>::m_submissionStartTime;

		 if (currentElapsed > m_maxTimeout) {
			 m_remainingTime = std::chrono::duration<double>(0.);
			 return true;
		 } else {
			 m_remainingTime = m_maxTimeout - currentElapsed;
			 return false;
		 }
	 }

	 /***************************************************************************/
	 /**
	  * Waits until a timeout occurs and returns, either complete (true) or
	  * incomplete (false). The algorithm works like this:
	  *
	  * In iteration n==0:
	  * - We have initially no indication how much time each calculation takes.
	  *   Hence we wait for the first return and measure the time. We then make
	  *   a very conservative estimate for the time needed for further returns as
	  *   "number of remaining items times the time needed for the first item
	  *   times an initial wait factor". This takes care of the case that there is
	  *   only a single client worker.
	  * - This estimate is then continuously revised for each new return
	  *
	  * In iteration n>0:
	  * - The timeout is calculated from the average time needed for the work items
	  *   of the previous iteration, times a wait factor
	  *
	  * @param workItems The work items to be processed
	  * @param workItemPos Booleans for each work item indicating whether processing has happened
	  * @param oldWorkItems A collection of old work items that have returned after a timeout
	  * @return A boolean indicating whether a complete return of items was achieved
	  */
	 bool waitForTimeOut(
		 std::vector<std::shared_ptr<processable_type>>& workItems
		 , std::vector<bool> &workItemPos
		 , std::vector<std::shared_ptr<processable_type>>& oldWorkItems
	 ) {
		 //----------------------------------------------------------------
		 // If the wait factor is == 0, we fall back to
		 // the "complete return" submission return mode.
		 if(m_waitFactor == 0.) {
			 return waitForFullReturn(
				 workItems
				 , workItemPos
				 , oldWorkItems
			 );
		 }

		 //----------------------------------------------------------------

		 std::shared_ptr<processable_type> w;
		 std::size_t nReturnedCurrent = 0;

		 // Check if this is the first iteration. If so, wait (possibly indefinitely)
		 // for the first item to return so we can estimate a suitable timeout
		 if (0 == this->getSubmissionId()) { // Wait indefinitely for first item
			 // Retrieve a single work item
			 w = this->retrieve();

			 // It is a severe error if no item is received in the first iteration.
			 // Note that retrieve() will wait indefinitely and, once it returns,
			 // should carry a work item.
			 if (!w) {
				 glogger
					 << "In GBrokerExecutorT<>::waitForTimeOut(): Error!" << std::endl
					 << "First item received in first iteration is empty. We cannot continue!"
					 << GEXCEPTION;
			 }

			 if (
				 this->addWorkItemAndCheckCompleteness(
					 w
					 , nReturnedCurrent
					 , workItems
					 , workItemPos
					 , oldWorkItems
				 )
			 ) {
				 // This covers the rare case that a "collection" of a *single*
				 // work item was submitted.
				 return true;
			 } else {
				 reviseMaxTime(0);
			 }
		 }

	 	 //------------------------------------

		 // Loop until a timeout is reached or all current items have returned
		 while (true) {
			 // Check if we have passed the maximum allowed time frame.
			 // This function will also update the remaining time.
			 if(passedMaxTime(nReturnedCurrent)) {
				 return false; // No complete return as we have reached the timeout
			 }

			 // Obtain the next item
			 w = retrieve(m_remainingTime);

			 // Leave if this was the last item
			 if (this->addWorkItemAndCheckCompleteness(
				 w
				 , nReturnedCurrent
				 , workItems
				 , workItemPos
				 , oldWorkItems
				)
			 ) {
				 break;
			 }

			 // Continuously revise the maxTimeout, if this is the first submission
			 if(w && 0 == this->getSubmissionId()) {
				 reviseMaxTime(nReturnedCurrent);
			 }
		 }

		 return true;
	 }

	 /***************************************************************************/
	 /**
	  * Waits until a timeout occurs, then resubmits missing items up to a maximum
	  * number of times. If m_maxResubmissions is set to 0, resubmission will happen
	  * without limit.
	  */
	 bool waitForTimeOutAndResubmit(
		 std::vector<std::shared_ptr<processable_type>>& workItems
		 , std::vector<bool> &workItemPos
		 , std::vector<std::shared_ptr<processable_type>>& oldWorkItems
	 ) {
		 bool completed = false;
		 std::size_t nResubmissions = 0;

		 do {
			 completed = waitForTimeOut(workItems, workItemPos, oldWorkItems);
		 } while (!completed && (m_maxResubmissions>0?(++nResubmissions < m_maxResubmissions):true));

		 return completed;
	 }

	 /***************************************************************************/
	 /**
	  * Waits (possibly indefinitely) until all items have returned. Note that this
	  * function may stall, if for whatever reason a work item does not return. If this
	  * is not acceptable, use either waitForTimeout() or waitForTimeoutAndResubmit()
	  * instead of this function. It is recommended to only use this function in
	  * environments that are considered safe in the sense that work items will practically
	  * always return. Local cluster environments will often fall into this category.
	  * There may not be returns from older iterations, which are attached to the end
	  * of the work item vector
	  */
	 bool waitForFullReturn(
		 std::vector<std::shared_ptr<processable_type>>& workItems
		 , std::vector<bool> &workItemPos
		 , std::vector<std::shared_ptr<processable_type>>& oldWorkItems
	 ) {
		 std::size_t nReturnedCurrent = 0;
		 while (!addWorkItemAndCheckCompleteness(
			 this->retrieve()
			 , nReturnedCurrent
			 , workItems
			 , workItemPos
			 , oldWorkItems
		 ));

		 return true;
	 }

	 /***************************************************************************/
	 /**
	  * Adds a work item to the corresponding vectors. This function assumes that
	  * the work item is valid, i.e. points to a valid object.
	  *
	  * @return A boolean indicating whether all work items of the current iteration were received
	  */
	 bool addWorkItemAndCheckCompleteness(
		 std::shared_ptr <processable_type> w_ptr
		 , std::size_t &nReturnedCurrent
		 , std::vector<std::shared_ptr<processable_type>>& workItems
		 , std::vector<bool> &workItemPos
		 , std::vector<std::shared_ptr<processable_type>>& oldWorkItems
	 ) {
		 // If we have been passed an empty item, simply continue the outer loop
		 if(!w_ptr) {
			 return false;
		 } else {
			 // Make the return time of the last item known
			 m_lastReturnTime = std::chrono::system_clock::now();
		 }

		 bool completed = false;
		 auto current_iteration = GBaseExecutorT<processable_type>::m_submission_counter;
		 auto w_iteration = w_ptr->getSubmissionCounter();

		 if (current_iteration == w_iteration) {
			 // Mark the position of the work item in the workItemPos vector and cross-check
			 std::size_t w_pos = w_ptr->getSubmissionPosition();

			 if (w_pos >= workItems.size()) {
				 glogger
					 << "In GBrokerExecutorT<processable_type>::addWorkItemAndCheckCompleteness(): Error!" << std::endl
					 << "Received work item for position " << w_pos << " while" << std::endl
					 << "only a range [0" << ", " << workItems.size() << "[ was expected." << std::endl
					 << GEXCEPTION;
			 }

			 if (GBC_UNPROCESSED == workItemPos.at(w_pos)) {
				 workItemPos.at(w_pos) = GBC_PROCESSED; // Successfully returned
				 workItems.at(w_pos) = w_ptr;
				 if (++nReturnedCurrent == GBaseExecutorT<processable_type>::m_expectedNumber) {
					 completed = true;
				 }
			 } // no else. Re-submitted items might return twice

		 } else { // It could be that a previous submission did not expect a full return. Hence older items may occur
			 oldWorkItems.push_back(w_ptr);
		 }

		 return completed;
	 }

	 /***************************************************************************/
	 /**
 	  * Allows to emit information at the end of an iteration
 	  * TODO: The content of this function is a hack. Submitted for current debugging purposes
 	  */
	 virtual void report() override {
		 std::chrono::duration<double> currentElapsed
			 = std::chrono::system_clock::now() - GBaseExecutorT<processable_type>::m_submissionStartTime;
		 auto current_iteration = GBaseExecutorT<processable_type>::m_submission_counter;

		 m_waiting_times_graph->add(std::make_tuple(boost::numeric_cast<double>(current_iteration), m_maxTimeout.count()));
		 m_returned_items_graph->add(std::make_tuple(boost::numeric_cast<double>(current_iteration), boost::numeric_cast<double>(this->getNReturned())));

		 if (0 == GBaseExecutorT<processable_type>::m_submission_counter) {
			 std::cout
				 << "Maximum waiting time in iteration " << current_iteration << ": " << m_maxTimeout.count()
				 << " s (" << currentElapsed.count() << ", "
				 << this->getNReturned() << " / " << GBaseExecutorT<processable_type>::m_expectedNumber << ", " << m_initialWaitFactor << ")" << std::endl;
		 } else {
			 std::cout
				 << "Maximum waiting time in iteration " << current_iteration << ": " << m_maxTimeout.count()
				 << " s (" << m_lastAverage.count() << ", "
				 << this->getNReturned() << " / " << GBaseExecutorT<processable_type>::m_expectedNumber << ", " << m_waitFactor << ")" << std::endl;
		 }
	 }

	 /***************************************************************************/
	 // Local data
	 submissionReturnMode m_srm = DEFAULTSRM; ///< Indicates how (long) the object shall wait for returns
	 std::size_t m_maxResubmissions = DEFAULTMAXRESUBMISSIONS; ///< The maximum number of re-submissions allowed if a full return of submitted items is attempted
	 double m_waitFactor = DEFAULTBROKERWAITFACTOR2; ///< A static factor to be applied to timeouts
	 double m_initialWaitFactor = DEFAULTINITIALBROKERWAITFACTOR2; ///< A static factor to be applied to timeouts in the first iteration

	 GBufferPortT_ptr m_CurrentBufferPort; ///< Holds a GBufferPortT object during the calculation. Note: It is neither serialized nor copied

	 Gem::Common::GPlotDesigner m_gpd; ///< A wrapper for the plots
	 std::shared_ptr<Gem::Common::GGraph2D> m_waiting_times_graph;  ///< The maximum waiting time resulting from the wait factor
	 std::shared_ptr<Gem::Common::GGraph2D> m_returned_items_graph;  ///< The maximum waiting time resulting from the wait factor

	 bool m_waitFactorWarningEmitted = false; ///< Specifies whether a warning about a small waitFactor has already been emitted

	 std::chrono::system_clock::time_point m_lastReturnTime = std::chrono::system_clock::now(); ///< Temporary that holds the time of the return of the last item of an iteration
	 std::chrono::duration<double> m_lastAverage = std::chrono::duration<double>(0.); ///< The average time needed for the last submission
	 std::chrono::duration<double> m_remainingTime = std::chrono::duration<double>(0.); ///< The remaining time in the current iteration
	 std::chrono::duration<double> m_maxTimeout = std::chrono::duration<double>(0.); ///< The maximum amount of time allowed for the entire calculation
};


/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/

} /* namespace Courtier */
} /* namespace Gem */

#endif /* GEXECUTOR_HPP_ */
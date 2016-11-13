/**
 * @file GBoundedBufferT.hpp
 */

/*
 * This file is part of the Geneva library collection.
 *
 * Note: this class was adapted from a circular_buffer test case
 * ([Boost 1.36 trunk version 17.7.08]/libs/circular_buffer/test/bounded_buffer_comparison.cpp )
 * by Jan Gaspar. The original code contained the following text:
 *
 ***************************************************************
 * Copyright (c) 2003-2007 Jan Gaspar
 * Use, modification, and distribution is subject to the Boost Software
 * License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 ***************************************************************
 *
 * As allowed by the license, modifications were applied to the code.
 * These are also covered by the Boost Software License, Version 1.0, and are
 * Copyright (C) Gemfony scientific UG (haftungsbeschraenkt)
 *
 * NOTE THAT THE BOOST-LICENSE DOES NOT APPLY TO ANY OTHER FILES OF THE
 * GENEVA LIBRARY, UNLESS THIS IS EXPLICITLY STATED IN THE CORRESPONDING FILE!
 *
 * See the AUTHORS file in the top-level directory for a list of authors.
 *
 * Contact: contact [at] gemfony (dot) eu
 *
 * Geneva was developed with kind support from Karlsruhe Institute of
 * Technology (KIT) and Steinbuch Centre for Computing (SCC). Further
 * information about KIT and SCC can be found at http://www.kit.edu/english
 * and http://scc.kit.edu .
 *
 * Geneva is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Boost Software License for more details.
 *
 * For further information on Gemfony scientific and Geneva, visit
 * http://www.gemfony.eu .
 */

/*
 * The following license applies to the code in this file:
 *
 * ***************************************************************************
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * ***************************************************************************
 */

// Global checks, defines and includes needed for all of Geneva
#include "common/GGlobalDefines.hpp"

// Standard headers go here

#include <string>
#include <iostream>
#include <fstream>
#include <deque>
#include <list>
#include <algorithm>
#include <stdexcept>
#include <tuple>
#include <mutex>
#include <condition_variable>
#include <chrono>

// Boost headers go here

#include <boost/lexical_cast.hpp>
#include <boost/utility.hpp>

#ifndef GBOUNDEDBUFFERT_HPP_
#define GBOUNDEDBUFFERT_HPP_

// Geneva headers go here
#include "common/GExceptions.hpp"
#include "common/GPlotDesigner.hpp"
#include "common/GLogger.hpp"
#include "common/GCommonEnums.hpp"

namespace Gem {
namespace Common {

/******************************************************************************/
/** @brief Class to be thrown as a message in the case of a time-out in GBuffer */
class condition_time_out : public std::exception
{ /* nothing */ };

/******************************************************************************/
/**
 * This class implements a bounded buffer. Items can be added to one
 * end by multiple threads and retrieved from the other, also by
 * multiple threads. When the buffer is full, attempts to add items
 * will block until there is again enough space. When the buffer is
 * empty, retrieval of items will block until new items have become
 * available. The class contains a "get" function that times out
 * when no item could be retrieved from the buffer. This allows
 * timeouts for data sinks. This can be important in situations
 * where sources might permanently or temporarily go away (e.g. due
 * to network failure). The underlying data structure is a
 * std::deque. The class works with condition variables.  Note that
 * this class assumes that an operator= is available for the items
 * stored in the buffer. Setting the template argument t_capacity to 0
 * results in an unbounded buffer, possibly useful for returning items,
 * if there may never be an "inflation".
 */
template<typename T, std::size_t t_capacity = DEFAULTBUFFERSIZE>
class GBoundedBufferT
	: private boost::noncopyable
{
public:

	 using container_type = std::deque<T>;
	 using value_type     = typename container_type::value_type;

	 /***************************************************************************/
	 /**
	  * The default constructor. Sets up a buffer of size DEFAULTBUFFERSIZE.
	  */
	 GBoundedBufferT()
	 { /* nothing */ }

	 /***************************************************************************/
	 /**
	  * A standard destructor. Virtual, as classes such as a producer-type
	  * class might get derived from this one. We do not want the destructor
	  * to throw, hence we try to catch all errors locally. Any error here
	  * means termination of the program. No logging takes place, as we want
	  * this class to be independent of the Geneva framework
	  */
	 virtual ~GBoundedBufferT() {
		 // Any error here is deadly ...
		 try {
			 std::unique_lock<std::mutex> lock(m_mutex);
			 m_container.clear();
		 }
		 // This is a standard error raised by the lock/mutex
		 catch (std::system_error& e) {
			 glogger
				 << "In GRandomFactory::producer(): Error!" << std::endl
				 << "Caught std::system_error exception with message" << std::endl
				 << e.what()
				 << "which might indicate that a mutex could not be locked." << std::endl
				 << GTERMINATION;
		 }
		 // We do not know whether any of the destructors of the items in the buffer throw anything
		 catch (...) {
			 glogger
				 << "Caught unknown exception in GBoundedBufferT::~GBoundedBufferT(). Terminating ..." << std::endl
				 << GTERMINATION;
		 }
	 }

	 /***************************************************************************/
	 /**
	  * Adds a single item to the front of the buffer. The function
	  * will block if there is no space in the buffer and continue once
	  * space is available.
	  *
	  * @param item An item to be added to the front of the buffer
	  */
	 void push_front(value_type item) {
		 std::unique_lock<std::mutex> lock(m_mutex);
		 // Note that this overload of wait() internally runs a loop on its predicate to
		 // deal with spurious wakeups
		 m_not_full.wait(
			 lock
			 , [&]() -> bool { return (t_capacity==0)?true:(m_container.size()<t_capacity); } // will always signal "not full" if t_capacity is == 0
		 );
		 m_container.push_front(std::move(item));
		 m_not_empty.notify_all();
	 }

	 /***************************************************************************/
	 /**
	  * Adds a single item to the front of the buffer. The function
	  * will time out after a given amount of time. This function was
	  * added to Jan Gaspar's original implementation.
	  *
	  * @param item An item to be added to the front of the buffer
	  * @param timeout duration until a timeout occurs
	  */
	 void push_front(value_type item, const std::chrono::duration<double> &timeout) {
		 std::unique_lock<std::mutex> lock(m_mutex);
		 if (!m_not_full.wait_for(
			 lock
			 , std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
			 , [&]() -> bool { return (t_capacity==0)?true:(m_container.size()<t_capacity); }
		 )) {
			 throw Gem::Common::condition_time_out();
		 }
		 m_container.push_front(std::move(item));
		 m_not_empty.notify_all();
	 }

	 /***************************************************************************/
	 /**
	  * Adds a single item to the front of the buffer. The function
	  * will time out after a given amount of time and return false
	  * in this case.
	  *
	  * @param item An item to be added to the front of the buffer
	  * @param timeout duration until a timeout occurs
	  * @return A boolean indicating whether an item has been successfully submitted
	  */
	 bool push_front_bool(value_type item, const std::chrono::duration<double> &timeout) {
		 std::unique_lock<std::mutex> lock(m_mutex);
		 if (!m_not_full.wait_for(
			 lock
			 , std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
			 , [&]() -> bool { return (t_capacity==0)?true:(m_container.size()<t_capacity); }
		 )) {
			 return false;
		 }
		 m_container.push_front(std::move(item));
		 m_not_empty.notify_all();
		 return true;
	 }

	 /***************************************************************************/
	 /**
	  * Retrieves a single item from the end of the buffer. The
	  * function will block if no items are available and will continue
	  * once items become available again.
	  *
	  * @param item Reference to a single item that was removed from the end of the buffer
	  */
	 void pop_back(value_type &item) {
		 std::unique_lock<std::mutex> lock(m_mutex);
		 m_not_empty.wait(
			 lock
			 , [&]() -> bool { return !m_container.empty(); }
		 );

		 item = std::move(m_container.back());
		 m_container.pop_back();
		 m_not_full.notify_all();
	 }

	 /***************************************************************************/
	 /**
	  * Retrieves a single item from the end of the buffer. The function
	  * will time out after a given amount of time. This function was
	  * added to Jan Gaspar's original implementation.
	  *
	  * @param item Reference to a single item that was removed from the end of the buffer
	  * @param timeout duration until a timeout occurs
	  */
	 void pop_back(value_type &item, const std::chrono::duration<double> &timeout) {
		 std::unique_lock<std::mutex> lock(m_mutex);
		 if (!m_not_empty.wait_for(
			 lock
			 , std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
			 , [&]() -> bool { return !m_container.empty(); }
		 )) {
			 throw Gem::Common::condition_time_out();
		 }

		 item = std::move(m_container.back());
		 m_container.pop_back();
		 m_not_full.notify_all();
	 }

	 /***************************************************************************/
	 /**
	  * Retrieves a single item from the end of the buffer. The function
	  * will time out after a given amount of time. It will return false
	  * in this case. "true" will be returned if an item could be retrieved
	  * successfully.
	  *
	  * @param item Reference to a single item that was removed from the end of the buffer
	  * @param timeout duration until a timeout occurs
	  * @return A boolean indicating whether an item has been successfully retrieved
	  */
	 bool pop_back_bool(value_type &item, const std::chrono::duration<double> &timeout) {
		 std::unique_lock<std::mutex> lock(m_mutex);
		 if (!m_not_empty.wait_for(
			 lock
			 , std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
			 , [&]() -> bool { return !m_container.empty(); }
		 )) {
			 return false;
		 }

		 item = std::move(m_container.back()); // Assign the item at the back of the container
		 m_container.pop_back(); // Remove it from the container
		 m_not_full.notify_all();
		 return true;
	 }

	 /***************************************************************************/
	 /**
	  * Retrieves the maximum allowed size of the buffer. No need for
	  * synchronization, as reading the value should be an atomic
	  * operation.
	  *
	  * @return The maximum allowed capacity
	  */
	 std::size_t getCapacity() const {
		 return t_capacity;
	 }

	 /***************************************************************************/
	 /**
	  * Retrieves the remaining space in the buffer. Note that the capacity
	  * may change once this function has completed. The information taken
	  * from this function can thus only serve as an indication.
	  *
	  * @return The currently remaining space in the buffer
	  */
	 std::size_t remainingSpace() {
		 std::unique_lock<std::mutex> lock(m_mutex);
		 return t_capacity - m_container.size();
	 }

	 /***************************************************************************/
	 /**
	  * Retrieves the current size of the buffer. Note that the buffer
	  * (and its size) may change immediately after this function has
	  * completed, this value should therefore only be taken as an
	  * indication.
	  *
	  * @return The current size of the buffer
	  */
	 std::size_t size() {
		 std::unique_lock<std::mutex> lock(m_mutex);
		 return m_container.size();
	 }

	 /***************************************************************************/
	 /**
	  * Returns whether the buffer is empty or not. Note that the buffer
	  * contents may change immediately after this function has
	  * completed, this value should therefore only be taken as an
	  * indication.
	  *
	  * @return True if the buffer is not empty
	  */
	 bool isNotEmpty() {
		 std::unique_lock<std::mutex> lock(m_mutex);
		 return !m_container.empty();
	 }

protected:
	 /***************************************************************************/

	 container_type m_container; ///< The actual data store
	 mutable std::mutex m_mutex; ///< Used for synchronization of access to the container
	 std::condition_variable m_not_empty; ///< Used for synchronization of access to the container
	 std::condition_variable m_not_full; ///< Used for synchronization of access to the container

private:
	 /***************************************************************************/

	 GBoundedBufferT(const GBoundedBufferT<T> &) = delete; ///< Disabled copy constructor
	 GBoundedBufferT &operator=(const GBoundedBufferT<T> &) = delete; ///< Disabled assign operator
};

/******************************************************************************/

} /* namespace Common */
} /* namespace Gem */

#endif /* GBOUNDEDBUFFERT_HPP_ */

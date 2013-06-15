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
 * See the AUTHORS file in the top-level directory for a list of authors.
 *
 * Contact: contact [at] gemfony (dot) com
 *
 * Geneva was developed with kind support from Karlsruhe Institute of
 * Technology (KIT) and Steinbuch Centre for Computing (SCC). Further
 * information about KIT and SCC can be found at http://www.kit.edu/english
 * and http://scc.kit.edu .
 *
 * In particular, please note that
 * Geneva is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Boost Software License for more details.
 *
 * Note that different licenses apply to other parts of the Geneva
 * library collection. These licenses are specified in each file of
 * the source code.
 */

/*
 * The following license applies to the code in this file:
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
 */

// Standard headers go here

#include <string>
#include <iostream>
#include <fstream>
#include <deque>
#include <list>
#include <algorithm>
#include <stdexcept>

// Includes check for correct Boost version(s)
#include "common/GGlobalDefines.hpp"

// Boost headers go here

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/utility.hpp>
#include <boost/cstdint.hpp>
#include <boost/date_time.hpp>
#include <boost/tuple/tuple.hpp>

#ifndef GBOUNDEDBUFFERT_HPP_
#define GBOUNDEDBUFFERT_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif


// Geneva headers go here
#include "common/GExceptions.hpp"
#include "common/GPlotDesigner.hpp"
#include "common/GLogger.hpp"

namespace Gem {
namespace Common {

/******************************************************************************/
/** @brief Class to be thrown as a message in the case of a time-out in GBuffer */
class condition_time_out: public std::exception {};

/******************************************************************************/
/**
 * By default the buffer will have this size. As the buffer
 * dynamically grows and shrinks, we choose a very high value. This
 * is a safeguard against errors like endless loops that might keep
 * filling the buffer until memory is exhausted. In normal work
 * conditions, however, the buffer should never reach its upper
 * limit.
 */
const std::size_t DEFAULTBUFFERSIZE = 20000;

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
 * stored in the buffer.
 */
template<typename T>
class GBoundedBufferT
	: private boost::noncopyable
{
public:

	typedef typename std::deque<T> container_type;
	// typedef typename std::list<T> container_type; // Tested to much slower than a deque
	typedef typename container_type::value_type value_type;

	/***************************************************************************/
	/**
	 * The default constructor. Sets up a buffer of size DEFAULTBUFFERSIZE.
	 */
	GBoundedBufferT()
		: capacity_(DEFAULTBUFFERSIZE)
#ifdef GEM_COMMON_BENCHMARK_BOUNDED_BUFFER
		, name_("no name")
#endif /* GEM_COMMON_BENCHMARK_BOUNDED_BUFFER */
	{
#ifdef GEM_COMMON_BENCHMARK_BOUNDED_BUFFER
		startTime_ = boost::posix_time::microsec_clock::local_time();
#endif /* GEM_COMMON_BENCHMARK_BOUNDED_BUFFER */
	}

	/***************************************************************************/
	/**
	 * A constructor that creates a buffer with custom size "capacity".
	 * It enforces a minimum buffer size of 1.
	 *
	 * @param capacity The desired size of the buffer
	 */
	explicit GBoundedBufferT(const std::size_t& capacity)
		: capacity_(capacity?capacity:1)
#ifdef GEM_COMMON_BENCHMARK_BOUNDED_BUFFER
		, name_("no name")
#endif /* GEM_COMMON_BENCHMARK_BOUNDED_BUFFER */
	{
#ifdef GEM_COMMON_BENCHMARK_BOUNDED_BUFFER
		startTime_ = boost::posix_time::microsec_clock::local_time();
#endif /* GEM_COMMON_BENCHMARK_BOUNDED_BUFFER */
	}

	/***************************************************************************/
	/**
	 * A standard destructor. Virtual, as classes such as a producer-type
	 * class might get derived from this one. We do not want the destructor
	 * to throw, hence we try to catch all errors locally. Any error here
	 * means termination of the program. No logging takes place, as we want
	 * this class to be independent of the Geneva framework
	 */
	virtual ~GBoundedBufferT()
	{
		// Any error here is deadly ...
		try {
			boost::mutex::scoped_lock lock(mutex_);
			container_.clear();
		}
		// This is a standard error raised by the lock/mutex
		catch(boost::thread_resource_error&) {
			std::cerr << "Caught thread_resource_error in GBoundedBufferT::~GBoundedBufferT(). Terminating ..." << std::endl;
			std::terminate();
		}
		// We do not know whether any of the destructors of the items in the buffer throw anything
		catch(...) {
			std::cerr << "Caught unknown exception in GBoundedBufferT::~GBoundedBufferT(). Terminating ..." << std::endl;
			std::terminate();
		}

#ifdef GEM_COMMON_BENCHMARK_BOUNDED_BUFFER
		// Find out when this object got destroyed
		endTime_ = boost::posix_time::microsec_clock::local_time();

		// Write out results
		emitPutAndGetTimes();
#endif /* GEM_COMMON_BENCHMARK_BOUNDED_BUFFER */
	}

	/***************************************************************************/
	/**
	 * Adds a single item to the front of the buffer. The function
	 * will block if there is no space in the buffer and continue once
	 * space is available.
	 *
	 * @param item An item to be added to the front of the buffer
	 */
	void push_front(value_type item)
	{
		boost::mutex::scoped_lock lock(mutex_);
		// Note that this overload of wait() internally runs a loop on is_not_full to
		// deal with spurious wakeups
		not_full_.wait(lock, boost::bind(&GBoundedBufferT<value_type>::is_not_full, this));
		container_.push_front(item);

#ifdef GEM_COMMON_BENCHMARK_BOUNDED_BUFFER
		// Update the puts_ vector
		long currentTime = (boost::posix_time::microsec_clock::local_time() - startTime_).total_microseconds();
		puts_.push_back(currentTime);
		entries_.push_back(boost::make_tuple<long, std::size_t>(currentTime, container_.size()));
#endif /* GEM_COMMON_BENCHMARK_BOUNDED_BUFFER */

		not_empty_.notify_one();
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
	void push_front(value_type item, const boost::posix_time::time_duration& timeout)
	{
		boost::mutex::scoped_lock lock(mutex_);
		if(!not_full_.timed_wait(lock,timeout,boost::bind(&GBoundedBufferT<value_type>::is_not_full, this))) {
			throw Gem::Common::condition_time_out();
		}
		container_.push_front(item);

#ifdef GEM_COMMON_BENCHMARK_BOUNDED_BUFFER
		// Update the puts_ vector
		long currentTime = (boost::posix_time::microsec_clock::local_time() - startTime_).total_microseconds();
		puts_.push_back(currentTime);
		entries_.push_back(boost::make_tuple<long, std::size_t>(currentTime, container_.size()));
#endif /* GEM_COMMON_BENCHMARK_BOUNDED_BUFFER */

		not_empty_.notify_one();
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
	bool push_front_bool(value_type item, const boost::posix_time::time_duration& timeout)
	{
		boost::mutex::scoped_lock lock(mutex_);
		if(!not_full_.timed_wait(lock, timeout, boost::bind(&GBoundedBufferT<value_type>::is_not_full, this))) {
			return false;
		}
		container_.push_front(item);

#ifdef GEM_COMMON_BENCHMARK_BOUNDED_BUFFER
		// Update the puts_ vector
		long currentTime = (boost::posix_time::microsec_clock::local_time() - startTime_).total_microseconds();
		puts_.push_back(currentTime);
		entries_.push_back(boost::make_tuple<long, std::size_t>(currentTime, container_.size()));
#endif /* GEM_COMMON_BENCHMARK_BOUNDED_BUFFER */

		not_empty_.notify_one();
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
	void pop_back(value_type& item)
	{
		boost::mutex::scoped_lock lock(mutex_);
		not_empty_.wait(lock, boost::bind(&GBoundedBufferT<value_type>::is_not_empty, this));

#ifdef DEBUG
		if(container_.empty()) {
		   glogger
		   << "In GBoundedBufferT<T>::pop_back(item): Container is empty when it shouldn't be!" << std::endl
		   << GEXCEPTION;
		}
#endif /* DEBUG */

		item = container_.back();
		container_.pop_back();

#ifdef GEM_COMMON_BENCHMARK_BOUNDED_BUFFER
		// Update the gets_ vector
		long currentTime = (boost::posix_time::microsec_clock::local_time() - startTime_).total_microseconds();
		gets_.push_back(currentTime);
		entries_.push_back(boost::make_tuple<long, std::size_t>(currentTime, container_.size()));
#endif /* GEM_COMMON_BENCHMARK_BOUNDED_BUFFER */

		not_full_.notify_one();
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
	void pop_back(value_type& item, const boost::posix_time::time_duration& timeout)
	{
		boost::mutex::scoped_lock lock(mutex_);
		if(!not_empty_.timed_wait(lock,timeout,boost::bind(&GBoundedBufferT<value_type>::is_not_empty, this))) {
			throw Gem::Common::condition_time_out();
		}

#ifdef DEBUG
		if(container_.empty()) {
		   glogger
		   << "In GBoundedBufferT<T>::pop_back(item,timeout): Container is empty when it shouldn't be!" << std::endl
		   << GEXCEPTION;
		}
#endif /* DEBUG */

		item = container_.back();
		container_.pop_back();

#ifdef GEM_COMMON_BENCHMARK_BOUNDED_BUFFER
		// Update the gets_ vector
		long currentTime = (boost::posix_time::microsec_clock::local_time() - startTime_).total_microseconds();
		gets_.push_back(currentTime);
		entries_.push_back(boost::make_tuple<long, std::size_t>(currentTime, container_.size()));
#endif /* GEM_COMMON_BENCHMARK_BOUNDED_BUFFER */

		not_full_.notify_one();
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
	bool pop_back_bool(value_type& item, const boost::posix_time::time_duration& timeout)
	{
		boost::mutex::scoped_lock lock(mutex_);
		if(!not_empty_.timed_wait(lock,timeout,boost::bind(&GBoundedBufferT<value_type>::is_not_empty, this))) {
			return false;
		}

#ifdef DEBUG
		if(container_.empty()) {
		   glogger
		   << "In GBoundedBufferT<T>::pop_back_bool(item,timeout): Container is empty when it shouldn't be!" << std::endl
		   << GEXCEPTION;
		}
#endif /* DEBUG */

		item = container_.back(); // Assign the item at the back of the container
		container_.pop_back(); // Remove it from the container

#ifdef GEM_COMMON_BENCHMARK_BOUNDED_BUFFER
		// Update the gets_ vector
		long currentTime = (boost::posix_time::microsec_clock::local_time() - startTime_).total_microseconds();
		gets_.push_back(currentTime);
		entries_.push_back(boost::make_tuple<long, std::size_t>(currentTime, container_.size()));
#endif /* GEM_COMMON_BENCHMARK_BOUNDED_BUFFER */

		not_full_.notify_one();
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
	std::size_t getCapacity() const
	{
		return capacity_;
	}

	/***************************************************************************/
	/**
	 * Retrieves the remaining space in the buffer. Note that the capacity
	 * may change once this function has completed. The information taken
	 * from this function can thus only serve as an indication.
	 *
	 * @return The currently remaining space in the buffer
	 */
	std::size_t remainingSpace()
	{
		boost::mutex::scoped_lock lock(mutex_);
		return capacity_ - container_.size();
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
	std::size_t size()
	{
		boost::mutex::scoped_lock lock(mutex_);
		return container_.size();
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
	bool isNotEmpty()
	{
		boost::mutex::scoped_lock lock(mutex_);
		return !container_.empty();
	}

#ifdef GEM_COMMON_BENCHMARK_BOUNDED_BUFFER
	/***************************************************************************/
	/**
	 * Allows to assign a name to this object
	 *
	 * @param name The name to be assigned to this object
	 */
	void setName(const std::string& name) {
		name_ = name;
	}

	/***************************************************************************/
	/**
	 * Allows to retrieve this object's name
	 *
	 * @return The assigned name of this object
	 */
	std::string getName() const {
		return name_;
	}

	/***************************************************************************/
#endif /* GEM_COMMON_BENCHMARK_BOUNDED_BUFFER */

protected:
	/***************************************************************************/
	/*
	 * We want to be able to add custom producer threads. Hence the
	 * following functions are all protected, not private.
	 */

	/**
	 * A helper function needed for the condition variables. It is only called
	 * in a safe context, where a mutex has been locked. Hence we do not need
	 * any local synchronization.
	 *
	 * @return A boolean value indicating whether the buffer is not empty
	 */
	bool is_not_empty() const {
		return !container_.empty();
	}

	/**
	 * A helper function needed for the condition variables. It is only called
	 * in a safe context, where a mutex has been locked. Hence we do not need
	 * any local synchronization.
	 *
	 * @return A boolean value indicating whether the buffer is not full
	 */
	bool is_not_full() const 	{
		return (container_.size() < capacity_);
	}

	const std::size_t capacity_; ///< The maximum allowed size of the container
	container_type container_; ///< The actual data store
	mutable boost::mutex mutex_; ///< Used for synchronization of access to the container
	boost::condition_variable not_empty_; ///< Used for synchronization of access to the container
	boost::condition_variable not_full_; ///< Used for synchronization of access to the container

private:
	/***************************************************************************/
	GBoundedBufferT(const GBoundedBufferT&); ///< Disabled copy constructor
	GBoundedBufferT& operator = (const GBoundedBufferT&); ///< Disabled assign operator

#ifdef GEM_COMMON_BENCHMARK_BOUNDED_BUFFER
	/***************************************************************************/
	/**
	 * Writes out put- and get times. You can evaluate the results using
	 * the root analysis framework (see http://root.cern.ch)
	 */
	void emitPutAndGetTimes() {
      long totalMicroseconds = (endTime_ - startTime_).total_microseconds();

      // Create the plot objects
		boost::shared_ptr<GHistogram1D> gets_ptr(new GHistogram1D(1000, 0, totalMicroseconds));
		gets_ptr->setPlotLabel(std::string("timing of pop_back calls (") +  name_ + std::string(" / ") + boost::posix_time::to_simple_string(startTime_) + std::string(")"));
		gets_ptr->setXAxisLabel("microsceonds after start");
		gets_ptr->setYAxisLabel("number of pop_back calls");

		boost::shared_ptr<GHistogram1D> puts_ptr(new GHistogram1D(1000, 0, totalMicroseconds));
		puts_ptr->setPlotLabel(std::string("timing of push_front calls (") +  name_ + std::string(" / ") + boost::posix_time::to_simple_string(startTime_) + std::string(")"));
      puts_ptr->setXAxisLabel("microsceonds after start");
      puts_ptr->setYAxisLabel("number of push_front calls");

		boost::shared_ptr<GGraph2D> entries_ptr(new GGraph2D());
		entries_ptr->setPlotLabel("buffer size");
		entries_ptr->setXAxisLabel("microsceonds after start");
		entries_ptr->setYAxisLabel("number of remaining entries in the buffer");

		// Fill with data
      for(std::size_t i=0; i<gets_.size(); i++) *gets_ptr & gets_[i];
      for(std::size_t i=0; i<puts_.size(); i++) *puts_ptr & puts_[i];
      for(std::size_t i=0; i<entries_.size(); i++) *entries_ptr & entries_[i];

      // Create the canvas object and save it to disk
      GPlotDesigner gpd("GBoundedBufferT timings", 1,3);

      gpd.setCanvasDimensions(800,1200);
      gpd.registerPlotter(gets_ptr);
      gpd.registerPlotter(puts_ptr);
      gpd.registerPlotter(entries_ptr);

      gpd.writeToFile((boost::lexical_cast<std::string>(this) + "-result.C").c_str());
	}

	/***************************************************************************/

	std::string name_; ///< A name to be assigned to this object

	boost::posix_time::ptime startTime_; ///< Holds information about the construction time of this object
	boost::posix_time::ptime endTime_; ///< Holds information about the destruction time of this object
	std::vector<boost::tuple<long, std::size_t> > entries_;

	std::vector<long> gets_, puts_; ///< Holds information about submission- and retrieval-times

#endif /* GEM_COMMON_BENCHMARK_BOUNDED_BUFFER */
};

/******************************************************************************/

} /* namespace Common */
} /* namespace Gem */

#endif /* GBOUNDEDBUFFERT_HPP_ */

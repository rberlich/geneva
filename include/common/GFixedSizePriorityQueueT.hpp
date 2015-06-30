/**
 * @file GFixedSizePriorityQueue.hpp
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
#include <deque>
#include <algorithm>
#include <functional>

// Boost headers go here
#include <boost/function.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/deque.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/tracking.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/export.hpp>

#ifndef GFIXEDSIZEPRIORITYQUEUET_HPP_
#define GFIXEDSIZEPRIORITYQUEUET_HPP_

// Geneva headers go here
#include "common/GHelperFunctionsT.hpp"
#include "common/GExceptions.hpp"
#include <common/GExpectationChecksT.hpp>
#include "common/GLogger.hpp"
#include "common/GMathHelperFunctionsT.hpp"
#include "common/GCommonInterfaceT.hpp"

namespace Gem {
namespace Common {

/******************************************************************************/
/**
 * This class implements a fixed-size priority queue. Note that data items
 * are held inside of std::shared_ptr objects and must be copy-constructible.
 * It is also required that T can be compared using operator== and operator!= .
 * A maxSize_ of 0 stands for an unlimited size of the data vector.
 *
 * IMPORTANT: This class assumes that T has a member function clone<T>()
 * which returns a std::shared_ptr<T> as a copy of the T object.
 */
template<typename T>
class GFixedSizePriorityQueueT
	: public GCommonInterfaceT<GFixedSizePriorityQueueT<T>>
{
	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive &ar, const unsigned int) {
		using boost::serialization::make_nvp;

		ar
		& make_nvp("GCommonInterfaceT_GFixedSizePriorityQueueT_T", boost::serialization::base_object<GCommonInterfaceT<GFixedSizePriorityQueueT<T>>>(*this))
		& BOOST_SERIALIZATION_NVP(data_)
		& BOOST_SERIALIZATION_NVP(maxSize_)
		& BOOST_SERIALIZATION_NVP(higherIsBetter_);
	}
	///////////////////////////////////////////////////////////////////////

public:
	/***************************************************************************/
	/**
	 * The default constructor. Note that some variables may be initialized in the class body.
	 */
	GFixedSizePriorityQueueT()
		: GFixedSizePriorityQueueT(10, false /* higherIsBetter_ */ )
	{ /* nothing */ }

	/***************************************************************************/
	/**
	 * Initialization with the maximum number of entries
	 *
	 * @param maxSize The maximum size of the queue
	 */
	explicit GFixedSizePriorityQueueT(const std::size_t &maxSize)
		: GFixedSizePriorityQueueT(maxSize, false /* higherIsBetter_ */ )
	{ /* nothing */ }

	/***************************************************************************/
	/**
	 * Initialization with the maximum number of entries and the information,
	 * whether higher or lower evaluations are better.
	 *
	 * @param maxSize The maximum size of the queue
	 */
	GFixedSizePriorityQueueT(
		const std::size_t &maxSize
		, const bool &higherIsBetter
	)
		: data_()
		, maxSize_(maxSize)
		, higherIsBetter_(higherIsBetter)
	{ /* nothing */ }

	/***************************************************************************/
	/**
	 * The copy constructor
	 */
	GFixedSizePriorityQueueT(const GFixedSizePriorityQueueT<T> &cp)
		: maxSize_(cp.maxSize_)
		, higherIsBetter_(cp.higherIsBetter_)
	{
		for(auto cit: data_) { // std::shared_ptr may be copied
			data_.push_back(cit->template clone<T>());
		}
	}

	/***************************************************************************/
	/**
	 * The destructor
	 */
	virtual ~GFixedSizePriorityQueueT() {
		data_.clear();
	}

	/***************************************************************************/
	/**
	 * A standard assignment operator
	 */
	const GFixedSizePriorityQueueT<T>& operator=(const GFixedSizePriorityQueueT<T>& cp) {
		this->load_(&cp);
		return *this;
	}

	/***************************************************************************/
	/**
	 * Checks for equality with another GFixedSizePriorityQueueT<T> object
	 *
	 * @param  cp A constant reference to another GFixedSizePriorityQueueT<T> object
	 * @return A boolean indicating whether both objects are equal
	 */
	bool operator==(const GFixedSizePriorityQueueT<T>& cp) const {
		using namespace Gem::Common;
		try {
			this->compare(cp, CE_EQUALITY, CE_DEF_SIMILARITY_DIFFERENCE);
			return true;
		} catch(g_expectation_violation&) {
			return false;
		}
	}

	/***************************************************************************/
	/**
	 * Checks for inequality with another GFixedSizePriorityQueueT<T> object
	 *
	 * @param  cp A constant reference to another GFixedSizePriorityQueueT<T> object
	 * @return A boolean indicating whether both objects are inequal
	 */
	bool operator!=(const GFixedSizePriorityQueueT<T>& cp) const {
		using namespace Gem::Common;
		try {
			this->compare(cp, CE_INEQUALITY, CE_DEF_SIMILARITY_DIFFERENCE);
			return true;
		} catch(g_expectation_violation&) {
			return false;
		}
	}

	/***************************************************************************/
	/**
	 * Gives access to the best item without copying it
	 */
	std::shared_ptr <T> best() const {
		if (data_.empty()) {
			// Throw an exception
			glogger
			<< "In GFixedSizePriorityQueueT<T>::best(): Error!" << std::endl
			<< "Priority queue is empty." << std::endl
			<< GEXCEPTION;

			// Make the compiler happy
			return std::shared_ptr<T>();
		} else {
			return data_.front();
		}
	}

	/***************************************************************************/
	/**
	 * Gives access to the worst item without copying it
	 */
	std::shared_ptr <T> worst() const {
		if (data_.empty()) {
			// Throw an exception
			glogger
			<< "In GFixedSizePriorityQueueT<T>::best(): Error!" << std::endl
			<< "Priority queue is empty." << std::endl
			<< GEXCEPTION;

			// Make the compiler happy
			return std::shared_ptr<T>();
		} else {
			return data_.back();
		}
	}

	/***************************************************************************/
	/**
	 * Allows to set the priority mode. A value of "true" means that higher
	 * values are considered better, "false" means that lower values are
	 * considered to be better.
	 */
	void setMaxMode(bool maxMode) {
		higherIsBetter_ = maxMode;
	}

	/***************************************************************************/
	/**
	 * Allows to retrieve the current value of higherIsBetter_
	 */
	bool getMaxMode() const {
		return higherIsBetter_;
	}

	/***************************************************************************/
	/**
	 * Add an item to the queue. Note that the comparator used in this function
	 * should sort the data in descending order (assuming that higher
	 * values are better) or ascending order (if lower values are better),
	 * so that the worst items are always at the end of the queue.
	 *
	 * @param item The item to be added to the queue
	 * @param do_clone If set to true, work items will be cloned. Otherwise only the smart pointer will be added
	 */
	virtual void add(
		std::shared_ptr <T> item
		, bool do_clone = false
	) {
		// Add the work item to the queue
		// - If the queue is unlimited
		// - If the queue isn't full yet
		// - If the item is better than the worst one contained in the queue
		if (0 == maxSize_ || data_.size() < maxSize_ ||
			 isBetter(this->evaluation(item), this->evaluation(this->worst()))) {
			if (do_clone) {
				data_.push_back(item->template clone<T>());
			} else {
				data_.push_back(item);
			}
		}

		// Sort the data according to their ids, so we may remove duplicates
		std::sort(
			data_.begin(), data_.end(), [this](const std::shared_ptr <T> &x, const std::shared_ptr <T> &y) -> bool {
				return this->id(x) < this->id(y);
			}
		);

		// Remove duplicate items
		data_.erase(
			std::unique(
				data_.begin(), data_.end(), [this](const std::shared_ptr <T> &x, const std::shared_ptr <T> &y) -> bool {
					return this->id(x) == this->id(y);
				}
			), data_.end()
		);

		// Sort the data according to the evaluation
		std::sort(
			data_.begin(), data_.end(), [this](const std::shared_ptr <T> &x, const std::shared_ptr <T> &y) -> bool {
				if (this->getMaxMode()) { // higher is better
					if (this->evaluation(x) > this->evaluation(y)) return true;
					else return false;
				} else {
					if (this->evaluation(x) < this->evaluation(y)) return true;
					else return false;
				}
			}
		);

		// Remove surplus work items, if the queue has reached the corresponding size
		// As the worst items are not at the end of the queue, they will be removed, if
		// they are beyond the allowed size. This will only have an effect if maxSize_ is != 0 .
		if (maxSize_ && data_.size() > maxSize_) {
			data_.resize(maxSize_);
		}
	}

	/***************************************************************************/
	/**
	 * Add a set of items to the queue. Note that the comparator used in this
	 * function should sort the data in descending order (assuming that higher
	 * values are better) or ascending order (if lower values are better),
	 * so that the worst items are always at the end of the queue.
	 *
	 * @param items The items to be added to the queue
	 * @param do_clone If set to true, work items will be cloned. Otherwise only the smart pointer will be added
	 * @param replace If set to true, the queue will be emptied before adding new work items
	 */
	virtual void add(
		const std::vector<std::shared_ptr < T>>& items
		, bool do_clone = false
		, bool replace = false
	) {
		double worstKnownEvaluation = Gem::Common::getWorstCase<double>(higherIsBetter_);
		if (true == replace || data_.empty()) {
			data_.clear();
		} else {
			// Data already exists, we know better than the worst known valid
			worstKnownEvaluation = this->evaluation(this->worst());
		}

		// At this point, worstKnownEvaluation will be
		// - the worst case, if the queue is empty or all entries in the queue will be replaced
		// - the evaluation of the worst entry in the queue if we only add items (regardless of whether they will be cloned or not)
		typename std::vector<std::shared_ptr < T>> ::const_iterator
		cit;
		for (cit = items.begin(); cit != items.end(); ++cit) {
			// Add the work item to the queue
			// - If the queue is unlimited
			// - If the queue isn't full yet
			// - If the item is better than the worst one already contained in the queue
			if (0 == maxSize_ || data_.size() < maxSize_ || isBetter(this->evaluation(*cit), worstKnownEvaluation)) {
				if (do_clone) {
					data_.push_back((*cit)->template clone<T>());
				} else {
					data_.push_back(*cit);
				}
			}
		}

		// Sort the data according to their ids, so we may remove duplicates
		std::sort(
			data_.begin(), data_.end(), [this](const std::shared_ptr <T> &x, const std::shared_ptr <T> &y) -> bool {
				return this->id(x) < this->id(y);
			}
		);

		// Remove duplicate items
		data_.erase(
			std::unique(
				data_.begin(), data_.end(), [this](const std::shared_ptr <T> &x, const std::shared_ptr <T> &y) -> bool {
					return this->id(x) == this->id(y);
				}
			), data_.end()
		);

		std::sort(
			data_.begin(), data_.end(), [this](const std::shared_ptr <T> &x, const std::shared_ptr <T> &y) -> bool {
				if (this->getMaxMode()) { // higher is better
					if (this->evaluation(x) > this->evaluation(y)) return true;
					else return false;
				} else {
					if (this->evaluation(x) < this->evaluation(y)) return true;
					else return false;
				}
			}
		);

		// Remove surplus work items, if the queue has reached the corresponding size
		// This will only have an effect if maxSize_ is != 0
		if (maxSize_ && data_.size() > maxSize_) {
			data_.resize(maxSize_);
		}
	}

	/***************************************************************************/
	/**
	 * Removes the best item from the queue and returns it
	 */
	T pop() {
		if (data_.empty()) {
			// Throw an exception
			glogger
			<< "In GFixedSizePriorityQueueT<T>::pop(): Error!" << std::endl
			<< "Priority queue is empty." << std::endl
			<< GEXCEPTION;

			// Make the compiler happy
			return (T) 0;
		} else {
			T item = data_.front();
			data_.pop_front();
			return item;
		}
	}

	/***************************************************************************/
	/**
	 * Converts the local deque to a std::vector and returns it
	 */
	std::vector<std::shared_ptr < T>> toVector() {
		std::vector<std::shared_ptr < T>> result;

		typename std::deque<std::shared_ptr < T>> ::iterator
		it;
		for (it = data_.begin(); it != data_.end(); ++it) {
			result.push_back(*it);
		}

		return result;
	}

	/***************************************************************************/
	/**
	 * Returns the current size of the queue
	 */
	std::size_t size() const {
		return data_.size();
	}

	/***************************************************************************/
	/**
	 * Checks whether the data is empty
	 */
	bool empty() const {
		return data_.empty();
	}

	/***************************************************************************/
	/**
	 * Allows to clear the queue
	 */
	void clear() {
		data_.clear();
	}

	/***************************************************************************/
	/**
	 * Sets the maximum size of the priority queue
	 */
	void setMaxSize(std::size_t maxSize) {
		// Make sure the current size of data_ complies with maxSize
		if (data_.size() > maxSize) {
			data_.resize(maxSize);
		}

		maxSize_ = maxSize;
	}

	/***************************************************************************/
	/**
	 * Retrieves the maximum size of the priority queue
	 */
	std::size_t getMaxSize() const {
		return maxSize_;
	}

	/***************************************************************************/
	/**
	 * Returns the name of this class
	 */
	virtual std::string name() const override {
		return std::string("GFixedSizePriorityQueueT<T>");
	}

	/***************************************************************************/
	/**
	 * Checks for compliance with expectations with respect to another object
	 * of the same type
	 *
	 * @param cp A constant reference to another GFixedSizePriorityQueueT<T> object
	 * @param e The expected outcome of the comparison
	 * @param limit The maximum deviation for floating point values (important for similarity checks)
	 */
	virtual void compare(
		const GFixedSizePriorityQueueT<T> &cp
		, const Gem::Common::expectation &e
		, const double &limit
	) const override {
		using namespace Gem::Common;

		// Check that we are dealing with a GFixedSizePriorityQueueT<T> reference independent of this object and convert the pointer
		const GFixedSizePriorityQueueT<T> *p_load = Gem::Common::g_convert_and_compare<GFixedSizePriorityQueueT<T>, GFixedSizePriorityQueueT<T>>(cp, this);

		GToken token("GFixedSizePriorityQueueT<T>", e);

		// Compare our parent data ...
		Gem::Common::compare_base<GCommonInterfaceT<GFixedSizePriorityQueueT<T>>>(IDENTITY(*this, *p_load), token);

		// ... and then our local data
		compare_t(IDENTITY(data_, p_load->data_), token);
		compare_t(IDENTITY(maxSize_, p_load->maxSize_), token);
		compare_t(IDENTITY(higherIsBetter_, p_load->higherIsBetter_), token);

		// React on deviations from the expectation
		token.evaluate();
	}

protected:
	/***************************************************************************/
	/**
	 * Loads the data of another GFixedSizePriorityQueue<T> object
	 */
	virtual void load_(const GFixedSizePriorityQueueT<T> *cp) override {
		// Check that we are dealing with a GFixedSizePriorityQueueT<T> reference independent of this object and convert the pointer
		const GFixedSizePriorityQueueT<T> *p_load = Gem::Common::g_convert_and_compare<GFixedSizePriorityQueueT<T>, GFixedSizePriorityQueueT<T>>(cp, this);

		// Load local data
		Gem::Common::copyCloneableSmartPointerContainer(p_load->data_, data_);
		maxSize_ = p_load->maxSize_;
		higherIsBetter_ = p_load->higherIsBetter_;
	}

	/***************************************************************************/
	/** @brief Creates a deep clone of this object */
	virtual GFixedSizePriorityQueueT<T> * clone_() const override = 0;

	/***************************************************************************/
	/**
	 * Checks whether value x is better than value y
	 */
	bool isBetter(std::shared_ptr <T> new_item, std::shared_ptr <T> old_item) const {
		if (higherIsBetter_) {
			if (this->evaluation(new_item) > this->evaluation(old_item)) return true;
			else return false;
		} else { // lower is better
			if (this->evaluation(new_item) < this->evaluation(old_item)) return true;
			else return false;
		}
	}

	/***************************************************************************/
	/**
	 * Checks whether value x is better than value y
	 */
	bool isBetter(std::shared_ptr <T> new_item, const double &old_item) const {
		if (higherIsBetter_) {
			if (this->evaluation(new_item) > old_item) return true;
			else return false;
		} else { // lower is better
			if (this->evaluation(new_item) < old_item) return true;
			else return false;
		}
	}

	/***************************************************************************/
	/**
	 * Checks whether value x is better than value y
	 */
	bool isBetter(const double &new_item, std::shared_ptr <T> old_item) const {
		if (higherIsBetter_) {
			if (new_item > this->evaluation(old_item)) return true;
			else return false;
		} else { // lower is better
			if (new_item < this->evaluation(old_item)) return true;
			else return false;
		}
	}

	/***************************************************************************/
	/**
	 * Checks whether value x is better than value y
	 */
	bool isBetter(const double &new_item, const double &old_item) const {
		if (higherIsBetter_) {
			if (new_item > old_item) return true;
			else return false;
		} else { // lower is better
			if (new_item < old_item) return true;
			else return false;
		}
	}

	/***************************************************************************/
	/** @brief Evaluates a single work item, so that it can be sorted */
	virtual double evaluation(const std::shared_ptr <T> &) const BASE = 0;

	/** @brief Returns a unique id for a work item */
	virtual std::string id(const std::shared_ptr <T> &) const BASE = 0;

	std::deque<std::shared_ptr < T>> data_; ///< Holds the actual data

	std::size_t maxSize_ = 10; ///< The maximum number of work-items
	bool higherIsBetter_ = false; ///< Indicates whether higher evaluations of items indicate a higher priority
};

/******************************************************************************/

} /* namespace Common */
} /* namespace Gem */

#endif /* GFIXEDSIZEPRIORITYQUEUET_HPP_ */

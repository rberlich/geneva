/**
 * @file GConstrainedNumT.hpp
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

// Boost headers go here
#include <boost/math/special_functions/next.hpp>

#ifndef GCONSTRAINEDNUMT_HPP_
#define GCONSTRAINEDNUMT_HPP_

// Geneva headers go here
#include "common/GExceptions.hpp"
#include "common/GTypeToStringT.hpp"
#include "geneva/GObject.hpp"
#include "geneva/GParameterT.hpp"
#include "geneva/GConstrainedValueLimitT.hpp"

namespace Gem {
namespace Geneva {

/******************************************************************************/
/* The GConstrainedNumT class represents a numeric value, such as an int or a double,
 * equipped with the ability to adapt itself. The value range can have an upper and a lower
 * limit.  Adapted values will only appear inside the given range to the user, while they are
 * internally represented as a continuous range of values. Note that appropriate adaptors
 * (see e.g the GDoubleGaussAdaptor class) need to be loaded in order to benefit from the
 * adaption capabilities. It depends on the implementation of derived classes whether boundaries
 * are inclusive or exclusive.
 */
template <typename T>
class GConstrainedNumT
	:public GParameterT<T>
{
	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int) {
		using boost::serialization::make_nvp;

		// Save data
		ar
		& make_nvp("GParameterT_T", boost::serialization::base_object<GParameterT<T>>(*this))
		& BOOST_SERIALIZATION_NVP(lowerBoundary_)
		& BOOST_SERIALIZATION_NVP(upperBoundary_);
	}
	///////////////////////////////////////////////////////////////////////

	// Make sure this class can only be instantiated with T as an arithmetic type
	BOOST_MPL_ASSERT((boost::is_arithmetic<T>));

public:
	/***************************************************************************/
	/**
	 * The default constructor
	 */
	GConstrainedNumT()
		: GParameterT<T>(GConstrainedValueLimitT<T>::lowest())
		, lowerBoundary_(GConstrainedValueLimitT<T>::lowest())
		, upperBoundary_(GConstrainedValueLimitT<T>::highest())
	{ /* nothing */ }


	/***************************************************************************/
	/**
	 * A constructor that initializes the value only. The boundaries will
	 * be set to the maximum and minimum allowed values of the corresponding type.
	 *
	 * @param val The desired external value of this object
	 */
	explicit GConstrainedNumT(const T& val)
		: GParameterT<T>(val)
		, lowerBoundary_(GConstrainedValueLimitT<T>::lowest())
		, upperBoundary_(GConstrainedValueLimitT<T>::highest())
	{ /* nothing */ }

	/***************************************************************************/
	/**
	 * Initializes the boundaries and sets the value to the lower boundary.
	 *
	 * @param lowerBoundary The lower boundary of the value range
	 * @param upperBoundary The upper boundary of the value range
	 */
	GConstrainedNumT(const T& lowerBoundary, const T& upperBoundary)
		: GParameterT<T>(lowerBoundary)
		, lowerBoundary_(lowerBoundary)
		, upperBoundary_(upperBoundary)
	{
		// Naturally the upper boundary should be > the lower boundary.
		if(lowerBoundary_ >= upperBoundary_) {
			glogger
			<< "In GConstrainedNumT<T>::GConstrainedNumT(lower,upper):" << std::endl
			<< "lowerBoundary_ = " << lowerBoundary_ << " is > upperBoundary_ = " << upperBoundary_ << std::endl
			<< GEXCEPTION;
		}

		// We might have constraints regarding the allowed boundaries. Cross-check
		if(lowerBoundary < GConstrainedValueLimitT<T>::lowest() || upperBoundary > GConstrainedValueLimitT<T>::highest()) {
			glogger
			<< "In GConstrainedNumT<T>::GConstrainedNumT(lower,upper):" << std::endl
			<< "lower and/or upper limit outside of allowed value range:" << std::endl
			<< "lowerBoundary = " << lowerBoundary << std::endl
			<< "upperBoundary = " << upperBoundary << std::endl
			<< "GConstrainedValueLimit<T>::lowest()  = " << GConstrainedValueLimitT<T>::lowest() << std::endl
			<< "GConstrainedValueLimit<T>::highest() = " << GConstrainedValueLimitT<T>::highest() << std::endl
			<< GEXCEPTION;
		}
	}

	/***************************************************************************/
	/**
	 * Initialization with value and boundaries.
	 *
	 * @param val The desired value of this object
	 * @param lowerBoundary The lower boundary of the value range
	 * @param upperBoundary The upper boundary of the value range
	 */
	GConstrainedNumT(const T& val, const T& lowerBoundary, const T& upperBoundary)
		: GParameterT<T>(val)
		, lowerBoundary_(lowerBoundary)
		, upperBoundary_(upperBoundary)
	{
		// Do some error checking
		if(lowerBoundary_ >= upperBoundary_) {
			glogger
			<< "In GConstrainedNumT<T>::GConstrainedNumT(val,lower,upper):" << std::endl
			<< "lowerBoundary_ = " << lowerBoundary_ << "is >= than" << std::endl
			<< "upperBoundary_ = " << upperBoundary_ << std::endl
			<< GEXCEPTION;
		}

		// We might have constraints regarding the allowed boundaries. Cross-check
		if(lowerBoundary < GConstrainedValueLimitT<T>::lowest() || upperBoundary > GConstrainedValueLimitT<T>::highest()) {
			glogger
			<< "In GConstrainedNumT<T>::GConstrainedNumT(val, lower,upper):" << std::endl
			<< "lower and/or upper limit outside of allowed value range:" << std::endl
			<< "lowerBoundary = " << lowerBoundary << std::endl
			<< "upperBoundary = " << upperBoundary << std::endl
			<< "GConstrainedValueLimitT<T>::lowest()  = " << GConstrainedValueLimitT<T>::lowest() << std::endl
			<< "GConstrainedValueLimitT<T>::highest() = " <<  GConstrainedValueLimitT<T>::highest() << std::endl
			<< GEXCEPTION;
		}

		// Check that the value is inside of the allowed value range
		if(val < lowerBoundary_ || val > upperBoundary_) {
			glogger
			<< "In GConstrainedNumT<T>::GConstrainedNumT(val,lower,upper):" << std::endl
			<< "Assigned value " << val << " is outside of its allowed boundaries: " << std::endl
			<< "lowerBoundary_ = " << lowerBoundary_ << std::endl
			<< "upperBoundary_ = " << upperBoundary_ << std::endl
			<< GEXCEPTION;
		}
	}

	/***************************************************************************/
	/**
	 * A standard copy constructor. Most work is done by the parent
	 * classes, we only need to copy the allowed value range.
	 *
	 * @param cp Another GConstrainedNumT<T> object
	 */
	GConstrainedNumT(const GConstrainedNumT<T>& cp)
		: GParameterT<T>(cp)
		, lowerBoundary_(cp.lowerBoundary_)
		, upperBoundary_(cp.upperBoundary_)
	{ /* nothing */ }

	/***************************************************************************/
	/**
	 * The standard destructor
	 */
	virtual ~GConstrainedNumT()
	{ /* nothing */	}

	/***************************************************************************/
	/**
	 * A standard assignment operator for T values. Note that this function
	 * will throw an exception if the new value is not in the allowed value range.
	 *
	 * @param The desired new external value
	 * @return The new external value of this object
	 */
	virtual T operator=(const T& val) override {
		GConstrainedNumT<T>::setValue(val);
		return val;
	}

	/***************************************************************************/
	/**
	 * The standard assignment operator
	 */
	const GConstrainedNumT<T>& operator=(const GConstrainedNumT<T>& cp) {
		this->load_(&cp);
		return *this;
	}

	/***************************************************************************/
	/**
	 * Checks for equality with another GConstrainedNumT<T> object
	 *
	 * @param  cp A constant reference to another GConstrainedNumT<T> object
	 * @return A boolean indicating whether both objects are equal
	 */
	bool operator==(const GConstrainedNumT<T>& cp) const {
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
	 * Checks for inequality with another GConstrainedNumT<T> object
	 *
	 * @param  cp A constant reference to another GConstrainedNumT<T> object
	 * @return A boolean indicating whether both objects are inequal
	 */
	bool operator!=(const GConstrainedNumT<T>& cp) const {
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
	 * Searches for compliance with expectations with respect to another object
	 * of the same type
	 *
	 * @param cp A constant reference to another GObject object
	 * @param e The expected outcome of the comparison
	 * @param limit The maximum deviation for floating point values (important for similarity checks)
	 */
	virtual void compare(
		const GObject& cp
		, const Gem::Common::expectation& e
		, const double& limit
	) const override {
		using namespace Gem::Common;

		// Check that we are dealing with a GConstrainedNumT<T> reference independent of this object and convert the pointer
		const GConstrainedNumT<T> *p_load = Gem::Common::g_convert_and_compare<GObject, GConstrainedNumT<T>>(cp, this);

		GToken token("GConstrainedNumT<T>", e);

		// Compare our parent data ...
		Gem::Common::compare_base<GParameterT<T>>(IDENTITY(*this, *p_load), token);

		// ... and then the local data
		compare_t(IDENTITY(lowerBoundary_, p_load->lowerBoundary_), token);
		compare_t(IDENTITY(upperBoundary_, p_load->upperBoundary_), token);

		// React on deviations from the expectation
		token.evaluate();
	}

	/***************************************************************************/
	/**
	  * Retrieves the lower boundary
	  *
	  * @return The value of the lower boundary
	  */
	T getLowerBoundary() const {
		return lowerBoundary_;
	}

	/* ----------------------------------------------------------------------------------
	 * Tested in GConstrainedNumT<T>::specificTestsNoFailuresExpected_GUnitTests()
	 * ----------------------------------------------------------------------------------
	 */

	/***************************************************************************/
	/**
	  * Retrieves the upper boundary
	  *
	  * @return The value of the upper boundary
	  */
	T getUpperBoundary() const {
		return upperBoundary_;
	}

	/* ----------------------------------------------------------------------------------
	 * Tested in GConstrainedNumT<T>::specificTestsNoFailuresExpected_GUnitTests()
	 * ----------------------------------------------------------------------------------
	 */

	/***************************************************************************/
	/**
	 * Resets the boundaries to the maximum allowed value.
	 */
	void resetBoundaries() {
		this->setBoundaries(GConstrainedValueLimitT<T>::lowest(), GConstrainedValueLimitT<T>::highest());
	}

	/* ----------------------------------------------------------------------------------
	 * Tested in GConstrainedNumT<T>::specificTestsNoFailuresExpected_GUnitTests()
	 * ----------------------------------------------------------------------------------
	 */

	/***************************************************************************/
	/**
	 * Sets the boundaries of this object and does corresponding
	 * error checks. If the current value is below or above the new
	 * boundaries, this function will throw. Set the external value to
	 * a new value between the new boundaries before calling this
	 * function, or use the corresponding "setValue()" overload, which
	 * also allows setting of boundaries.
	 *
	 * @param lowerBoundary The new lower boundary for this object
	 * @param upperBoundary The new upper boundary for this object
	 */
	virtual void setBoundaries(const T& lowerBoundary, const T& upperBoundary) {
		const T currentValue = this->value(); // Store the externally visible value

		// Check that the boundaries make sense
		if(lowerBoundary > upperBoundary) {
			glogger
			<< "In GConstrainedNumT<T>::setBoundaries(const T&, const T&)" << std::endl
			<< "with typeid(T).name() = " << typeid(T).name() << " :" << std::endl
			<< "Lower and/or upper boundary has invalid value : " << lowerBoundary << " " << upperBoundary << std::endl
			<< GEXCEPTION;
		}

		// We might have constraints regarding the allowed boundaries. Cross-check
		if(lowerBoundary < GConstrainedValueLimitT<T>::lowest() || upperBoundary > GConstrainedValueLimitT<T>::highest()) {
			glogger
			<< "In GConstrainedNumT<T>::setBoundaries(const T&, const T&):" << std::endl
			<< "lower and/or upper limit outside of allowed value range:" << std::endl
			<< "lowerBoundary = " << lowerBoundary << std::endl
			<< "upperBoundary = " << upperBoundary << std::endl
			<< "GConstrainedValueLimitT<T>::lowest() = " << GConstrainedValueLimitT<T>::lowest() << std::endl
			<< " GConstrainedValueLimit<T>::highest() = " <<  GConstrainedValueLimitT<T>::highest() << std::endl
			<< GEXCEPTION;
		}

		// Check that the value is inside the allowed range
		if(currentValue < lowerBoundary || currentValue > upperBoundary){
			glogger
			<< "In GConstrainedNumT<T>::setBoundaries(const T&, const T&) :" << std::endl
			<< "with typeid(T).name() = " << typeid(T).name() << std::endl
			<< "Attempt to set new boundaries [" << lowerBoundary << ":" << upperBoundary << "]" << std::endl
			<< "with existing value  " << currentValue << " outside of this range." << std::endl
			<< GEXCEPTION;
		}

		lowerBoundary_ = lowerBoundary;
		upperBoundary_ = upperBoundary;

		// Re-set the internal representation of the value
		GParameterT<T>::setValue(currentValue);
	}

	/* ----------------------------------------------------------------------------------
	 * Throwing tested in GConstrainedNumT<T>::specificTestsFailuresExpected_GUnitTests()
	 * Throwing for invalid boundaries tested in GConstrainedFPT<T>::specificTestsFailuresExpected_GUnitTests()
	 * Throwing for invalid boundaries tested in GConstrainedIntT<T>::specificTestsFailuresExpected_GUnitTests()
	 * Tested in GConstrainedNumT<T>::specificTestsNoFailuresExpected_GUnitTests()
	 * ----------------------------------------------------------------------------------
	 */

	/***************************************************************************/
	/**
	 * Allows to set the value. This function will throw if val is not in the currently
	 * assigned value range. Use the corresponding overload if you want to set the value
	 * together with its boundaries instead.
	 *
	 * @param val The new T value stored in this class
	 */
	virtual void setValue(const T& val) override {
		// Do some error checking
		if(val < lowerBoundary_ || val > upperBoundary_) {
			glogger
			<< "In GConstrainedNumT<T>::setValue(val):" << std::endl
			<< std::setprecision(20)
			<< "Assigned value = " << val << " is outside of its allowed boundaries: " << std::endl
			<< "lowerBoundary_ = " << lowerBoundary_ << std::endl
			<< "upperBoundary_ = " << upperBoundary_ << std::endl
			<< GEXCEPTION;
		}

		// O.k., assign value
		GParameterT<T>::setValue(val);
	}

	/* ----------------------------------------------------------------------------------
	 * Throwing tested in GConstrainedNumT<T>::specificTestsFailuresExpected_GUnitTests()
	 * Tested in GConstrainedNumT<T>::specificTestsNoFailuresExpected_GUnitTests()
	 * ----------------------------------------------------------------------------------
	 */

	/***************************************************************************/
	/**
	 * Allows to set the value of this object together with its boundaries.
	 *
	 * @param val The desired value of this object
	 * @param lowerBoundary The lower boundary of the value range
	 * @param upperBoundary The upper boundary of the value range
	 */
	virtual void setValue(
		const T& val
		, const T& lowerBoundary
		, const T& upperBoundary
	) BASE {
		// Do some error checking

		// Do the boundaries make sense ?
		if(lowerBoundary > upperBoundary) {
			glogger
			<< "In GConstrainedNumT<T>::setValue(val,lower,upper):" << std::endl
			<< "lowerBoundary_ = " << lowerBoundary_ << "is larger than" << std::endl
			<< "upperBoundary_ = " << upperBoundary_ << std::endl
			<< GEXCEPTION;
		}

		// We might have constraints regarding the allowed boundaries. Cross-check
		if(lowerBoundary < GConstrainedValueLimitT<T>::lowest() || upperBoundary > GConstrainedValueLimitT<T>::highest()) {
			glogger
			<< "In GConstrainedNumT<T>::setValue(val,lower,upper):" << std::endl
			<< "lower and/or upper limit outside of allowed value range:" << std::endl
			<< "lowerBoundary = " << lowerBoundary << std::endl
			<< "upperBoundary = " << upperBoundary << std::endl
			<< "GConstrainedValueLimitT<T>::lowest() = " << GConstrainedValueLimitT<T>::lowest() << std::endl
			<< " GConstrainedValueLimit<T>::highest() = " <<  GConstrainedValueLimitT<T>::highest() << std::endl
			<< GEXCEPTION;
		}

		// Check that the value is inside of the allowed value range
		if(val < lowerBoundary || val > upperBoundary) {
			glogger
			<< "In GConstrainedNumT<T>::setValue(val,lower,upper):" << std::endl
			<< "Assigned value = " << val << " is outside of its allowed boundaries: " << std::endl
			<< "lowerBoundary  = " << lowerBoundary << std::endl
			<< "upperBoundary  = " << upperBoundary << std::endl
			<< GEXCEPTION;
		}

		// O.k., assign the boundaries
		lowerBoundary_ = lowerBoundary;
		upperBoundary_ = upperBoundary;

		// Set the internal representation of the value -- we might be in a different
		// region of the transformation internally, and the mapping will likely depend on
		// the boundaries.
		GParameterT<T>::setValue(val);
	}

	/* ----------------------------------------------------------------------------------
	 * Throwing tested in GConstrainedNumT<T>::specificTestsFailuresExpected_GUnitTests()
	 * Throwing for invalid boundaries tested in GConstrainedFPT<T>::specificTestsFailuresExpected_GUnitTests()
	 * Throwing for invalid boundaries tested in GConstrainedIntT<T>::specificTestsFailuresExpected_GUnitTests()
	 * Tested in GConstrainedNumT<T>::specificTestsNoFailuresExpected_GUnitTests()
	 * ----------------------------------------------------------------------------------
	 */

	/***************************************************************************/
	/**
	 * Retrieval of the value. This is an overloaded version of the original
	 * GParameterT<T>::value() function which applies a transformation, to be
	 * defined in derived classes.
	 *
	 * @return The transformed value of val_
	 */
	virtual T value() const  override {
		T mapping = this->transfer(GParameterT<T>::value());

		// Reset internal value -- possible because it is declared mutable in
		// GParameterT<T>. Resetting the internal value prevents divergence through
		// extensive mutation and also speeds up the previous part of the transfer
		// function
		GParameterT<T>::setValue_(mapping);

		return mapping;
	}

	/* ----------------------------------------------------------------------------------
	 * Tested in GConstrainedNumT<T>::specificTestsNoFailuresExpected_GUnitTests()
	 * Effects of variations of the internal value on the external value tested in most-derived classes
	 * ----------------------------------------------------------------------------------
	 */

	/***************************************************************************/
	/**
	 * Retrieves GParameterT<T>'s internal value. Added here for compatibility
	 * reasons.
	 */
	T getInternalValue() const {
		return GParameterT<T>::value();
	}

	/* ----------------------------------------------------------------------------------
	 * Tested in GConstrainedNumT<T>::specificTestsNoFailuresExpected_GUnitTests()
	 * ----------------------------------------------------------------------------------
	 */

	/***************************************************************************/
	/**
	 * The transfer function needed to calculate the externally visible
	 * value. Declared public so we can do tests of the value transformation.
	 */
	virtual T transfer(const T&) const = 0;


	/***************************************************************************/
	/**
	 * Converts the local data to a boost::property_tree node
	 *
	 * @param ptr The boost::property_tree object the data should be saved to
	 * @param baseName The name assigned to the object
	 */
	virtual void toPropertyTree(
		pt::ptree& ptr
		, const std::string& baseName
	) const override {
		ptr.put(baseName + ".name", this->getParameterName());
		ptr.put(baseName + ".type", this->name());
		ptr.put(baseName + ".baseType", Gem::Common::GTypeToStringT<T>::value());
		ptr.put(baseName + ".isLeaf", this->isLeaf());
		ptr.put(baseName + ".nVals", 1);
		ptr.put(baseName + ".values.value0", this->value());
		ptr.put(baseName + ".lowerBoundary", this->getLowerBoundary());
		ptr.put(baseName + ".upperBoundary", this->getUpperBoundary());
		ptr.put(baseName + ".initRandom", false); // Unused for the creation of a property tree
		ptr.put(baseName + ".adaptionsActive", this->adaptionsActive());
	}

	/***************************************************************************/
	/**
	 * Emits a name for this class / object
	 */
	virtual std::string name() const override {
		return std::string("GConstrainedNumT");
	}

protected:
	/***************************************************************************/
	/**
	 * Loads the data of another GConstrainedNumT<T>, camouflaged as a GObject.
	 *
	 * @param cp Another GConstrainedNumT<T> object, camouflaged as a GObject
	 */
	virtual void load_(const GObject *cp) override {
		// Check that we are dealing with a GConstrainedNumT<T> reference independent of this object and convert the pointer
		const GConstrainedNumT<T> *p_load = Gem::Common::g_convert_and_compare<GObject, GConstrainedNumT<T>>(cp, this);

		// Load our parent class'es data ...
		GParameterT<T>::load_(cp);

		// ... and then our own
		lowerBoundary_ = p_load->lowerBoundary_;
		upperBoundary_ = p_load->upperBoundary_;
	}

	/***************************************************************************/
	/**
	 * Returns a "comparative range". This is e.g. used to make Gauss-adaption
	 * independent of a parameters value range
	 */
	virtual T range() const override {
		return upperBoundary_ - lowerBoundary_;
	}

	/***************************************************************************/

private:
	/***************************************************************************/

	T lowerBoundary_; ///< The lower allowed boundary for our value
	T upperBoundary_; ///< The upper allowed boundary for our value

public:
	/***************************************************************************/
	/**
	 * Applies modifications to this object. This is needed for testing purposes
	 *
	 * @return A boolean which indicates whether modifications were made
	 */
	virtual bool modify_GUnitTests() override {
#ifdef GEM_TESTING
      bool result = false;

		// Call the parent classes' functions
		if(GParameterT<T>::modify_GUnitTests()) result = true;

		return result;

#else /* GEM_TESTING */  // If this function is called when GEM_TESTING isn't set, throw
		condnotset("GConstrainedNumT<>::modify_GUnitTests", "GEM_TESTING");
		return false;
#endif /* GEM_TESTING */
	}

	/***************************************************************************/
	/**
	 * Performs self tests that are expected to succeed. This is needed for testing purposes
	 */
	virtual void specificTestsNoFailureExpected_GUnitTests() override {
#ifdef GEM_TESTING
		// Some general settings
		const T testVal = T(42);
		const T lowerBoundary = T(0);
		const T upperBoundary = T(100);

		// Call the parent classes' functions
		GParameterT<T>::specificTestsNoFailureExpected_GUnitTests();

		//------------------------------------------------------------------------------

		{ // Make sure resetting the boundaries results in correct limits
			// Clone the current object, so we can always recover from failures
			std::shared_ptr<GConstrainedNumT<T>> p_test = this->GObject::template clone<GConstrainedNumT<T>>();

			// Make sure we can freely assign values
			BOOST_CHECK_NO_THROW(p_test->resetBoundaries());
			BOOST_CHECK(p_test->getLowerBoundary() == GConstrainedValueLimitT<T>::lowest());

			// GConstrainedDoubleObject assigns the float prior to the specified boundary
			if(typeid(T) == typeid(double)) {
				BOOST_CHECK(double(p_test->getUpperBoundary()) ==  boost::math::float_prior<double>(double(GConstrainedValueLimitT<T>::highest())));
			} else {
				BOOST_CHECK(p_test->getUpperBoundary() ==  GConstrainedValueLimitT<T>::highest());
			}
		}

		//------------------------------------------------------------------------------

		{ // Check that assigning a simple, valid value works
			std::shared_ptr<GConstrainedNumT<T>> p_test = this->GObject::template clone<GConstrainedNumT<T>>();

			// Make sure we can freely assign values
			BOOST_CHECK_NO_THROW(p_test->resetBoundaries());

			// Assign a valid value
			BOOST_CHECK_NO_THROW(p_test->setValue(testVal));

			if(typeid(T) == typeid(bool)) {
			   BOOST_CHECK_NO_THROW(p_test->setValue(true, false, true));
			} else {
			   BOOST_CHECK_NO_THROW(p_test->setValue(testVal, T(30), T(50)));
			}

			// Check with the local value() function that the value has been set
			BOOST_CHECK(p_test->value() == testVal);

			// Check that getInternalValue() behaves as expected
			BOOST_CHECK(p_test->value() == p_test->getInternalValue());
		}

		//------------------------------------------------------------------------------

		{ // Test that setting of boundaries with setBoundaries(lower, upper) results in the correct values
			std::shared_ptr<GConstrainedNumT<T>> p_test = this->GObject::template clone<GConstrainedNumT<T>>();

			// Make sure we can freely assign values
			BOOST_CHECK_NO_THROW(p_test->resetBoundaries());

			// Set the desired value
			BOOST_CHECK_NO_THROW(p_test->setValue(testVal));

			// Check that the value has indeed been set
			BOOST_CHECK(p_test->value() == testVal);

			// Set the boundaries
			BOOST_CHECK_NO_THROW(p_test->setBoundaries(lowerBoundary, upperBoundary));

			// Check the values of these boundaries
			BOOST_CHECK(p_test->getLowerBoundary() == lowerBoundary);

			// GConstrainedDoubleObject assigns the float prior to the specified boundary
			if(typeid(T) == typeid(double)) {
				BOOST_CHECK(double(p_test->getUpperBoundary()) == boost::math::float_prior<double>(double(upperBoundary)));
			} else {
				BOOST_CHECK(p_test->getUpperBoundary() == upperBoundary);
			}

			// Check that the value is still the same
			BOOST_CHECK(p_test->value() == testVal);
		}

		//------------------------------------------------------------------------------

		{ // Test that setting of boundaries with setValue(val, lower, upper) results in the correct values
			std::shared_ptr<GConstrainedNumT<T>> p_test = this->GObject::template clone<GConstrainedNumT<T>>();

			// Make sure we can freely assign values
			BOOST_CHECK_NO_THROW(p_test->resetBoundaries());

			// Set the desired value
			BOOST_CHECK_NO_THROW(p_test->setValue(testVal, lowerBoundary, upperBoundary));

			// Check the values of these boundaries
			BOOST_CHECK_MESSAGE(
					p_test->getLowerBoundary() == lowerBoundary
					,  "\n"
					<< "p_test->getLowerBoundary() = " << p_test->getLowerBoundary() << "\n"
					<< "lowerBoundary = " << lowerBoundary << "\n"
			);

         if(typeid(T) == typeid(double)) {
            BOOST_CHECK_MESSAGE(
                  double(p_test->getUpperBoundary()) == boost::math::float_prior<double>(double(upperBoundary))
                  ,  "\n"
                  << "p_test->getUpperBoundary() = " << p_test->getUpperBoundary() << "\n"
                  << "upperBoundary = " << upperBoundary << "\n"
            );
         } else if(typeid(T) == typeid(float)) {
            BOOST_CHECK_MESSAGE(
                  float(p_test->getUpperBoundary()) == boost::math::float_prior<float>(float(upperBoundary))
                  ,  "\n"
                  << "p_test->getUpperBoundary() = " << p_test->getUpperBoundary() << "\n"
                  << "upperBoundary = " << upperBoundary << "\n"
            );
         } else {
            BOOST_CHECK_MESSAGE(
                  p_test->getUpperBoundary() == upperBoundary
                  ,  "\n"
                  << "p_test->getUpperBoundary() = " << p_test->getUpperBoundary() << "\n"
                  << "upperBoundary = " << upperBoundary << "\n"
            );
         }

			// Check that the value is still the same
			BOOST_CHECK(p_test->value() == testVal);
		}

		//------------------------------------------------------------------------------

		{ // Check that assigning a valid value using operator= results in the correct value
			std::shared_ptr<GConstrainedNumT<T>> p_test = this->GObject::template clone<GConstrainedNumT<T>>();

			// Make sure we can freely assign values
			BOOST_CHECK_NO_THROW(p_test->resetBoundaries());

			// Set the desired value
			BOOST_CHECK_NO_THROW(p_test->setValue(testVal, lowerBoundary, upperBoundary));

			// Assign a value
			BOOST_CHECK_NO_THROW(*p_test = (testVal - T(1)));

			// Check that is was set correctly
			BOOST_CHECK(p_test->value() == (testVal - T(1)));
		}

		//------------------------------------------------------------------------------

#else /* GEM_TESTING */ // If this function is called when GEM_TESTING isn't set, throw
		condnotset("GConstrainedNumT<>::specificTestsNoFailureExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
	}

	/***************************************************************************/
	/**
	 * Performs self tests that are expected to fail. This is needed for testing purposes
	 */
	virtual void specificTestsFailuresExpected_GUnitTests() override {
#ifdef GEM_TESTING
		// Call the parent classes' functions
		GParameterT<T>::specificTestsFailuresExpected_GUnitTests();

		//------------------------------------------------------------------------------

		{ // Check that setting invalid boundaries in setBoundaries(lower, upper) throws
			std::shared_ptr<GConstrainedNumT<T>> p_test = this->GObject::template clone<GConstrainedNumT<T>>();

			// Setting an upper boundary < lower boundary should throw
			BOOST_CHECK_THROW(p_test->setBoundaries(T(1), T(0)), Gem::Common::gemfony_error_condition);
		}

		//------------------------------------------------------------------------------

		{ // Check that setting boundaries incompatible with the current value throws
			std::shared_ptr<GConstrainedNumT<T>> p_test = this->GObject::template clone<GConstrainedNumT<T>>();

			// First make sure we have the widest possible boundaries
			BOOST_CHECK_NO_THROW(p_test->resetBoundaries());

			// Now assign a value
			BOOST_CHECK_NO_THROW(p_test->setValue(T(2)));

			// Setting of boundaries incompatible with T(2) should throw
			BOOST_CHECK_THROW(p_test->setBoundaries(T(0), T(1)), Gem::Common::gemfony_error_condition);
		}

		//------------------------------------------------------------------------------

		{ // Check that setting invalid boundaries with setValue(val, lower, upper) throws
			std::shared_ptr<GConstrainedNumT<T>> p_test = this->GObject::template clone<GConstrainedNumT<T>>();

			// Setting an upper boundary < lower boundary should throw
			BOOST_CHECK_THROW(p_test->setValue(T(0), T(2), T(0)), Gem::Common::gemfony_error_condition);
		}

		//------------------------------------------------------------------------------

		{ // Check that setting a value outside of valid boundaries with setValue(val, lower, upper) throws
			std::shared_ptr<GConstrainedNumT<T>> p_test = this->GObject::template clone<GConstrainedNumT<T>>();

			// Try to assign a value outside of the allowed boundaries should throw
			BOOST_CHECK_THROW(p_test->setValue(T(2), T(0), T(1)), Gem::Common::gemfony_error_condition);
		}

		//------------------------------------------------------------------------------

		{ // Check that setting a value outside of the currently assigned boundaries throws
			std::shared_ptr<GConstrainedNumT<T>> p_test = this->GObject::template clone<GConstrainedNumT<T>>();

			// Assign a compatible value and boundaries
			BOOST_CHECK_NO_THROW(p_test->setValue(T(0), T(0), T(1)));

			// Try to assign 2 as a value - should throw
			BOOST_CHECK_THROW(p_test->setValue(T(2)), Gem::Common::gemfony_error_condition);
		}

		//------------------------------------------------------------------------------

		{ // Check that assigning a value using operator= outside of the allowed range throws
			std::shared_ptr<GConstrainedNumT<T>> p_test = this->GObject::template clone<GConstrainedNumT<T>>();

			// Assign a compatible value and boundaries
			BOOST_CHECK_NO_THROW(p_test->setValue(T(0), T(0), T(1)));

			// Try to assign 2 as a value - should throw
			if(typeid(T) != typeid(bool)) {
			   BOOST_CHECK_THROW(*p_test = T(2), Gem::Common::gemfony_error_condition);
			}
		}

		//------------------------------------------------------------------------------

#else /* GEM_TESTING */ // If this function is called when GEM_TESTING isn't set, throw
		condnotset("GConstrainedNumT<>::specificTestsFailuresExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
	}

	/***************************************************************************/

};

/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */

/******************************************************************************/
// The content of BOOST_SERIALIZATION_ASSUME_ABSTRACT(T)

namespace boost {
namespace serialization {
template<typename T>
struct is_abstract<Gem::Geneva::GConstrainedNumT<T>> : public boost::true_type {};
template<typename T>
struct is_abstract< const Gem::Geneva::GConstrainedNumT<T>> : public boost::true_type {};
}
}

/******************************************************************************/

#endif /* GCONSTRAINEDNUMT_HPP_ */

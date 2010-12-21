/**
 * @file GConstrainedIntT.hpp
 */

/*
 * Copyright (C) Authors of the Geneva library collection and Karlsruhe
 * Institute of Technology (University of the State of Baden-Wuerttemberg
 * and National Laboratory of the Helmholtz Association).
 *
 * See the AUTHORS file in the top-level directory for a list of authors.
 *
 * Contact: info [at] gemfony (dot) com
 *
 * This file is part of the Geneva library collection
 *
 * Geneva is free software: you can redistribute it and/or modify
 * it under the terms of version 3 of the GNU Affero General Public License
 * as published by the Free Software Foundation.
 *
 * Geneva is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with the Geneva library.  If not, see <http://www.gnu.org/licenses/>.
 *
 * For further information on Gemfony scientific and Geneva, visit
 * http://www.gemfony.com .
 */


// Standard headers go here

// Boost headers go here

#ifndef GCONSTRAINEDINTEGERT_HPP_
#define GCONSTRAINEDINTEGERT_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif

// Geneva headers go here
#include "geneva/GObject.hpp"
#include "geneva/GConstrainedNumT.hpp"
#include "common/GExceptions.hpp"
#include "hap/GRandomT.hpp"

namespace Gem
{
namespace Geneva
{

/******************************************************************************/
/* The GConstrainedIntT class represents an integer type, such as an int or a long,
 * equipped with the ability to adapt itself. The value range can have an upper and a lower
 * limit, both are included in the allowed value range.  Adapted values will only appear
 * in the given range to the user. Note that appropriate adaptors (see e.g the
 * GInt32FlipAdaptor class) need to be loaded in order to benefit from the adaption
 * capabilities. We currently only allow signed integers, as a mapping takes place from
 * internal to external value, and both are required to be of the same type at the moment.
 * Signed integers as types are enforced using Boost's concept checks.
 */
template <typename int_type>
class GConstrainedIntT
	:public GConstrainedNumT<int_type>
{
	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int) {
		using boost::serialization::make_nvp;

		// Save data
		ar & make_nvp("GConstrainedNumT_T", boost::serialization::base_object<GConstrainedNumT<int_type> >(*this));
	}
	///////////////////////////////////////////////////////////////////////

	// Make sure this class can only be instantiated if int_type is a *signed* integer type
	BOOST_MPL_ASSERT((boost::is_signed<int_type>));

public:
	/****************************************************************************/
	/**
	 * The default constructor
	 */
	GConstrainedIntT()
		: GConstrainedNumT<int_type>()
    { /* nothing */ }

	/****************************************************************************/
	/**
	 * A constructor that initializes the value only. The boundaries will
	 * be set to the maximum and minimum values of the corresponding type.
	 *
	 * @param val The desired external value of this object
	 */
	explicit GConstrainedIntT(const int_type& val)
		: GConstrainedNumT<int_type>(val)
	{ /* nothing */	}

	/****************************************************************************/
	/**
	 * Initializes the boundaries and sets the value to the lower boundary.
	 *
	 * @param lowerBoundary The lower boundary of the value range
	 * @param upperBoundary The upper boundary of the value range
	 */
	GConstrainedIntT(const int_type& lowerBoundary, const int_type& upperBoundary)
		: GConstrainedNumT<int_type>(lowerBoundary, upperBoundary)
	{ /* nothing */	}

	/****************************************************************************/
	/**
	 * Initialization with value and boundaries.
	 *
	 * @param val The desired value of this object
	 * @param lowerBoundary The lower boundary of the value range
	 * @param upperBoundary The upper boundary of the value range
	 */
	GConstrainedIntT(const int_type& val, const int_type& lowerBoundary, const int_type& upperBoundary)
		: GConstrainedNumT<int_type>(val, lowerBoundary, upperBoundary)
	{ /* nothing */	}

	/****************************************************************************/
	/**
	 * A standard copy constructor. Most work is done by the parent
	 * classes, we only need to copy the allowed value range.
	 *
	 * @param cp Another GConstrainedNumT<int_type> object
	 */
	GConstrainedIntT(const GConstrainedIntT<int_type>& cp)
		: GConstrainedNumT<int_type>(cp)
	{ /* nothing */ }

	/****************************************************************************/
	/**
	 * The standard destructor
	 */
	virtual ~GConstrainedIntT()
	{ /* nothing */	}

	/****************************************************************************/
	/**
	 * A standard assignment operator for GConstrainedIntT<int_type> objects
	 *
	 * @param cp A constant reference to another GConstrainedIntT<int_type> object
	 * @return A constant reference to this object
	 */
	const GConstrainedIntT<int_type>& operator=(const GConstrainedIntT<int_type>& cp) {
		GConstrainedIntT<int_type>::load_(&cp);
		return *this;
	}

	/****************************************************************************/
	/**
	 * A standard assignment operator for int_type values. Note that this function
	 * will throw an exception if the new value is not in the allowed value range.
	 *
	 * @param The desired new external value
	 * @return The new external value of this object
	 */
	virtual int_type operator=(const int_type& val) {
		return GConstrainedNumT<int_type>::operator=(val);
	}

	/* ----------------------------------------------------------------------------------
	 * Tested with and without boundaries in GConstrainedIntT<int_type>::specificTestsNoFailuresExpected_GUnitTests()
	 * ----------------------------------------------------------------------------------
	 */

	/****************************************************************************/
    /**
     * Checks equality of this object with another.
     *
     * @param cp A constant reference to another GConstrainedIntT<int_type> object
     * @return A boolean indicating whether both objects are equal
     */
	bool operator==(const GConstrainedIntT<int_type>& cp) const {
		using namespace Gem::Common;
		// Means: The expectation of equality was fulfilled, if no error text was emitted (which converts to "true")
		return !checkRelationshipWith(cp, CE_EQUALITY, 0.,"GConstrainedIntT<int_type>::operator==","cp", CE_SILENT);
	}

	/****************************************************************************/
    /**
     * Checks inequality of this object with another.
     *
     * @param cp A constant reference to another GConstrainedIntT<int_type> object
     * @return A boolean indicating whether both objects are inequal
     */
	bool operator!=(const GConstrainedIntT<int_type>& cp) const {
		using namespace Gem::Common;
		// Means: The expectation of inequality was fulfilled, as no error text was emitted (which converts to "true")
		return !checkRelationshipWith(cp, CE_INEQUALITY, 0.,"GConstrainedIntT<int_type>::operator!=","cp", CE_SILENT);
	}

	/****************************************************************************/
	/**
	 * Checks whether a given expectation for the relationship between this object and another object
	 * is fulfilled.
	 *
	 * @param cp A constant reference to another object, camouflaged as a GObject
	 * @param e The expected outcome of the comparison
	 * @param limit The maximum deviation for floating point values (important for similarity checks)
	 * @param caller An identifier for the calling entity
	 * @param y_name An identifier for the object that should be compared to this one
	 * @param withMessages Whether or not information should be emitted in case of deviations from the expected outcome
	 * @return A boost::optional<std::string> object that holds a descriptive string if expectations were not met
	 */
	boost::optional<std::string> checkRelationshipWith(const GObject& cp,
			const Gem::Common::expectation& e,
			const double& limit,
			const std::string& caller,
			const std::string& y_name,
			const bool& withMessages) const
	{
	    using namespace Gem::Common;

		// Check that we are indeed dealing with a GParamterBase reference
		const GConstrainedIntT<int_type>  *p_load = GObject::conversion_cast<GConstrainedIntT<int_type> >(&cp);

		// Will hold possible deviations from the expectation, including explanations
	    std::vector<boost::optional<std::string> > deviations;

		// Check our parent class'es data ...
		deviations.push_back(GConstrainedNumT<int_type>::checkRelationshipWith(cp, e, limit, "GConstrainedIntT<int_type>", y_name, withMessages));

		// ... no local data

		return evaluateDiscrepancies("GConstrainedIntT<int_type>", caller, deviations, e);
	}

	/****************************************************************************/
	/**
	 * The transfer function needed to calculate the externally visible value.
	 *
	 * @param val The value to which the transformation should be applied
	 * @return The transformed value
	 */
	virtual int_type transfer(const int_type& val) const {
		// Find out the size of the confined area

		if(val >= GConstrainedNumT<int_type>::getLowerBoundary() && val <= GConstrainedNumT<int_type>::getUpperBoundary()) {
			return val;
		}
		else {
			// The result
			std::size_t mapping = 0;

			// Find out the size of the value range. Note that both boundaries
			// are included, so that we need to add 1 to the difference.
			std::size_t value_range = GConstrainedNumT<int_type>::getUpperBoundary() - GConstrainedNumT<int_type>::getLowerBoundary() + 1;

			if(val < GConstrainedNumT<int_type>::getLowerBoundary()) {
				// Find out how many full value ranges val is below the lower boundary.
				// We use integer division here, so 13/4 would be 3.
				std::size_t nBelowLowerBoundary = (GConstrainedNumT<int_type>::getLowerBoundary() - (val + 1)) / value_range;

				// We are dealing with descending (nBelowLowerBoundary is even) and
				// ascending ranges (nBelowLowerBoundary is odd), which need to be treated differently
				if(nBelowLowerBoundary%2 == 0) { // nBelowLowerBoundary is even
					// Transfer the value into the allowed region
					mapping = val + (value_range * (nBelowLowerBoundary + 1));
				}
				else { // nBelowLowerBoundary is odd
					// Transfer the value into the allowed region
					mapping = val + (value_range * (nBelowLowerBoundary + 1));

					// Revert the value to a descending sequence
					mapping = revert(mapping);
				}
			}
			else { // val > getUpperBoundary()
				// Find out how many full value ranges val is above the upper boundary.
				// We use integer division here, so 13/4 would be 3.
				std::size_t nAboveUpperBoundary = (val - GConstrainedNumT<int_type>::getUpperBoundary() - 1) / value_range;

				// We are dealing with descending (nAboveUpperBoundary is even) and
				// ascending ranges (nAboveUpperBoundary is odd), which need to be treated differently
				if(nAboveUpperBoundary%2 == 0) { // nAboveUpperBoundary is even
					// Transfer into the allowed region
					mapping = val - (value_range * (nAboveUpperBoundary + 1));

					// Revert, as we are dealing with a descending value range
					mapping = revert(mapping);
				}
				else { // nAboveUpperBoundary is odd
					// Transfer into the allowed region
					mapping = val - (value_range * (nAboveUpperBoundary + 1));
				}
			}

			return mapping;
		}
	}

	/* ----------------------------------------------------------------------------------
	 * Tested in GConstrainedIntT<int_type>::specificTestsNoFailuresExpected_GUnitTests()
	 * ----------------------------------------------------------------------------------
	 */

protected:
	/****************************************************************************/
	/**
	 * Loads the data of another GConstrainedIntT<int_type>, camouflaged as a GObject.
	 *
	 * @param cp Another GConstrainedIntT<int_type> object, camouflaged as a GObject
	 */
	virtual void load_(const GObject *cp) {
		// Convert GObject pointer to local format
		const GConstrainedIntT<int_type> *p_load = GObject::conversion_cast<GConstrainedIntT<int_type> >(cp);

		// Load our parent class'es data ...
		GConstrainedNumT<int_type>::load_(cp);

		// no local data
	}

	/****************************************************************************/
	/** @brief Create a deep copy of this object */
	virtual GObject *clone_() const = 0;

	/****************************************************************************/
	/**
	 * Randomly initializes the parameter (within its limits)
	 */
	virtual void randomInit_() {
		setValue(this->GParameterBase::gr->uniform_int(GConstrainedNumT<int_type>::getLowerBoundary(), GConstrainedNumT<int_type>::getUpperBoundary()));
	}

	/* ----------------------------------------------------------------------------------
	 * Tested in GConstrainedIntT<int_type>::specificTestsNoFailuresExpected_GUnitTests()
	 * ----------------------------------------------------------------------------------
	 */

private:
	/****************************************************************************/
	/**
	 * Reverts the value to descending order. Note: No check is made whether the value
	 * is indeed in the allowed region.
	 *
	 * @param value The value to be reverted
	 * @return The reverted value
	 */
	int_type revert(const int_type& value) const {
		int_type position = value - GConstrainedNumT<int_type>::getLowerBoundary();
		int_type reverted = GConstrainedNumT<int_type>::getUpperBoundary() - position;
		return reverted;
	}

#ifdef GENEVATESTING
public:
	/****************************************************************************/
	/**
	 * Applies modifications to this object. This is needed for testing purposes
	 *
	 * @return A boolean which indicates whether modifications were made
	 */
	virtual bool modify_GUnitTests() {
		bool result = false;

		// Call the parent classes' functions
		if(GConstrainedNumT<int_type>::modify_GUnitTests()) result = true;

		return result;
	}

	/****************************************************************************/
	/**
	 * Performs self tests that are expected to succeed. This is needed for testing purposes
	 */
	virtual void specificTestsNoFailureExpected_GUnitTests() {
		// Some general settings
		const int_type minLower = -50; // NOTE: This will fail if int_type is unsigned; GConstrainedIntT has been designed for signed types only
		const int_type maxLower =  50;
		const int_type minUpper =  25; // Allow some overlap
		const int_type maxUpper = 125;
		const int_type nTests = 10000;

		// Call the parent classes' functions
		GConstrainedNumT<int_type>::specificTestsNoFailureExpected_GUnitTests();

		//------------------------------------------------------------------------------

		{ // Check that the assignment of different valid values in the allowed range works without boundaries
			boost::shared_ptr<GConstrainedIntT<int_type> > p_test = this->GObject::clone<GConstrainedIntT<int_type> >();

			// Reset the boundaries
			BOOST_CHECK_NO_THROW(p_test->resetBoundaries());

			// Try to assign values
			for(int_type i=-nTests; i<nTests; i++) {
				BOOST_CHECK_NO_THROW(*p_test = i);
				BOOST_CHECK(p_test->value() == i);
			}
		}

		//------------------------------------------------------------------------------

		{ // Check that the assignment of different valid values in the allowed range works with boundaries
			boost::shared_ptr<GConstrainedIntT<int_type> > p_test = this->GObject::clone<GConstrainedIntT<int_type> >();

			for(int_type i=-nTests; i<nTests; i++) {
				// Make sure we start with the maximum range
				BOOST_CHECK_NO_THROW(p_test->resetBoundaries());

				int_type lowerBoundary = this->GParameterBase::gr->uniform_int(minLower, maxLower);
				int_type upperBoundary;
				while((upperBoundary = this->GParameterBase::gr->uniform_int(minUpper, maxUpper)) <= lowerBoundary);

				BOOST_CHECK_NO_THROW(p_test->setValue(lowerBoundary, lowerBoundary, upperBoundary));

				// Check that there are no values outside of the allowed range
				int_type probe = this->GParameterBase::gr->uniform_int(lowerBoundary, upperBoundary);
				BOOST_CHECK_NO_THROW(*p_test = probe);
				BOOST_CHECK(p_test->value() == probe);
			}
		}

		//------------------------------------------------------------------------------

		{ // Check that the transfer function only returns items in the allowed value range
			boost::shared_ptr<GConstrainedIntT<int_type> > p_test = this->GObject::clone<GConstrainedIntT<int_type> >();

			for(int_type i=0; i<nTests; i++) {
				// Make sure we start with the maximum range
				BOOST_CHECK_NO_THROW(p_test->resetBoundaries());

				int_type lowerBoundary = this->GParameterBase::gr->uniform_int(minLower, maxLower);
				int_type upperBoundary;
				while((upperBoundary = this->GParameterBase::gr->uniform_int(minUpper, maxUpper)) <= lowerBoundary);

				BOOST_CHECK_NO_THROW(p_test->setValue(lowerBoundary, lowerBoundary, upperBoundary));

				// Check that there are no values outside of the allowed range
				for(std::size_t j=0; j<100; j++) {
					int_type probe = this->GParameterBase::gr->uniform_int(-10000, 10000);
					int_type mapping = int_type(0);
					BOOST_CHECK_NO_THROW(mapping = p_test->transfer(probe));
					BOOST_CHECK(mapping >= lowerBoundary && mapping <= upperBoundary);
				}
			}
		}

		//------------------------------------------------------------------------------

		{ // Test random initialization using our internal randomInit_ function, without boundaries
			boost::shared_ptr<GConstrainedIntT<int_type> > p_test = this->GObject::clone<GConstrainedIntT<int_type> >();

			// Reset the boundaries
			BOOST_CHECK_NO_THROW(p_test->resetBoundaries());

			// Randomly initialize using our internal function -- will use the most extreme boundaries available
			BOOST_CHECK_NO_THROW(p_test->randomInit_());
		}

		//------------------------------------------------------------------------------

		{ // Test random initialization using our internal randomInit_ function, with boundaries
			boost::shared_ptr<GConstrainedIntT<int_type> > p_test = this->GObject::clone<GConstrainedIntT<int_type> >();

			for(int_type i=-nTests; i<nTests; i++) {
				// Make sure we start with the maximum range
				BOOST_CHECK_NO_THROW(p_test->resetBoundaries());

				int_type lowerBoundary = this->GParameterBase::gr->uniform_int(minLower, maxLower);
				int_type upperBoundary;
				while((upperBoundary = this->GParameterBase::gr->uniform_int(minUpper, maxUpper)) <= lowerBoundary);

				BOOST_CHECK_NO_THROW(p_test->setValue(lowerBoundary, lowerBoundary, upperBoundary));

				// Randomly initialize, using our internal value
				BOOST_CHECK_NO_THROW(p_test->randomInit_());
			}
		}

		//------------------------------------------------------------------------------

		{ // Check that setting an upper boundary larger than the allowed value (see GConstrainedValueLimit<T>) with the setValue(val, lower, upper) function throws
			boost::shared_ptr<GConstrainedIntT<int_type> > p_test = this->GObject::clone<GConstrainedIntT<int_type> >();

			// Reset the boundaries so we are free to do what we want
			BOOST_CHECK_NO_THROW(p_test->resetBoundaries());

			// Check that the boundaries have the expected values
			BOOST_CHECK(p_test->getLowerBoundary() == -GConstrainedValueLimit<int_type>::max());
			BOOST_CHECK(p_test->getUpperBoundary() ==  GConstrainedValueLimit<int_type>::max());

			// Try to set a boundary to a bad value
			BOOST_CHECK_THROW(p_test->setValue(0, 0, std::numeric_limits<int_type>::max()), Gem::Common::gemfony_error_condition);
		}

		//------------------------------------------------------------------------------

		{ // Check that setting a lower boundary smaller than the allowed value (see GConstrainedValueLimit<T>)  with the setValue(val, lower, upper) function throws
			boost::shared_ptr<GConstrainedIntT<int_type> > p_test = this->GObject::clone<GConstrainedIntT<int_type> >();

			// Reset the boundaries so we are free to do what we want
			BOOST_CHECK_NO_THROW(p_test->resetBoundaries());

			// Check that the boundaries have the expected values
			BOOST_CHECK(p_test->getLowerBoundary() == -GConstrainedValueLimit<int_type>::max());
			BOOST_CHECK(p_test->getUpperBoundary() ==  GConstrainedValueLimit<int_type>::max());

			// Try to set a boundary to a bad value
			BOOST_CHECK_THROW(p_test->setValue(0, -std::numeric_limits<int_type>::max(), 100), Gem::Common::gemfony_error_condition);
		}

		//------------------------------------------------------------------------------

		{ // Check that setting an upper boundary larger than the allowed value (see GConstrainedValueLimit<T>) with the setBoundaries(lower, upper) function throws
			boost::shared_ptr<GConstrainedIntT<int_type> > p_test = this->GObject::clone<GConstrainedIntT<int_type> >();

			// Reset the boundaries so we are free to do what we want
			BOOST_CHECK_NO_THROW(p_test->resetBoundaries());

			// Check that the boundaries have the expected values
			BOOST_CHECK(p_test->getLowerBoundary() == -GConstrainedValueLimit<int_type>::max());
			BOOST_CHECK(p_test->getUpperBoundary() ==  GConstrainedValueLimit<int_type>::max());

			// Try to set a boundary to a bad value
			BOOST_CHECK_THROW(p_test->setBoundaries(0, std::numeric_limits<int_type>::max()), Gem::Common::gemfony_error_condition);
		}

		//------------------------------------------------------------------------------

		{ // Check that setting a lower boundary smaller than the allowed value (see GConstrainedValueLimit<T>) with the setBoundaries(lower, upper) function throws
			boost::shared_ptr<GConstrainedIntT<int_type> > p_test = this->GObject::clone<GConstrainedIntT<int_type> >();

			// Reset the boundaries so we are free to do what we want
			BOOST_CHECK_NO_THROW(p_test->resetBoundaries());

			// Check that the boundaries have the expected values
			BOOST_CHECK(p_test->getLowerBoundary() == -GConstrainedValueLimit<int_type>::max());
			BOOST_CHECK(p_test->getUpperBoundary() ==  GConstrainedValueLimit<int_type>::max());

			// Try to set a boundary to a bad value
			BOOST_CHECK_THROW(p_test->setBoundaries(-std::numeric_limits<int_type>::max(), 100), Gem::Common::gemfony_error_condition);
		}

		//------------------------------------------------------------------------------

		{ // Test reversion of order
			boost::shared_ptr<GConstrainedIntT<int_type> > p_test = this->GObject::clone<GConstrainedIntT<int_type> >();

			// Reset the boundaries so we are free to do what we want
			BOOST_CHECK_NO_THROW(p_test->resetBoundaries());

			for(int_type i=1; i<100; i++) {
				int_type probe = this->GParameterBase::gr->uniform_int(i,2*i);
				BOOST_CHECK_NO_THROW(p_test->setValue(probe, i, 2*i));
				BOOST_CHECK(p_test->revert(probe) == p_test->getUpperBoundary() - (probe - p_test->getLowerBoundary()));
			}
		}

		//------------------------------------------------------------------------------
	}

	/****************************************************************************/
	/**
	 * Performs self tests that are expected to fail. This is needed for testing purposes
	 */
	virtual void specificTestsFailuresExpected_GUnitTests() {
		// Call the parent classes' functions
		GConstrainedNumT<int_type>::specificTestsFailuresExpected_GUnitTests();
	}

#endif /* GENEVATESTING */
};

} /* namespace Geneva */
} /* namespace Gem */

/******************************************************************************/
// The content of BOOST_SERIALIZATION_ASSUME_ABSTRACT(T)

namespace boost {
	namespace serialization {
		template<typename int_type>
		struct is_abstract<Gem::Geneva::GConstrainedIntT<int_type> > : public boost::true_type {};
		template<typename int_type>
		struct is_abstract< const Gem::Geneva::GConstrainedIntT<int_type> > : public boost::true_type {};
	}
}

#endif /* GCONSTRAINEDINTEGERT_HPP_ */

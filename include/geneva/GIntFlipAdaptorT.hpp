/**
 * @file GIntFlipAdaptorT.hpp
 */

/*
 * Copyright (C) Gemfony scientific UG (haftungsbeschraenkt)
 *
 * See the AUTHORS file in the top-level directory for a list of authors.
 *
 * Contact: contact [at] gemfony (dot) com
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
 * http://www.gemfony.com .
 */


// Standard headers go here

// Boost headers go here

#ifndef GINTFLIPADAPTORT_HPP_
#define GINTFLIPADAPTORT_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif

// Geneva headers go here
#include "common/GExceptions.hpp"
#include "geneva/GNumFlipAdaptorT.hpp"
#include "geneva/GConstrainedDoubleObject.hpp"
#include "geneva/GDoubleGaussAdaptor.hpp"
#include "geneva/GObject.hpp"
#include "geneva/GOptimizationEnums.hpp"

namespace Gem {
namespace Geneva {

/************************************************************************************************/
/**
 * GIntFlipAdaptorT represents an adaptor used for the adaption of integer
 * types, by flipping an integer number to the next larger or smaller number.
 * The integer type used needs to be specified as a template parameter. Note
 * that a specialization of this class, as defined in GIntFlipAdaptorT.cpp,
 * allows to deal with booleans instead of "standard" integer types.
 */
template<typename int_type>
class GIntFlipAdaptorT
	:public GNumFlipAdaptorT<int_type>
{
	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int) {
		using boost::serialization::make_nvp;
		ar & make_nvp("GNumFlipAdaptorT", boost::serialization::base_object<GNumFlipAdaptorT<int_type> >(*this));
	}
	///////////////////////////////////////////////////////////////////////

public:
	/********************************************************************************************/
	/**
	 * The standard constructor.
	 */
	GIntFlipAdaptorT()
		: GNumFlipAdaptorT<int_type> ()
	{ /* nothing */	}

	/********************************************************************************************/
	/**
	 * This constructor takes an argument, that specifies the (initial) probability
	 * for the adaption of an integer or bit value
	 *
	 * @param prob The probability for a flip
	 */
	explicit GIntFlipAdaptorT(const double& prob)
		: GNumFlipAdaptorT<int_type>(prob)
	{ /* nothing */ }

	/********************************************************************************************/
	/**
	 * A standard copy constructor.
	 *
	 * @param cp Another GIntFlipAdaptorT object
	 */
	GIntFlipAdaptorT(const GIntFlipAdaptorT<int_type>& cp)
		: GNumFlipAdaptorT<int_type>(cp)
	{ /* nothing */	}

	/********************************************************************************************/
	/**
	 * The standard destructor. Empty, as we have no local, dynamically
	 * allocated data.
	 */
	virtual ~GIntFlipAdaptorT()
	{ /* nothing */ }

	/********************************************************************************************/
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
	boost::optional<std::string> checkRelationshipWith(
			const GObject& cp
			, const Gem::Common::expectation& e
			, const double& limit
			, const std::string& caller
			, const std::string& y_name
			, const bool& withMessages
	) const	{
	    using namespace Gem::Common;

		// Check that we are indeed dealing with a GParamterBase reference
		const GIntFlipAdaptorT<int_type>  *p_load = GObject::gobject_conversion<GIntFlipAdaptorT<int_type> >(&cp);

		// Will hold possible deviations from the expectation, including explanations
	    std::vector<boost::optional<std::string> > deviations;

		// Check our parent class'es data ...
		deviations.push_back(GNumFlipAdaptorT<int_type>::checkRelationshipWith(cp, e, limit, "GIntFlipAdaptorT<int_type>", y_name, withMessages));

		// no local data ...

		return evaluateDiscrepancies("GIntFlipAdaptorT<int_type>", caller, deviations, e);
	}

	/********************************************************************************************/
	/**
	 * Retrieves the id of the adaptor. Purely virtual, as we do not want this class to be
	 * instantiated.
	 *
	 * @return The id of the adaptor
	 */
	virtual Gem::Geneva::adaptorId getAdaptorId() const = 0;

	/* ----------------------------------------------------------------------------------
	 * Tested in GInt32FlipAdaptor::specificTestsNoFailuresExpected_GUnitTests()
	 * Tested in GBooleanAdaptor::specificTestsNoFailuresExpected_GUnitTests()
	 * ----------------------------------------------------------------------------------
	 */

protected:
	/********************************************************************************************/
	/**
	 * This function loads the data of another GIntFlipAdaptorT, camouflaged as a GObject.
	 *
	 * @param A copy of another GIntFlipAdaptorT, camouflaged as a GObject
	 */
	void load_(const GObject *cp)
	{
		// Check that this object is not accidently assigned to itself
		GObject::selfAssignmentCheck<GIntFlipAdaptorT<int_type> >(cp);

		// Load the data of our parent class ...
		GNumFlipAdaptorT<int_type>::load_(cp);

		// no local data
	}

	/********************************************************************************************/
	/**
	 * This function creates a deep copy of this object. The function is purely virtual, as we
	 * do not want this class to be instantiated. Use the derived classes instead.
	 *
	 * @return A deep copy of this object
	 */
	virtual GObject *clone_() const = 0;

#ifdef GEM_TESTING
public:
	/*******************************************************************************************/
	/**
	 * Applies modifications to this object. This is needed for testing purposes
	 *
	 * @return A boolean which indicates whether modifications were made
	 */
	virtual bool modify_GUnitTests() {
		using boost::unit_test_framework::test_suite;
		using boost::unit_test_framework::test_case;

		bool result = false;

		// Call the parent classes' functions
		if(GNumFlipAdaptorT<int_type>::modify_GUnitTests()) result = true;

		// no local data -- nothing to change

		return result;
	}

	/*******************************************************************************************/
	/**
	 * Performs self tests that are expected to succeed. This is needed for testing purposes
	 */
	virtual void specificTestsNoFailureExpected_GUnitTests() {
		using boost::unit_test_framework::test_suite;
		using boost::unit_test_framework::test_case;

		// Call the parent classes' functions
		GNumFlipAdaptorT<int_type>::specificTestsNoFailureExpected_GUnitTests();
	}

	/*******************************************************************************************/
	/**
	 * Performs self tests that are expected to fail. This is needed for testing purposes
	 */
	virtual void specificTestsFailuresExpected_GUnitTests() {
		using boost::unit_test_framework::test_suite;
		using boost::unit_test_framework::test_case;

		// Call the parent classes' functions
		GNumFlipAdaptorT<int_type>::specificTestsFailuresExpected_GUnitTests();
	}

#endif /* GEM_TESTING */
};

/************************************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */

/********************************************************************************************/
// The content of BOOST_SERIALIZATION_ASSUME_ABSTRACT(T)

namespace boost {
	namespace serialization {
		template<typename int_type>
		struct is_abstract<Gem::Geneva::GIntFlipAdaptorT<int_type> > : public boost::true_type {};
		template<typename int_type>
		struct is_abstract< const Gem::Geneva::GIntFlipAdaptorT<int_type> > : public boost::true_type {};
	}
}

/********************************************************************************************/

#endif /* GINTFLIPADAPTORT_HPP_ */

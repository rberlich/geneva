/**
 * @file GBooleanAdaptor.cpp
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
#include "geneva/GBooleanAdaptor.hpp"

BOOST_CLASS_EXPORT_IMPLEMENT(Gem::Geneva::GBooleanAdaptor)

namespace Gem {
namespace Geneva {

/******************************************************************************/
/**
 * The default constructor
 */
GBooleanAdaptor::GBooleanAdaptor()
	: GAdaptorT<bool>(DEFAULTBITADPROB)
{ /* nothing */ }

// Tested in this class

/******************************************************************************/
/**
 * The copy constructor
 *
 * @param cp A copy of another GBooleanAdaptor object
 */
GBooleanAdaptor::GBooleanAdaptor(const GBooleanAdaptor& cp)
	: GAdaptorT<bool>(cp)
{ /* nothing */ }

// Tested in this class

/******************************************************************************/
/**
 * Initialization with a adaption probability
 *
 * @param adProb The adaption probability
 */
GBooleanAdaptor::GBooleanAdaptor(const double& adProb)
	: GAdaptorT<bool>(adProb)
{ /* nothing */ }

// Tested in this class

/******************************************************************************/
/**
 * The destructor
 */
GBooleanAdaptor::~GBooleanAdaptor()
{ /* nothing */ }

/******************************************************************************/
/**
 * A standard assignment operator.
 *
 * @param cp A copy of another GBooleanAdaptor object
 * @return A constant reference to this object
 */
const GBooleanAdaptor& GBooleanAdaptor::operator=(const GBooleanAdaptor& cp){
	GBooleanAdaptor::load_(&cp);
	return *this;
}

/******************************************************************************/
/**
 * Creates a deep clone of this object.
 *
 * @return A copy of this object, camouflaged as a GObject
 */
GObject* GBooleanAdaptor::clone_() const {
	return new GBooleanAdaptor(*this);
}

/******************************************************************************/
/**
 * Flip the value up or down by 1, depending on a random number
 */
void GBooleanAdaptor::customAdaptions(
   bool& value
   , const bool& range
) {
   value==true?value=false:value=true;
}

/******************************************************************************/
/**
 * Checks for equality with another GBooleanAdaptor object
 *
 * @param  cp A constant reference to another GBooleanAdaptor object
 * @return A boolean indicating whether both objects are equal
 */
bool GBooleanAdaptor::operator==(const GBooleanAdaptor& cp) const {
	using namespace Gem::Common;
	// Means: The expectation of equality was fulfilled, if no error text was emitted (which converts to "true")
	return !checkRelationshipWith(cp, CE_EQUALITY, 0.,"GBooleanAdaptor::operator==","cp", CE_SILENT);
}

/******************************************************************************/
/**
 * Checks for inequality with another GBooleanAdaptor object
 *
 * @param  cp A constant reference to another GBooleanAdaptor object
 * @return A boolean indicating whether both objects are inequal
 */
bool GBooleanAdaptor::operator!=(const GBooleanAdaptor& cp) const {
	using namespace Gem::Common;
	// Means: The expectation of inequality was fulfilled, if no error text was emitted (which converts to "true")
	return !checkRelationshipWith(cp, CE_INEQUALITY, 0.,"GBooleanAdaptor::operator!=","cp", CE_SILENT);
}

/******************************************************************************/
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
boost::optional<std::string> GBooleanAdaptor::checkRelationshipWith(const GObject& cp,
		const Gem::Common::expectation& e,
		const double& limit,
		const std::string& caller,
		const std::string& y_name,
		const bool& withMessages) const
{
    using namespace Gem::Common;

	// Check for a possible self-assignment
	GObject::selfAssignmentCheck<GBooleanAdaptor>(&cp);

	// Will hold possible deviations from the expectation, including explanations
    std::vector<boost::optional<std::string> > deviations;

	// Check our parent class'es data ...
	deviations.push_back(GAdaptorT<bool>::checkRelationshipWith(cp, e, limit, "GBooleanAdaptor", y_name, withMessages));

	// no local data ...

	return evaluateDiscrepancies("GBooleanAdaptor", caller, deviations, e);
}

/***********************************************************************************/
/**
 * Emits a name for this class / object
 */
std::string GBooleanAdaptor::name() const {
   return std::string("GBooleanAdaptor");
}

/***********************************************************************************//**
 * Allows to randomly initialize parameter members. No local data, hence no
 * action taken.
 */
void GBooleanAdaptor::randomInit()
{ /* nothing - no local data */ }

/******************************************************************************/
/**
 * Loads the data of another GObject
 *
 * @param cp A copy of another GBooleanAdaptor object, camouflaged as a GObject
 */
void GBooleanAdaptor::load_(const GObject* cp){
	// Check for a possible self-assignment
	GObject::selfAssignmentCheck<GBooleanAdaptor>(cp);

	// Load our parent class'es data ...
	GAdaptorT<bool>::load_(cp);

	// ... no local data
}

/******************************************************************************/
/**
 * Retrieves the id of this adaptor
 *
 * @return The id of this adaptor
 */
Gem::Geneva::adaptorId GBooleanAdaptor::getAdaptorId() const {
	return Gem::Geneva::GBOOLEANADAPTOR;
}

/* ----------------------------------------------------------------------------------
 * Tested in GBooleanAdaptor::specificTestsNoFailuresExpected_GUnitTests()
 * ----------------------------------------------------------------------------------
 */

/******************************************************************************/
/**
 * Applies modifications to this object. This is needed for testing purposes
 *
 * @return A boolean which indicates whether modifications were made
 */
bool GBooleanAdaptor::modify_GUnitTests() {
#ifdef GEM_TESTING
	using boost::unit_test_framework::test_suite;
	using boost::unit_test_framework::test_case;

	bool result = false;

	// Call the parent class'es function
	if(GAdaptorT<bool>::modify_GUnitTests()) result = true;

	return result;
#else /* GEM_TESTING */  // If this function is called when GEM_TESTING isn't set, throw
   condnotset("GBooleanAdaptor::modify_GUnitTests", "GEM_TESTING");
   return false;
#endif /* GEM_TESTING */
}

/******************************************************************************/
/**
 * Performs self tests that are expected to succeed. This is needed for testing purposes
 */
void GBooleanAdaptor::specificTestsNoFailureExpected_GUnitTests() {
#ifdef GEM_TESTING
	using boost::unit_test_framework::test_suite;
	using boost::unit_test_framework::test_case;

	// Call the parent class'es function
	GAdaptorT<bool>::specificTestsNoFailureExpected_GUnitTests();

	// --------------------------------------------------------------------------

	{ // Check default construction
		GBooleanAdaptor gba;
		BOOST_CHECK_MESSAGE(
				gba.getAdaptionProbability() == DEFAULTBITADPROB
				, "\n"
				<< "gba.getAdaptionProbability() = " << gba.getAdaptionProbability() << "\n"
				<< "DEFAULTADPROB = " << DEFAULTBITADPROB
		);
	}

	// --------------------------------------------------------------------------

	{ // Check construction with a given adaption probability
		const double TRIALADPROB = 0.1;
		GBooleanAdaptor gba(TRIALADPROB);
		BOOST_CHECK_MESSAGE(
				gba.getAdaptionProbability() == TRIALADPROB
				, "\n"
				<< "gba.getAdaptionProbability() = " << gba.getAdaptionProbability()
				<< "TRIALADPROB = " << TRIALADPROB
		);
	}

	// --------------------------------------------------------------------------

	{ // Check copy construction
		const double TRIALADPROB = 0.1;
		GBooleanAdaptor gba1(TRIALADPROB);
		GBooleanAdaptor gba2(gba1);
		BOOST_CHECK_MESSAGE(
				gba2.getAdaptionProbability() == TRIALADPROB
				, "\n"
				<< "gba2.getAdaptionProbability() = " << gba2.getAdaptionProbability()
				<< "TRIALADPROB = " << TRIALADPROB
		);
	}

	// --------------------------------------------------------------------------

	{ // Check that the adaptor returns the correct adaptor id
		boost::shared_ptr<GBooleanAdaptor> p_test = this->clone<GBooleanAdaptor>();

		BOOST_CHECK_MESSAGE(
			p_test->getAdaptorId() == GBOOLEANADAPTOR
			,  "\n"
			<< "p_test->getAdaptorId() = " << p_test->getAdaptorId()
			<< "GBOOLEANADAPTOR        = " << GBOOLEANADAPTOR << "\n"
		);
	}

	// --------------------------------------------------------------------------

#else /* GEM_TESTING */ // If this function is called when GEM_TESTING isn't set, throw
   condnotset("GBooleanAdaptor::specificTestsNoFailureExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
}

/******************************************************************************/
/**
 * Performs self tests that are expected to fail. This is needed for testing purposes
 */
void GBooleanAdaptor::specificTestsFailuresExpected_GUnitTests() {
#ifdef GEM_TESTING
	using boost::unit_test_framework::test_suite;
	using boost::unit_test_framework::test_case;

	// Call the parent class'es function
	GAdaptorT<bool>::specificTestsFailuresExpected_GUnitTests();

	// no local data - nothing to test

#else /* GEM_TESTING */ // If this function is called when GEM_TESTING isn't set, throw
   condnotset("GBooleanAdaptor::specificTestsFailuresExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
}

/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */

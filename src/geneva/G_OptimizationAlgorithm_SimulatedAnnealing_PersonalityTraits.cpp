/**
 * @file
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

#include "geneva/G_OptimizationAlgorithm_SimulatedAnnealing_PersonalityTraits.hpp"

BOOST_CLASS_EXPORT_IMPLEMENT(Gem::Geneva::GSimulatedAnnealing_PersonalityTraits)

namespace Gem {
namespace Geneva {

/******************************************************************************/
/** A short identifier suitable for storage in a std::map */
G_API_GENEVA const std::string GSimulatedAnnealing_PersonalityTraits::nickname = "sa";

/******************************************************************************/
/**
 * The default constructor
 */
GSimulatedAnnealing_PersonalityTraits::GSimulatedAnnealing_PersonalityTraits()
	: GBaseParChildPersonalityTraits() { /* nothing */ }

/******************************************************************************/
/**
 * The copy contructor
 *
 * @param cp A copy of another GSAPersonalityTraits object
 */
GSimulatedAnnealing_PersonalityTraits::GSimulatedAnnealing_PersonalityTraits(const GSimulatedAnnealing_PersonalityTraits &cp)
	: GBaseParChildPersonalityTraits(cp) { /* nothing */ }

/******************************************************************************/
/**
 * The standard destructor
 */
GSimulatedAnnealing_PersonalityTraits::~GSimulatedAnnealing_PersonalityTraits() { /* nothing */ }

/******************************************************************************/
/**
 * Searches for compliance with expectations with respect to another object
 * of the same type
 *
 * @param cp A constant reference to another GObject object
 * @param e The expected outcome of the comparison
 * @param limit The maximum deviation for floating point values (important for similarity checks)
 */
void GSimulatedAnnealing_PersonalityTraits::compare_(
	const GObject &cp, const Gem::Common::expectation &e, const double &limit
) const {
	using namespace Gem::Common;

	// Check that we are dealing with a GSimulatedAnnealing_PersonalityTraits reference independent of this object and convert the pointer
	const GSimulatedAnnealing_PersonalityTraits *p_load = Gem::Common::g_convert_and_compare<GObject, GSimulatedAnnealing_PersonalityTraits>(cp, this);

	GToken token("GSimulatedAnnealing_PersonalityTraits", e);

	// Compare our parent data ...
	Gem::Common::compare_base_t<GBaseParChildPersonalityTraits>(*this, *p_load, token);

	// ... no local data

	// React on deviations from the expectation
	token.evaluate();
}

/******************************************************************************/
/**
 * Emits a name for this class / object
 */
std::string GSimulatedAnnealing_PersonalityTraits::name_() const {
	return std::string("GSimulatedAnnealing_PersonalityTraits");
}

/******************************************************************************/
/**
 * Retrieves the mnemonic of the optimization algorithm
 */
std::string GSimulatedAnnealing_PersonalityTraits::getMnemonic() const {
	return GSimulatedAnnealing_PersonalityTraits::nickname;
}

/******************************************************************************/
/**
 * Creates a deep clone of this object
 *
 * @return A clone of this object, camouflaged as a GObject
 */
GObject *GSimulatedAnnealing_PersonalityTraits::clone_() const {
	return new GSimulatedAnnealing_PersonalityTraits(*this);
}

/******************************************************************************/
/**
 * Loads the data of another GSAPersonalityTraits object
 *
 * @param cp A copy of another GSAPersonalityTraits object, camouflaged as a GObject
 */
void GSimulatedAnnealing_PersonalityTraits::load_(const GObject *cp) {
	// Check that we are dealing with a GSimulatedAnnealing_PersonalityTraits reference independent of this object and convert the pointer
	const GSimulatedAnnealing_PersonalityTraits *p_load = Gem::Common::g_convert_and_compare<GObject, GSimulatedAnnealing_PersonalityTraits>(cp, this);

	// Load the parent class'es data
	GBaseParChildPersonalityTraits::load_(cp);

	// Then load our local data
	// no local data ...
}

/******************************************************************************/
/**
 * Applies modifications to this object. This is needed for testing purposes
 *
 * @return A boolean which indicates whether modifications were made
 */
bool GSimulatedAnnealing_PersonalityTraits::modify_GUnitTests() {
#ifdef GEM_TESTING
	bool result = false;

	// Call the parent class'es function
	if (GBaseParChildPersonalityTraits::modify_GUnitTests()) result = true;

	return result;
#else /* GEM_TESTING */  // If this function is called when GEM_TESTING isn't set, throw
	Gem::Common::condnotset("GSimulatedAnnealing_PersonalityTraits::modify_GUnitTests", "GEM_TESTING");
   return false;
#endif /* GEM_TESTING */
}

/******************************************************************************/
/**
 * Performs self tests that are expected to succeed. This is needed for testing purposes
 */
void GSimulatedAnnealing_PersonalityTraits::specificTestsNoFailureExpected_GUnitTests() {
#ifdef GEM_TESTING
	using boost::unit_test_framework::test_suite;
	using boost::unit_test_framework::test_case;

	// Call the parent class'es function
	GBaseParChildPersonalityTraits::specificTestsNoFailureExpected_GUnitTests();

	// --------------------------------------------------------------------------
	// --------------------------------------------------------------------------

#else /* GEM_TESTING */ // If this function is called when GEM_TESTING isn't set, throw
	Gem::Common::condnotset("GSimulatedAnnealing_PersonalityTraits::specificTestsNoFailureExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
}

/******************************************************************************/
/**
 * Performs self tests that are expected to fail. This is needed for testing purposes
 */
void GSimulatedAnnealing_PersonalityTraits::specificTestsFailuresExpected_GUnitTests() {
#ifdef GEM_TESTING
	using boost::unit_test_framework::test_suite;
	using boost::unit_test_framework::test_case;

	// Call the parent class'es function
	GBaseParChildPersonalityTraits::specificTestsFailuresExpected_GUnitTests();

	// --------------------------------------------------------------------------
	// --------------------------------------------------------------------------

#else /* GEM_TESTING */ // If this function is called when GEM_TESTING isn't set, throw
	Gem::Common::condnotset("GSimulatedAnnealing_PersonalityTraits::specificTestsFailuresExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
}

/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */

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

#include "geneva/G_OptimizationAlgorithm_ParChildT_PersonalityTraits.hpp"

BOOST_CLASS_EXPORT_IMPLEMENT(Gem::Geneva::GBaseParChildPersonalityTraits)

namespace Gem {
namespace Geneva {

/******************************************************************************/
/**
 * Searches for compliance with expectations with respect to another object
 * of the same type
 *
 * @param cp A constant reference to another GObject object
 * @param e The expected outcome of the comparison
 * @param limit The maximum deviation for floating point values (important for similarity checks)
 */
void GBaseParChildPersonalityTraits::compare_(
	const GObject &cp
	, const Gem::Common::expectation &e
	, const double &limit
) const {
	using namespace Gem::Common;

	// Check that we are dealing with a GBasePS::GBaseParChildPersonalityTraits reference independent of this object and convert the pointer
	const GBaseParChildPersonalityTraits *p_load = Gem::Common::g_convert_and_compare<GObject, GBaseParChildPersonalityTraits>(cp, this);

	GToken token("GBaseParChildPersonalityTraits", e);

	// Compare our parent data ...
	Gem::Common::compare_base_t<GObject>(*this, *p_load, token);

	// ... and then the local data
	compare_t(IDENTITY(parentCounter_, p_load->parentCounter_), token);
	compare_t(IDENTITY(popPos_, p_load->popPos_), token);
	compare_t(IDENTITY(parentId_, p_load->parentId_), token);

	// React on deviations from the expectation
	token.evaluate();
}

/******************************************************************************/
/**
 * Emits a name for this class / object
 */
std::string GBaseParChildPersonalityTraits::name_() const {
	return std::string("GBaseParChildPersonalityTraits");
}

/******************************************************************************/
/**
 * Retrieves the mnemonic of the optimization algorithm
 */
std::string GBaseParChildPersonalityTraits::getMnemonic() const {
	throw gemfony_exception(
		g_error_streamer(DO_LOG,  time_and_place)
			<< "In GBaseParChildPersonalityTraits::getMnemonic(): Error!" << std::endl
			<< "This function should never have been called" << std::endl
	);

	return "none";
}

/******************************************************************************/
/**
 * Creates a deep clone of this object
 *
 * @return A clone of this object, camouflaged as a GObject
 */
GObject *GBaseParChildPersonalityTraits::clone_() const {
	return new GBaseParChildPersonalityTraits(*this);
}

/******************************************************************************/
/**
 * Loads the data of another GBaseParChildPersonalityTraits object
 *
 * @param cp A copy of another GBaseParChildPersonalityTraits object, camouflaged as a GObject
 */
void GBaseParChildPersonalityTraits::load_(const GObject *cp) {
	// Check that we are dealing with a GBasePS::GBaseParChildPersonalityTraits reference independent of this object and convert the pointer
	const GBaseParChildPersonalityTraits *p_load = Gem::Common::g_convert_and_compare<GObject, GBaseParChildPersonalityTraits>(cp, this);

	// Load the parent class'es data
	GPersonalityTraits::load_(cp);

	// Then load our local data
	parentCounter_ = p_load->parentCounter_;
	popPos_ = p_load->popPos_;
	parentId_ = p_load->parentId_;
}

/******************************************************************************/
/**
 * Checks whether this is a parent individual
 *
 * @return A boolean indicating whether this object is a parent at this time
 */
bool GBaseParChildPersonalityTraits::isParent() const {
	return (parentCounter_ > 0) ? true : false;
}

/* ----------------------------------------------------------------------------------
 * Tested in GBaseParChildPersonalityTraits::specificTestsNoFailuresExpected_GUnitTests()
 * ----------------------------------------------------------------------------------
 */

/******************************************************************************/
/**
 * Retrieves the current value of the parentCounter_ variable
 *
 * @return The current value of the parentCounter_ variable
 */
std::uint32_t GBaseParChildPersonalityTraits::getParentCounter() const {
	return parentCounter_;
}

/* ----------------------------------------------------------------------------------
 * Tested in GBaseParChildPersonalityTraits::specificTestsNoFailuresExpected_GUnitTests()
 * ----------------------------------------------------------------------------------
 */

/******************************************************************************/
/**
 * Marks an individual as a parent
 *
 * @return A boolean indicating whether this individual was previously a parent (true) or a child (false)
 */
bool GBaseParChildPersonalityTraits::setIsParent() {
	bool previous = (parentCounter_ > 0) ? true : false;
	parentCounter_++;
	return previous;
}

/* ----------------------------------------------------------------------------------
 * Tested in GBaseParChildPersonalityTraits::specificTestsNoFailuresExpected_GUnitTests()
 * ----------------------------------------------------------------------------------
 */

/******************************************************************************/
/**
 * Marks an individual as a child
 *
 * @return A boolean indicating whether this individual was previously a parent (true) or a child (false)
 */
bool GBaseParChildPersonalityTraits::setIsChild() {
	bool previous = (parentCounter_ > 0) ? true : false;
	parentCounter_ = 0;
	return previous;
}

/* ----------------------------------------------------------------------------------
 * Tested in GBaseParChildPersonalityTraits::specificTestsNoFailuresExpected_GUnitTests()
 * ----------------------------------------------------------------------------------
 */

/******************************************************************************/
/**
 * Sets the position of the individual in the population
 *
 * @param popPos The new position of this individual in the population
 */
void GBaseParChildPersonalityTraits::setPopulationPosition(const std::size_t &popPos) {
	popPos_ = popPos;
}

/* ----------------------------------------------------------------------------------
 * Tested in GBaseParChildPersonalityTraits::specificTestsNoFailuresExpected_GUnitTests()
 * ----------------------------------------------------------------------------------
 */

/******************************************************************************/
/**
 * Retrieves the position of the individual in the population
 *
 * @return The current position of this individual in the population
 */
std::size_t GBaseParChildPersonalityTraits::getPopulationPosition(void) const {
	return popPos_;
}

/* ----------------------------------------------------------------------------------
 * Tested in GBaseParChildPersonalityTraits::specificTestsNoFailuresExpected_GUnitTests()
 * ----------------------------------------------------------------------------------
 */

/******************************************************************************/
/**
 * Stores the parent's id with this object.
 *
 * @param parentId The id of the individual's parent
 */
void GBaseParChildPersonalityTraits::setParentId(const std::size_t &parentId) {
	parentId_ = (std::int16_t) parentId;
}

/* ----------------------------------------------------------------------------------
 * Tested in GBaseParChildPersonalityTraits::specificTestsNoFailuresExpected_GUnitTests()
 * ----------------------------------------------------------------------------------
 */

/******************************************************************************/
/**
 * Retrieves the parent id's value. Note that this function will throw if
 * no parent id has been set.
 *
 * @return The parent's id
 */
std::size_t GBaseParChildPersonalityTraits::getParentId() const {
	if (parentId_ >= 0) return parentId_;
	else {
		throw gemfony_exception(
			g_error_streamer(DO_LOG,  time_and_place)
				<< "In GBaseParChildPersonalityTraits::getParentId():" << std::endl
				<< "parentId_ is unset" << std::endl
		);
	}

	// Make the compiler happy
	return std::size_t(0);
}

/* ----------------------------------------------------------------------------------
 * Tested in GBaseParChildPersonalityTraits::specificTestsNoFailuresExpected_GUnitTests()
 * Tested in GBaseParChildPersonalityTraits::specificTestsFailuresExpected_GUnitTests()
 * ----------------------------------------------------------------------------------
 */

/******************************************************************************/
/**
 * Checks whether a parent id has been set
 *
 * @return A boolean which indicates whether the parent id has been set
 */
bool GBaseParChildPersonalityTraits::parentIdSet() const {
	if (parentId_ >= 0) return true;
	else return false;
}

/* ----------------------------------------------------------------------------------
 * Tested in GBaseParChildPersonalityTraits::specificTestsNoFailuresExpected_GUnitTests()
 * ----------------------------------------------------------------------------------
 */

/******************************************************************************/
/**
 * Marks the parent id as unset
 */
void GBaseParChildPersonalityTraits::unsetParentId() {
	parentId_ = -1;
}

/* ----------------------------------------------------------------------------------
 * Tested in GBaseParChildPersonalityTraits::specificTestsNoFailuresExpected_GUnitTests()
 * Tested in GBaseParChildPersonalityTraits::specificTestsFailuresExpected_GUnitTests()
 * ----------------------------------------------------------------------------------
 */

/******************************************************************************/
/**
 * Applies modifications to this object. This is needed for testing purposes
 *
 * @return A boolean which indicates whether modifications were made
 */
bool GBaseParChildPersonalityTraits::modify_GUnitTests_() {
#ifdef GEM_TESTING
	bool result = false;

	// Call the parent class'es function
	if (GPersonalityTraits::modify_GUnitTests_()) result = true;

	// A relatively harmless modification is a change of the parentCounter variable
	parentCounter_++;
	result = true;

	return result;

#else /* GEM_TESTING */  // If this function is called when GEM_TESTING isn't set, throw
	Gem::Common::condnotset("GBaseParChildPersonalityTraits::modify_GUnitTests", "GEM_TESTING");
   return false;
#endif /* GEM_TESTING */
}

/******************************************************************************/
/**
 * Performs self tests that are expected to succeed. This is needed for testing purposes
 */
void GBaseParChildPersonalityTraits::specificTestsNoFailureExpected_GUnitTests_() {
#ifdef GEM_TESTING
	using boost::unit_test_framework::test_suite;
	using boost::unit_test_framework::test_case;

	// Call the parent class'es function
	GPersonalityTraits::specificTestsNoFailureExpected_GUnitTests_();

	// --------------------------------------------------------------------------

	{ // Check that it is possible to mark this as a parent or child-entity
		std::shared_ptr <GBaseParChildPersonalityTraits> p_test = this->clone<GBaseParChildPersonalityTraits>();

		// Mark this object as belonging to a parent and check the correct setting
		BOOST_CHECK_NO_THROW(p_test->setIsParent());
		BOOST_CHECK(p_test->isParent() == true);

		// Mark this object as belonging to a child and check the correct setting
		BOOST_CHECK_NO_THROW(p_test->setIsChild());
		BOOST_CHECK(p_test->isParent() == false);
	}

	// --------------------------------------------------------------------------


	{ // Check that the parent counter is incremented or reset correctly
		std::shared_ptr <GBaseParChildPersonalityTraits> p_test = this->clone<GBaseParChildPersonalityTraits>();

		// Mark this object as belonging to a child and check the correct setting
		BOOST_CHECK_NO_THROW(p_test->setIsChild());
		BOOST_CHECK(p_test->isParent() == false);

		// Check that the parent counter is now 0
		BOOST_CHECK(p_test->getParentCounter() == 0);

		// Mark the individual as a parent a number of times and check the parent counter
		for (std::uint32_t i = 1; i <= 10; i++) {
			BOOST_CHECK_NO_THROW(p_test->setIsParent());
			BOOST_CHECK(p_test->getParentCounter() == i);
		}

		// Mark the individual as a child and check the parent counter again
		BOOST_CHECK_NO_THROW(p_test->setIsChild());
		BOOST_CHECK(p_test->isParent() == false);

		// Check that the parent counter is now 0
		BOOST_CHECK(p_test->getParentCounter() == 0);
	}

	// --------------------------------------------------------------------------

	{ // Check setting and retrieval of the individual's position in the population
		std::shared_ptr <GBaseParChildPersonalityTraits> p_test = this->clone<GBaseParChildPersonalityTraits>();

		for (std::size_t i = 0; i < 10; i++) {
			BOOST_CHECK_NO_THROW(p_test->setPopulationPosition(i));
			BOOST_CHECK(p_test->getPopulationPosition() == i);
		}
	}

	// --------------------------------------------------------------------------

	{ // Test setting and retrieval of valid parent ids
		std::shared_ptr <GBaseParChildPersonalityTraits> p_test = this->clone<GBaseParChildPersonalityTraits>();

		for (std::size_t i = 0; i < 10; i++) {
			BOOST_CHECK_NO_THROW(p_test->setParentId(i));
			BOOST_CHECK(p_test->getParentId() == i);
			BOOST_CHECK(p_test->parentIdSet() == true);
			BOOST_CHECK_NO_THROW(p_test->unsetParentId());
			BOOST_CHECK(p_test->parentIdSet() == false);
		}
	}

	// --------------------------------------------------------------------------

#else /* GEM_TESTING */ // If this function is called when GEM_TESTING isn't set, throw
	Gem::Common::condnotset("GBaseParChildPersonalityTraits::specificTestsNoFailureExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
}

/******************************************************************************/
/**
 * Performs self tests that are expected to fail. This is needed for testing purposes
 */
void GBaseParChildPersonalityTraits::specificTestsFailuresExpected_GUnitTests_() {
#ifdef GEM_TESTING
	using boost::unit_test_framework::test_suite;
	using boost::unit_test_framework::test_case;

	// Call the parent class'es function
	GPersonalityTraits::specificTestsFailuresExpected_GUnitTests_();

	// --------------------------------------------------------------------------

	{ // Test that retrieval of the parent id throws, if the id isn't set
		std::shared_ptr <GBaseParChildPersonalityTraits> p_test = this->clone<GBaseParChildPersonalityTraits>();

		BOOST_CHECK_NO_THROW(p_test->unsetParentId());
		BOOST_CHECK_THROW(p_test->getParentId(), gemfony_exception);
	}

	// --------------------------------------------------------------------------

#else /* GEM_TESTING */ // If this function is called when GEM_TESTING isn't set, throw
	Gem::Common::condnotset("GBaseParChildPersonalityTraits::specificTestsFailuresExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
}

/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */

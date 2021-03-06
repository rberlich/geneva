/********************************************************************************
 *
 * This file is part of the Geneva library collection. The following license
 * applies to this file:
 *
 * ------------------------------------------------------------------------------
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ------------------------------------------------------------------------------
 *
 * Note that other files in the Geneva library collection may use a different
 * license. Please see the licensing information in each file.
 *
 ********************************************************************************
 *
 * Geneva was started by Dr. Rüdiger Berlich and was later maintained together
 * with Dr. Ariel Garcia under the auspices of Gemfony scientific. For further
 * information on Gemfony scientific, see http://www.gemfomy.eu .
 *
 * The majority of files in Geneva was released under the Apache license v2.0
 * in February 2020.
 *
 * See the NOTICE file in the top-level directory of the Geneva library
 * collection for a list of contributors and copyright information.
 *
 ********************************************************************************/

#include "geneva-individuals/GTestIndividual2.hpp"

BOOST_CLASS_EXPORT_IMPLEMENT(Gem::Tests::GTestIndividual2)

namespace Gem {
namespace Tests {

/******************************************************************************/
/**
 * Puts a Gem::Tests::PERFOBJECTTYPE item into a stream
 *
 * @param o The ostream the item should be added to
 * @param lt the item to be added to the stream
 * @return The std::ostream object used to add the item to
 */
std::ostream &operator<<(std::ostream &o, const Gem::Tests::PERFOBJECTTYPE &lt) {
	Gem::Common::ENUMBASETYPE tmp = static_cast<Gem::Common::ENUMBASETYPE>(lt);
	o << tmp;
	return o;
}

/******************************************************************************/
/**
 * Reads a Gem::Tests::PERFOBJECTTYPE item from a stream
 *
 * @param i The stream the item should be read from
 * @param lt The item read from the stream
 * @return The std::istream object used to read the item from
 */
std::istream &operator>>(std::istream &i, Gem::Tests::PERFOBJECTTYPE &lt) {
	Gem::Common::ENUMBASETYPE tmp;
	i >> tmp;

#ifdef DEBUG
	lt = boost::numeric_cast<Gem::Tests::PERFOBJECTTYPE>(tmp);
#else
	lt = static_cast<Gem::Tests::PERFOBJECTTYPE>(tmp);
#endif /* DEBUG */

	return i;
}

/******************************************************************************/
/**
 * The default constructor -- private, as it is only needed for (de-)serialization purposes
 */
GTestIndividual2::GTestIndividual2()
	: GParameterSet() { /* nothing */ }

/******************************************************************************/
/**
 * The standard constructor
 *
 * @param nObjects The number of parameters to be added to this individual
 */
GTestIndividual2::GTestIndividual2(const std::size_t &nObjects, const PERFOBJECTTYPE &otype)
	: GParameterSet() {
	using namespace Gem::Geneva;

	// Fill with the requested amount of data of the requested type
	switch (otype) {
		case PERFOBJECTTYPE::PERFGDOUBLEOBJECT: {
			for (std::size_t i = 0; i < nObjects; i++) {
				std::shared_ptr <GDoubleObject> gdo_ptr(new GDoubleObject(0.));
				std::shared_ptr <GDoubleGaussAdaptor> gdga_ptr(new GDoubleGaussAdaptor(0.025, 0.1, 0., 1., 1.));
				gdo_ptr->addAdaptor(gdga_ptr);
				this->push_back(gdo_ptr);
			}
		} break;

		case PERFOBJECTTYPE::PERFGCONSTRDOUBLEOBJECT: {
			for (std::size_t i = 0; i < nObjects; i++) {
				std::shared_ptr <GConstrainedDoubleObject> gcdo_ptr(new GConstrainedDoubleObject(0., -10., 10.));
				std::shared_ptr <GDoubleGaussAdaptor> gdga_ptr(new GDoubleGaussAdaptor(0.025, 0.1, 0., 1., 1.));
				gcdo_ptr->addAdaptor(gdga_ptr);
				this->push_back(gcdo_ptr);
			}
		} break;

		case PERFOBJECTTYPE::PERFGCONSTRAINEDDOUBLEOBJECTCOLLECTION: {
			std::shared_ptr <GConstrainedDoubleObject> gcdo_ptr(new GConstrainedDoubleObject(0., -10., 10.));
			std::shared_ptr <GDoubleGaussAdaptor> gdga_ptr(new GDoubleGaussAdaptor(0.025, 0.1, 0., 1., 1.));
			gcdo_ptr->addAdaptor(gdga_ptr);
			std::shared_ptr <GConstrainedDoubleObjectCollection> gcdc_ptr(
				new GConstrainedDoubleObjectCollection(nObjects, gcdo_ptr));
			this->push_back(gcdc_ptr);
		} break;

		case PERFOBJECTTYPE::PERFGDOUBLECOLLECTION: {
			std::shared_ptr <GDoubleCollection> gdc_ptr(new GDoubleCollection(nObjects, 0., -10., 10.));
			std::shared_ptr <GDoubleGaussAdaptor> gdga_ptr(new GDoubleGaussAdaptor(0.025, 0.1, 0., 1., 1.));
			gdc_ptr->addAdaptor(gdga_ptr);
			this->push_back(gdc_ptr);
		} break;

		case PERFOBJECTTYPE::PERFGCONSTRAINEDDOUBLECOLLECTION: {
			std::shared_ptr <GConstrainedDoubleCollection> gcdc_ptr(
				new GConstrainedDoubleCollection(nObjects, 0., -10., 10.));
			std::shared_ptr <GDoubleGaussAdaptor> gdga_ptr(new GDoubleGaussAdaptor(0.025, 0.1, 0., 1., 1.));
			gcdc_ptr->addAdaptor(gdga_ptr);
			this->push_back(gcdc_ptr);
		} break;

		default: {
			glogger
			<< "In GTestIndividual2::GTestIndividual2(): Error!" << std::endl
			<< "Invalid object type requested: " << otype << std::endl
			<< GTERMINATION;
		} break;
	}
}

/******************************************************************************/
/**
 * The copy constructor
 *
 * @param cp A constant reference to another GTestIndividual2 object
 */
GTestIndividual2::GTestIndividual2(const GTestIndividual2 &cp)
	: Gem::Geneva::GParameterSet(cp) {   /* nothing */ }

/******************************************************************************/
/**
 * The standard destructor
 */
GTestIndividual2::~GTestIndividual2() { /* nothing */   }

/******************************************************************************/
/**
 * Searches for compliance with expectations with respect to another object
 * of the same type
 *
 * @param cp A constant reference to another GObject object
 * @param e The expected outcome of the comparison
 * @param limit The maximum deviation for floating point values (important for similarity checks)
 */
void GTestIndividual2::compare_(
	const GObject &cp, const Gem::Common::expectation &e, const double &limit
) const {
	using namespace Gem::Common;
	using namespace Gem::Geneva;

	// Check that we are dealing with a GTestIndividual2 reference independent of this object and convert the pointer
	const GTestIndividual2 *p_load = Gem::Common::g_convert_and_compare<GObject, GTestIndividual2>(cp, this);

	Gem::Common::GToken token("GTestIndividual2", e);

	// Compare our parent data ...
	Gem::Common::compare_base_t<GParameterSet>(*this, *p_load, token);

	// ...no local data

	// React on deviations from the expectation
	token.evaluate();
}

/******************************************************************************/
/**
 * Loads the data of another GTestIndividual2, camouflaged as a GObject.
 *
 * @param cp A copy of another GTestIndividual2, camouflaged as a GObject
 */
void GTestIndividual2::load_(const GObject *cp) {
	using namespace Gem::Common;
	using namespace Gem::Geneva;

	// Check that we are dealing with a GTestIndividual2 reference independent of this object and convert the pointer
	const GTestIndividual2 *p_load = Gem::Common::g_convert_and_compare<GObject, GTestIndividual2>(cp, this);

	// Load our parent's data
	GParameterSet::load_(cp);

	// no local data
}

/******************************************************************************/
/**
 * Creates a deep clone of this object
 *
 * @return A deep clone of this object, camouflaged as a GObject
 */
Gem::Geneva::GObject *GTestIndividual2::clone_() const {
	return new GTestIndividual2(*this);
}

/******************************************************************************/
/**
 * The actual fitness calculation takes place here.
 *
 * @param id The id of the target function (ignored here)
 * @return The value of this object
 */
double GTestIndividual2::fitnessCalculation() {
	double result = 0.;

	// We just calculate the square of all double values
	std::vector<double> parVec;
	this->streamline(parVec);

	// Calculate the value of the parabola
	for (std::size_t i = 0; i < parVec.size(); i++) {
		result += GSQUARED(parVec[i]);
	}

	return result;
}

/******************************************************************************/
/**
 * Applies modifications to this object. This is needed for testing purposes
 *
 * @return A boolean which indicates whether modifications were made
 */
bool GTestIndividual2::modify_GUnitTests_() {
#ifdef GEM_TESTING
	using boost::unit_test_framework::test_suite;
	using boost::unit_test_framework::test_case;

	bool result = false;

	// Call the parent classes' functions
	if (Gem::Geneva::GParameterSet::modify_GUnitTests_()) result = true;

	// Change the parameter settings
	result = true;

	return result;

#else /* GEM_TESTING */  // If this function is called when GEM_TESTING isn't set, throw
   Gem::Common::condnotset("GTestIndividual2::modify_GUnitTests", "GEM_TESTING");
   return false;
#endif /* GEM_TESTING */
}

/******************************************************************************/
/**
 * Performs self tests that are expected to succeed. This is needed for testing purposes
 */
void GTestIndividual2::specificTestsNoFailureExpected_GUnitTests_() {
#ifdef GEM_TESTING
	using namespace Gem::Geneva;

	using boost::unit_test_framework::test_suite;
	using boost::unit_test_framework::test_case;

	// Call the parent classes' functions
	Gem::Geneva::GParameterSet::specificTestsNoFailureExpected_GUnitTests_();

	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------

#else /* GEM_TESTING */ // If this function is called when GEM_TESTING isn't set, throw
   Gem::Common::condnotset("GTestIndividual2::specificTestsNoFailureExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
}

/******************************************************************************/
/**
 * Performs self tests that are expected to fail. This is needed for testing purposes
 */
void GTestIndividual2::specificTestsFailuresExpected_GUnitTests_() {
#ifdef GEM_TESTING
	using namespace Gem::Geneva;

	using boost::unit_test_framework::test_suite;
	using boost::unit_test_framework::test_case;

	// Call the parent classes' functions
	Gem::Geneva::GParameterSet::specificTestsFailuresExpected_GUnitTests_();

	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------

#else /* GEM_TESTING */ // If this function is called when GEM_TESTING isn't set, throw
   Gem::Common::condnotset("GTestIndividual2::specificTestsFailuresExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
}

/******************************************************************************/

} /* namespace Tests */
} /* namespace Gem */

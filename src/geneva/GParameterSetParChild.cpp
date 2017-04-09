/**
 * @file GParameterSetParChild.cpp
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

#include "geneva/GParameterSetParChild.hpp"

namespace Gem {
namespace Geneva {

/******************************************************************************/
/**
 * The default constructor
 */
GParameterSetParChild::GParameterSetParChild()
	: GBaseParChildT<GParameterSet>()
  	, amalgamationLikelihood_(DEFAULTAMALGAMATIONLIKELIHOOD)
{ /* nothing */ }

/******************************************************************************/
/**
 * A standard copy constructor
 *
 * @param cp Another GParameterSetParChild object
 */
GParameterSetParChild::GParameterSetParChild(const GParameterSetParChild &cp)
	: GBaseParChildT<GParameterSet>(cp)
   , amalgamationLikelihood_(cp.amalgamationLikelihood_)
{ /* nothing */ }

/******************************************************************************/
/**
 * The standard destructor
 */
GParameterSetParChild::~GParameterSetParChild()
{ /* nothing */ }

/***************************************************************************/
/**
 * The standard assignment operator
 */
const GParameterSetParChild &GParameterSetParChild::operator=(const GParameterSetParChild &cp) {
	this->load_(&cp);
	return *this;
}

/******************************************************************************/
/**
 * Checks for equality with another GParameterSetParChild object
 *
 * @param  cp A constant reference to another GParameterSetParChild object
 * @return A boolean indicating whether both objects are equal
 */
bool GParameterSetParChild::operator==(const GParameterSetParChild &cp) const {
	using namespace Gem::Common;
	try {
		this->compare(cp, Gem::Common::expectation::CE_EQUALITY, CE_DEF_SIMILARITY_DIFFERENCE);
		return true;
	} catch (g_expectation_violation &) {
		return false;
	}
}

/******************************************************************************/
/**
 * Checks for inequality with another GParameterSetParChild object
 *
 * @param  cp A constant reference to another GParameterSetParChild object
 * @return A boolean indicating whether both objects are inequal
 */
bool GParameterSetParChild::operator!=(const GParameterSetParChild &cp) const {
	using namespace Gem::Common;
	try {
		this->compare(cp, Gem::Common::expectation::CE_INEQUALITY, CE_DEF_SIMILARITY_DIFFERENCE);
		return true;
	} catch (g_expectation_violation &) {
		return false;
	}
}

/******************************************************************************/
/**
 * Searches for compliance with expectations with respect to another object
 * of the same type
 *
 * @param cp A constant reference to another GObject object
 * @param e The expected outcome of the comparison
 * @param limit The maximum deviation for floating point values (important for similarity checks)
 */
void GParameterSetParChild::compare(
	const GObject &cp, const Gem::Common::expectation &e, const double &limit
) const {
	using namespace Gem::Common;

	// Check that we are dealing with a GParameterSetParChild reference independent of this object and convert the pointer
	const GParameterSetParChild *p_load = Gem::Common::g_convert_and_compare<GObject, GParameterSetParChild>(cp, this);

	GToken token("GParameterSetParChild", e);

	// Compare our parent data ...
	Gem::Common::compare_base<GBaseParChildT<GParameterSet>>(IDENTITY(*this, *p_load), token);

	// ... and then the local data
	compare_t(IDENTITY(amalgamationLikelihood_, p_load->amalgamationLikelihood_), token);

	// React on deviations from the expectation
	token.evaluate();
}

/******************************************************************************/
/**
 * Adds local configuration options to a GParserBuilder object
 *
 * @param gpb The GParserBuilder object to which configuration options should be added
 */
void GParameterSetParChild::addConfigurationOptions(
	Gem::Common::GParserBuilder &gpb
) {
	// Call our parent class'es function
	GBaseParChildT<GParameterSet>::addConfigurationOptions(gpb);

	gpb.registerFileParameter<double>(
		"amalgamationLikelihood" // The name of the variable
		, DEFAULTAMALGAMATIONLIKELIHOOD // The default value
		, [this](double al) { this->setAmalgamationLikelihood(al); }
	)
	<< "The likelihood for parent individuals to be \"fused\" together" << std::endl
	<< "rather than \"just\" being created through duplication schemes";
}

/******************************************************************************/
/**
 * Allows to set the likelihood for amalgamation of two units to be
 * performed instead of "just" duplication.
 */
void GParameterSetParChild::setAmalgamationLikelihood(double amalgamationLikelihood) {
	if (amalgamationLikelihood < 0. || amalgamationLikelihood > 1.) {
		glogger
		<< "In GParameterSetParChild::setCrossOverLikelihood(" << amalgamationLikelihood << "): Error!" << std::endl
		<< "Received invalid likelihood for amalgamation. Must be in the range [0:1]." << std::endl
		<< GEXCEPTION;
	}

	amalgamationLikelihood_ = amalgamationLikelihood;
}

/******************************************************************************/
/**
 * Allows to retrieve the likelihood for amalgamation of two units to be
 * performed instead of "just" duplication.
 */
double GParameterSetParChild::getAmalgamationLikelihood() const {
	return amalgamationLikelihood_;
}

/******************************************************************************/
/**
 * This function assigns a new value to each child individual according to the chosen
 * recombination scheme.
 */
void GParameterSetParChild::doRecombine() {
	std::size_t i;
	std::vector<double> threshold(m_n_parents);
	double thresholdSum = 0.;
	// TODO: Check whether it is sufficient to do this only once
	if (duplicationScheme::VALUEDUPLICATIONSCHEME == m_recombination_method && m_n_parents > 1) {          // Calculate a weight vector
		for (i = 0; i < m_n_parents; i++) {
			thresholdSum += 1. / (static_cast<double>(i) + 2.);
		}
		for (i = 0; i < m_n_parents - 1; i++) {
			// Normalizing the sum to 1
			threshold[i] = (1. / (static_cast<double>(i) + 2.)) / thresholdSum;

			// Make sure the subsequent range is in the right position
			if (i > 0) threshold[i] += threshold[i - 1];
		}
		threshold[m_n_parents - 1] = 1.; // Necessary due to rounding errors
	}

	std::vector<std::shared_ptr<GParameterSet>>::iterator it;
	std::bernoulli_distribution amalgamationWanted(amalgamationLikelihood_); // true with a likelihood of m_amalgamation_likelihood
	for (it = GOptimizationAlgorithmT<GParameterSet>::data.begin() + m_n_parents;
		  it != GOptimizationAlgorithmT<GParameterSet>::data.end(); ++it) {
		// Retrieve a random number so we can decide whether to perform cross-over or duplication
		// If we do perform cross-over, we always cross the best individual with another random parent
		if (m_n_parents > 1 && amalgamationWanted(m_gr)) { // Create individuals using a cross-over scheme
			std::shared_ptr <GParameterSet> bestParent = this->front();
			std::shared_ptr <GParameterSet> combiner = (m_n_parents > 2) ? (*(this->begin() + this->m_uniform_int(m_gr, std::uniform_int_distribution<std::size_t>::param_type(1, m_n_parents - 1)))) : (*(this->begin() + 1));

			(*it)->GObject::load(bestParent->amalgamate(combiner));
		} else { // Just perform duplication
			switch (m_recombination_method) {
				case duplicationScheme::DEFAULTDUPLICATIONSCHEME: // we want the RANDOMDUPLICATIONSCHEME behavior
				case duplicationScheme::RANDOMDUPLICATIONSCHEME: {
					randomRecombine(*it);
				}
					break;

				case duplicationScheme::VALUEDUPLICATIONSCHEME: {
					if (m_n_parents == 1) {
						(*it)->GObject::load(*(GOptimizationAlgorithmT<GParameterSet>::data.begin()));
						(*it)->GOptimizableEntity::getPersonalityTraits<GBaseParChildPersonalityTraits> ()->setParentId(0);
					} else {
						// A recombination taking into account the value does not make
						// sense in the first iteration, as parents might not have a suitable
						// value. Instead, this function might accidentaly trigger value
						// calculation. Hence we fall back to random recombination in iteration 0.
						// No value calculation takes place there.
						if (GOptimizationAlgorithmT<GParameterSet>::inFirstIteration()) {
							randomRecombine(*it);
						} else {
							valueRecombine(*it, threshold);
						}
					}
				}
					break;
			}
		}
	}
}

/***************************************************************************/
/**
 * Gives individuals an opportunity to update their internal structures. Here
 * we just trigger an update of the adaptors. We only do so for parents, as
 * they will be replicated in the next iteration. We leave the best parent
 * untouched, so that otherwise successful adaptor settings may survive.
 */
void GParameterSetParChild::actOnStalls() {
	// Make sure the actions of our parent class are carried out
	GBaseParChildT<GParameterSet>::actOnStalls();

	if (this->getNParents() > 1) {
		// Update parent individuals. We leave the best parent untouched
		GParameterSetParChild::iterator it;
		for (it = this->begin() + 1; it != this->begin() + this->getNParents(); ++it) {
			(*it)->updateAdaptorsOnStall(this->getStallCounter());
		}
	}
}

/******************************************************************************/
/**
 * The function checks that the population size meets the requirements and does some
 * tagging. It is called from within GOptimizationAlgorithmT<Gem::Geneva::GParameterSet>::optimize(), before the
 * actual optimization cycle starts.
 */
void GParameterSetParChild::init() {
	// To be performed before any other action
	GBaseParChildT<GParameterSet>::init();
}

/******************************************************************************/
/**
 * Does any necessary finalization work
 */
void GParameterSetParChild::finalize() {
	// Last action
	GBaseParChildT<GParameterSet>::finalize();
}

/******************************************************************************/
/**
 * Loads the data of another GParameterSetParChild object, camouflaged as a GObject.
 *
 * @param cp A pointer to another GParameterSetParChild object, camouflaged as a GObject
 */
void GParameterSetParChild::load_(const GObject *cp) {
	// Check that we are dealing with a GParameterSetParChild reference independent of this object and convert the pointer
	const GParameterSetParChild *p_load = Gem::Common::g_convert_and_compare<GObject, GParameterSetParChild>(cp, this);

	// First load the parent class'es data ...
	GBaseParChildT<GParameterSet>::load_(cp);

	// ... and then our own data
	amalgamationLikelihood_ = p_load->amalgamationLikelihood_;
}

/******************************************************************************/
/**
 * Applies modifications to this object. This is needed for testing purposes
 *
 * @return A boolean which indicates whether modifications were made
 */
bool GParameterSetParChild::modify_GUnitTests() {
#ifdef GEM_TESTING
	bool result = false;

	// Call the parent class'es function
	if (GBaseParChildT<GParameterSet>::modify_GUnitTests()) result = true;

	return result;

#else /* GEM_TESTING */  // If this function is called when GEM_TESTING isn't set, throw
   condnotset("GParameterSetParChild::modify_GUnitTests", "GEM_TESTING");
   return false;
#endif /* GEM_TESTING */
}

/******************************************************************************/
/**
 * Performs self tests that are expected to succeed. This is needed for testing purposes
 */
void GParameterSetParChild::specificTestsNoFailureExpected_GUnitTests() {
#ifdef GEM_TESTING
	// Call the parent class'es function
	GBaseParChildT<GParameterSet>::specificTestsNoFailureExpected_GUnitTests();

	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------

#else /* GEM_TESTING */ // If this function is called when GEM_TESTING isn't set, throw
   condnotset("GParameterSetParChild::specificTestsNoFailureExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
}

/******************************************************************************/
/**
 * Performs self tests that are expected to fail. This is needed for testing purposes
 */
void GParameterSetParChild::specificTestsFailuresExpected_GUnitTests() {
#ifdef GEM_TESTING
	// Call the parent class'es function
	GBaseParChildT<GParameterSet>::specificTestsFailuresExpected_GUnitTests();

	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------

#else /* GEM_TESTING */
   condnotset("GParameterSetParChild::specificTestsFailuresExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
}

/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */

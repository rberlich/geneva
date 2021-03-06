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

#include "geneva/G_OptimizationAlgorithm_ParameterScan.hpp"

BOOST_CLASS_EXPORT_IMPLEMENT(Gem::Geneva::bScanPar) // NOLINT
BOOST_CLASS_EXPORT_IMPLEMENT(Gem::Geneva::int32ScanPar) // NOLINT
BOOST_CLASS_EXPORT_IMPLEMENT(Gem::Geneva::dScanPar) // NOLINT
BOOST_CLASS_EXPORT_IMPLEMENT(Gem::Geneva::fScanPar) // NOLINT

BOOST_CLASS_EXPORT_IMPLEMENT(Gem::Geneva::GParameterScan) // NOLINT

namespace Gem {
namespace Geneva {

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * Returns a set of boolean data items
 */
template<>
std::vector<bool> fillWithData<bool>(
	std::size_t nSteps
	, bool lower
	, bool upper
) {
	std::vector<bool> result;
	result.push_back(false);
	result.push_back(true);
	return result;
}

/******************************************************************************/
/**
 * Returns a set of std::int32_t data items
 */
template<>
std::vector<std::int32_t> fillWithData<std::int32_t>(
	std::size_t nSteps // will only be used for random entries
	, std::int32_t lower
	, std::int32_t upper // inclusive
) {
	std::vector<std::int32_t> result;
	for (std::int32_t i = lower; i <= upper; i++) {
		result.push_back(i);
	}
	return result;
}

/******************************************************************************/
/**
 * Returns a set of float data items
 */
template<>
std::vector<float> fillWithData<float>(
	std::size_t nSteps
	, float lower
	, float upper
) {
	std::vector<float> result;

	// We require at least 2 steps, unless we are are in random mode
	if (nSteps < 2) {
		throw gemfony_exception(
			g_error_streamer(DO_LOG,  time_and_place)
				<< "In std::vector<float> fillWithData<float>(): Error!" << std::endl
				<< "Number of requested steps is too low: " << nSteps << std::endl
		);
	}

	for (std::size_t i = 0; i < nSteps; i++) {
		result.push_back(lower + (upper - lower) * float(i) / float(nSteps - 1));
	}

	return result;
}

/******************************************************************************/
/**
 * Returns a set of double data items
 */
template<>
std::vector<double> fillWithData<double>(
	std::size_t nSteps
	, double lower
	, double upper
) {
	std::vector<double> result;

	// We require at least 2 steps, unless we are are in random mode
	if (nSteps < 2) {
		throw gemfony_exception(
			g_error_streamer(DO_LOG,  time_and_place)
				<< "In std::vector<float> fillWithData<double>(): Error!" << std::endl
				<< "Number of requested steps is too low: " << nSteps << std::endl
		);
	}

	for (std::size_t i = 0; i < nSteps; i++) {
		result.push_back(lower + (upper - lower) * double(i) / double(nSteps - 1));
	}

	return result;
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * The default constructor. Only needed for de-serialization.
 */
bScanPar::bScanPar() : baseScanParT<bool>() { /* nothing */ }

/******************************************************************************/
/**
 * Construction from local variables
 */
bScanPar::bScanPar(parPropSpec<bool> pps, bool randomScan)
	: baseScanParT<bool>(pps, randomScan, "b")
{ /* nothing */ }

/******************************************************************************/
/**
 * Cloning of this object
 */
std::shared_ptr <bScanPar> bScanPar::clone() const {
	return std::shared_ptr<bScanPar>(new bScanPar(*this));
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * The default constructor. Only needed for de-serialization.
 */
int32ScanPar::int32ScanPar() : baseScanParT<std::int32_t>() { /* nothing */ }

/******************************************************************************/
/**
 * The standard destructor
 */
int32ScanPar::int32ScanPar(
	parPropSpec<std::int32_t> pps, bool randomScan
)
	: baseScanParT<std::int32_t>(pps, randomScan, "i")
{ /* nothing */ }

/******************************************************************************/
/**
 * Cloning
 */
std::shared_ptr <int32ScanPar> int32ScanPar::clone() const {
	return std::shared_ptr<int32ScanPar>(new int32ScanPar(*this));
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * The default constructor. Only needed for de-serialization.
 */
dScanPar::dScanPar() : baseScanParT<double>() { /* nothing */ }

/******************************************************************************/
/**
 * The standard destructor
 */
dScanPar::dScanPar(
	parPropSpec<double> pps, bool randomScan
)
	: baseScanParT<double>(pps, randomScan, "d")
{ /* nothing */ }

/******************************************************************************/
/**
 * Cloning
 */
std::shared_ptr <dScanPar> dScanPar::clone() const {
	return std::shared_ptr<dScanPar>(new dScanPar(*this));
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * The default constructor. Only needed for de-serialization.
 */
fScanPar::fScanPar() : baseScanParT<float>() { /* nothing */ }

/******************************************************************************/
/**
 * The standard destructor
 */
fScanPar::fScanPar(
	parPropSpec<float> pps, bool randomScan
)
	: baseScanParT<float>(pps, randomScan, "f")
{ /* nothing */ }

/******************************************************************************/
/**
 * Cloning
 */
std::shared_ptr <fScanPar> fScanPar::clone() const {
	return std::shared_ptr<fScanPar>(new fScanPar(*this));
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * A simple output operator for parSet object, mostly meant for debugging
 */
std::ostream &operator<<(std::ostream &os, const parSet &pS) {
	os
		<< "###########################################################" << std::endl
		<< "# New parSet object:" << std::endl;

	// Boolean data
	if (not pS.bParVec.empty()) {
		os << "# Boolean data" << std::endl;
		std::vector<singleBPar>::const_iterator cit;
		for (cit = pS.bParVec.begin(); cit != pS.bParVec.end(); ++cit) {
			os << (std::get<1>(*cit) ? "true" : "false") << ":" << std::get<0>(*cit);
			if (cit + 1 != pS.bParVec.end()) {
				os << ", ";
			}
		}
		os << std::endl;
	}

	// std::int32_t data
	if (not pS.iParVec.empty()) {
		os << "# std::int32_t data" << std::endl;
		std::vector<singleInt32Par>::const_iterator cit;
		for (cit = pS.iParVec.begin(); cit != pS.iParVec.end(); ++cit) {
			os << std::get<1>(*cit) << ":" << std::get<0>(*cit);
			if (cit + 1 != pS.iParVec.end()) {
				os << ", ";
			}
		}
		os << std::endl;
	}

	// float data
	if (not pS.fParVec.empty()) {
		os << "# float data" << std::endl;
		std::vector<singleFPar>::const_iterator cit;
		for (cit = pS.fParVec.begin(); cit != pS.fParVec.end(); ++cit) {
			os << std::get<1>(*cit) << ":" << std::get<0>(*cit);
			if (cit + 1 != pS.fParVec.end()) {
				os << ", ";
			}
		}
		os << std::endl;
	}

	// double data
	if (not pS.dParVec.empty()) {
		os << "# double data" << std::endl;
		std::vector<singleDPar>::const_iterator cit;
		for (cit = pS.dParVec.begin(); cit != pS.dParVec.end(); ++cit) {
			os << std::get<1>(*cit) << ":" << std::get<0>(*cit);
			if (cit + 1 != pS.dParVec.end()) {
				os << ", ";
			}
		}
		os << std::endl;
	}

	return os;
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * A standard copy constructor.
 *
 * @param cp A copy of another GradientDescent object
 */
GParameterScan::GParameterScan(const GParameterScan &cp)
	: G_OptimizationAlgorithm_Base(cp)
	  , m_cycleLogicHalt(cp.m_cycleLogicHalt)
	  , m_scanRandomly(cp.m_scanRandomly)
	  , m_nMonitorInds(cp.m_nMonitorInds)
	  , m_simpleScanItems(cp.m_simpleScanItems)
	  , m_scansPerformed(cp.m_scansPerformed)
{
	// Copying / setting of the optimization algorithm id is done by the parent class. The same
	// applies to the copying of the optimization monitor.

	// Load the parameter objects
	std::vector<std::shared_ptr < bScanPar>> ::const_iterator b_it;
	for (b_it = cp.m_b_cnt.begin(); b_it != cp.m_b_cnt.end(); ++b_it) {
		m_b_cnt.push_back((*b_it)->clone());
	}

	std::vector<std::shared_ptr < int32ScanPar>> ::const_iterator i_it;
	for (i_it = cp.m_int32_cnt.begin(); i_it != cp.m_int32_cnt.end(); ++i_it) {
		m_int32_cnt.push_back((*i_it)->clone());
	}

	std::vector<std::shared_ptr < dScanPar>> ::const_iterator d_it;
	for (d_it = cp.m_d_cnt.begin(); d_it != cp.m_d_cnt.end(); ++d_it) {
		m_d_cnt.push_back((*d_it)->clone());
	}

	std::vector<std::shared_ptr < fScanPar>> ::const_iterator f_it;
	for (f_it = cp.m_f_cnt.begin(); f_it != cp.m_f_cnt.end(); ++f_it) {
		m_f_cnt.push_back((*f_it)->clone());
	}
}

/******************************************************************************/
/**
 * Returns information about the type of optimization algorithm. This function needs
 * to be overloaded by the actual algorithms to return the correct type.
 *
 * @return The type of optimization algorithm
 */
std::string GParameterScan::getAlgorithmPersonalityType_() const {
	return "PERSONALITY_PS";
}

/******************************************************************************/
/**
 * Returns the name of this optimization algorithm
 *
 * @return The name assigned to this optimization algorithm
 */
std::string GParameterScan::getAlgorithmName_() const {
	return std::string("Parameter Scan");
}

/******************************************************************************/
/**
 * Retrieve the number of processable items in the current iteration.
 *
 * @return The number of processable items in the current iteration
 */
std::size_t GParameterScan::getNProcessableItems_() const {
	return this->size(); // Evaluation always needs to be done for the entire population
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
void GParameterScan::compare_(
	const GObject &cp, const Gem::Common::expectation &e, const double &limit
) const {
	using namespace Gem::Common;

	// Check that we are dealing with a GParameterScan reference independent of this object and convert the pointer
	const GParameterScan *p_load = Gem::Common::g_convert_and_compare<GObject, GParameterScan>(cp, this);

	GToken token("GParameterScan", e);

	// Compare our parent data ...
	Gem::Common::compare_base_t<G_OptimizationAlgorithm_Base>(*this, *p_load, token);

	// ... and then the local data
	compare_t(IDENTITY(m_cycleLogicHalt,  p_load->m_cycleLogicHalt),  token);
	compare_t(IDENTITY(m_scanRandomly,    p_load->m_scanRandomly),    token);
	compare_t(IDENTITY(m_nMonitorInds,    p_load->m_nMonitorInds),    token);
	compare_t(IDENTITY(m_simpleScanItems, p_load->m_simpleScanItems), token);
	compare_t(IDENTITY(m_scansPerformed,  p_load->m_scansPerformed),  token);

	// React on deviations from the expectation
	token.evaluate();
}

/******************************************************************************/
/**
 * Resets the settings of this population to what was configured when
 * the optimize()-call was issued
 */
void GParameterScan::resetToOptimizationStart_() {
	// Reset m_b_cnt, m_int32_cnt, m_d_cnt and m_f_cnt
	this->resetParameterObjects();

	// Reset the custom halt criterion
	m_cycleLogicHalt = false;

	// No scans have been peformed so far
	m_scansPerformed = 0;

	// Make sure we start with a fresh central vector of parameter objects
	this->clearAllParVec();

	// There is no more work to be done here, so we simply call the
	// function of the parent class
	G_OptimizationAlgorithm_Base::resetToOptimizationStart_();
}

/******************************************************************************/
/**
 * Emits a name for this class / object
 */
std::string GParameterScan::name_() const {
	return std::string("GParameterScan");
}

/******************************************************************************/
/**
 * Allows to set the number of "best" individuals to be monitored
 * over the course of the algorithm run
 */
void GParameterScan::setNMonitorInds(std::size_t nMonitorInds) {
	m_nMonitorInds = nMonitorInds;
}

/******************************************************************************/
/**
 * Allows to retrieve  the number of "best" individuals to be monitored
 * over the course of the algorithm run
 */
std::size_t GParameterScan::getNMonitorInds() const {
	return m_nMonitorInds;
}

/******************************************************************************/
/**
 * Loads the data of another population
 *
 * @param cp A pointer to another GParameterScan object, camouflaged as a GObject
 */
void GParameterScan::load_(const GObject *cp) {
	// Check that we are dealing with a GParameterScan reference independent of this object and convert the pointer
	const GParameterScan *p_load = Gem::Common::g_convert_and_compare<GObject, GParameterScan>(cp, this);

	// First load the parent class'es data.
	// This will also take care of copying all individuals.
	G_OptimizationAlgorithm_Base::load_(cp);

	// ... and then our own data
	m_cycleLogicHalt = p_load->m_cycleLogicHalt;
	m_scanRandomly = p_load->m_scanRandomly;
	m_nMonitorInds = p_load->m_nMonitorInds;
	m_simpleScanItems = p_load->m_simpleScanItems;
	m_scansPerformed = p_load->m_scansPerformed;

	// Load the parameter objects
	m_b_cnt.clear();
	std::vector<std::shared_ptr < bScanPar>> ::const_iterator b_it;
	for (b_it = (p_load->m_b_cnt).begin(); b_it != (p_load->m_b_cnt).end(); ++b_it) {
		m_b_cnt.push_back((*b_it)->clone());
	}

	m_int32_cnt.clear();
	std::vector<std::shared_ptr < int32ScanPar>> ::const_iterator i_it;
	for (i_it = (p_load->m_int32_cnt).begin(); i_it != (p_load->m_int32_cnt).end(); ++i_it) {
		m_int32_cnt.push_back((*i_it)->clone());
	}

	m_d_cnt.clear();
	std::vector<std::shared_ptr < dScanPar>> ::const_iterator d_it;
	for (d_it = (p_load->m_d_cnt).begin(); d_it != (p_load->m_d_cnt).end(); ++d_it) {
		m_d_cnt.push_back((*d_it)->clone());
	}

	m_f_cnt.clear();
	std::vector<std::shared_ptr < fScanPar>> ::const_iterator f_it;
	for (f_it = (p_load->m_f_cnt).begin(); f_it != (p_load->m_f_cnt).end(); ++f_it) {
		m_f_cnt.push_back((*f_it)->clone());
	}
}

/******************************************************************************/
/**
 * Creates a deep clone of this object
 *
 * @return A deep copy of this object, camouflaged as a GObject
 */
GObject *GParameterScan::clone_() const {
	return new GParameterScan(*this);
}

/******************************************************************************/
/**
 * The actual business logic to be performed during each iteration. Returns the best achieved fitness
 *
 * @return The value of the best individual found
 */
std::tuple<double, double> GParameterScan::cycleLogic_() {
	std::tuple<double, double> bestFitness = std::make_tuple(this->at(0)->getWorstCase(), this->at(0)->getWorstCase());

	// Apply all necessary modifications to individuals
	if (0 == m_simpleScanItems) { // We have been asked to deal with specific parameters
		updateSelectedParameters();
	} else { // We have been asked to randomly initialize the individuals a given number of times
		randomShuffle();
	}

	// Trigger value calculation for all individuals
	// This function is purely virtual and needs to be
	// re-implemented in derived classes
	runFitnessCalculation_();

	// Retrieve information about the best fitness found and disallow re-evaluation
	GParameterScan::iterator it;
	std::tuple<double, double> newEval = std::make_tuple(0., 0.);
	auto m = this->at(0)->getMaxMode(); // We assume that the maxMode is the same for all individuals
	for (it = this->begin(); it != this->end(); ++it) {
#ifdef DEBUG
		if(not (*it)->is_processed()) {
			throw gemfony_exception(
				g_error_streamer(DO_LOG,  time_and_place)
					<< "In GParameterScan::cycleLogic(): Error!" << std::endl
					<< "Individual in position " << (it-this->begin()) << " is not processed" << std::endl
			);
		}
#endif

		newEval = (*it)->getFitnessTuple();
		if(isBetter(std::get<G_TRANSFORMED_FITNESS>(newEval), std::get<G_TRANSFORMED_FITNESS>(bestFitness), m)) {
			bestFitness = newEval;
		}
	}

	// Let the audience know
	return bestFitness;
}

/******************************************************************************/
/**
 * Adds new values to the population's individuals. Note that this function
 * may resize the population and set the default population size, if there
 * is no sufficient number of data sets to be evaluated left.
 */
void GParameterScan::updateSelectedParameters() {
	std::size_t indPos = 0;

	while (true) {
		//------------------------------------------------------------------------
		// Retrieve a work item
		std::size_t mode = 0;
		std::shared_ptr <parSet> pS = getParameterSet(mode);

		switch (mode) {
			//---------------------------------------------------------------------
			case 0: // Parameters are referenced by index
			{
				std::vector<bool> bData;
				std::vector<std::int32_t> iData;
				std::vector<float> fData;
				std::vector<double> dData;

				// Fill the parameter set data into the current individual

				// Retrieve the parameter vectors
				this->at(indPos)->streamline<bool>(bData);
				this->at(indPos)->streamline<std::int32_t>(iData);
				this->at(indPos)->streamline<float>(fData);
				this->at(indPos)->streamline<double>(dData);

				// Add the data items from the parSet object to the vectors

				// 1) For boolean data
				std::vector<singleBPar>::iterator b_it;
				for (b_it = pS->bParVec.begin(); b_it != pS->bParVec.end(); ++b_it) {
					this->addDataPoint<bool>(*b_it, bData);
				}

				// 2) For std::int32_t data
				std::vector<singleInt32Par>::iterator i_it;
				for (i_it = pS->iParVec.begin(); i_it != pS->iParVec.end(); ++i_it) {
					this->addDataPoint<std::int32_t>(*i_it, iData);
				}

				// 3) For float values
				std::vector<singleFPar>::iterator f_it;
				for (f_it = pS->fParVec.begin(); f_it != pS->fParVec.end(); ++f_it) {
					this->addDataPoint<float>(*f_it, fData);
				}

				// 4) For double values
				std::vector<singleDPar>::iterator d_it;
				for (d_it = pS->dParVec.begin(); d_it != pS->dParVec.end(); ++d_it) {
					this->addDataPoint<double>(*d_it, dData);
				}

				// Copy the data back into the individual
				this->at(indPos)->assignValueVector<bool>(bData);
				this->at(indPos)->assignValueVector<std::int32_t>(iData);
				this->at(indPos)->assignValueVector<float>(fData);
				this->at(indPos)->assignValueVector<double>(dData);
			}
				break;

				//---------------------------------------------------------------------
				// Mode 1 and 2 are treated alike
			case 1: // Parameters are referenced as var[n]
			case 2: // Parameters are references as var --> equivalent to var[0]
			{
				std::map<std::string, std::vector<bool>> bData;
				std::map<std::string, std::vector<std::int32_t>> iData;
				std::map<std::string, std::vector<float>> fData;
				std::map<std::string, std::vector<double>> dData;

				// Retrieve the parameter maos
				this->at(indPos)->streamline<bool>(bData);
				this->at(indPos)->streamline<std::int32_t>(iData);
				this->at(indPos)->streamline<float>(fData);
				this->at(indPos)->streamline<double>(dData);

				// Add the data items from the parSet object to the maos

				// 1) For boolean data
				std::vector<singleBPar>::iterator b_it;
				for (b_it = pS->bParVec.begin(); b_it != pS->bParVec.end(); ++b_it) {
					this->addDataPoint<bool>(*b_it, bData);
				}

				// 2) For std::int32_t data
				std::vector<singleInt32Par>::iterator i_it;
				for (i_it = pS->iParVec.begin(); i_it != pS->iParVec.end(); ++i_it) {
					this->addDataPoint<std::int32_t>(*i_it, iData);
				}

				// 3) For float values
				std::vector<singleFPar>::iterator f_it;
				for (f_it = pS->fParVec.begin(); f_it != pS->fParVec.end(); ++f_it) {
					this->addDataPoint<float>(*f_it, fData);
				}

				// 4) For double values
				std::vector<singleDPar>::iterator d_it;
				for (d_it = pS->dParVec.begin(); d_it != pS->dParVec.end(); ++d_it) {
					this->addDataPoint<double>(*d_it, dData);
				}


				// Copy the data back into the individual
				this->at(indPos)->assignValueVectors<bool>(bData);
				this->at(indPos)->assignValueVectors<std::int32_t>(iData);
				this->at(indPos)->assignValueVectors<float>(fData);
				this->at(indPos)->assignValueVectors<double>(dData);
			}

				break;

				//---------------------------------------------------------------------
			default: {
				throw gemfony_exception(
					g_error_streamer(DO_LOG,  time_and_place)
						<< "In GParameterScan::updateSelectedParameters(): Error!" << std::endl
						<< "Encountered invalid mode " << mode << std::endl
				);
			}
				break;
		}

		//------------------------------------------------------------------------
		// Mark the individual as "dirty", so it gets re-evaluated the
		// next time the fitness() function is called
		this->at(indPos)->mark_as_due_for_processing();

		// We were successful
		m_cycleLogicHalt = false;

		//------------------------------------------------------------------------
		// Make sure we continue with the next parameter set in the next iteration
		if (not this->switchToNextParameterSet()) {
			// Let the audience know that the optimization may be stopped
			this->m_cycleLogicHalt = true;

			// Reset all parameter objects for the next run (if desired)
			this->resetParameterObjects();

			// Resize the population, so we only have modified individuals
			this->resize(indPos + 1);

			// Terminate the loop
			break;
		}

		//------------------------------------------------------------------------
		// We do not want to exceed the boundaries of the population
		if (++indPos >= this->getDefaultPopulationSize()) break;
	}
}

/******************************************************************************/
/**
 * Randomly initialize the individuals a given number of times
 */
void GParameterScan::randomShuffle() {
	std::size_t indPos = 0;

	while (true) {
		// Update the individual and mark it as "dirty"
		this->at(indPos)->randomInit(activityMode::ACTIVEONLY);
		// Mark the individual as "dirty", so it gets re-evaluated the
		// next time the fitness() function is called
		this->at(indPos)->mark_as_due_for_processing();

		// We were successful
		m_cycleLogicHalt = false;

		//------------------------------------------------------------------------
		// We do not want to exceed the boundaries of the population -- stop
		// if we have reached the end of the population
		if (++indPos >= this->getDefaultPopulationSize()) break;

		//------------------------------------------------------------------------
		// Make sure we terminate when the desired overall number of random scans has
		// been performed
		if (++m_scansPerformed >= m_simpleScanItems) {
			// Let the audience know that the optimization may be stopped
			this->m_cycleLogicHalt = true;

			// Reset all parameter objects for the next run (if desired)
			this->resetParameterObjects();

			// Resize the population, so we only have modified individuals
			this->resize(indPos + 1);

			// Terminate the loop
			break;
		}
	}
}

/******************************************************************************/
/**
 * Resets all parameter objects
 */
void GParameterScan::resetParameterObjects() {
	std::vector<std::shared_ptr < bScanPar>> ::iterator b_it;
	for (b_it = m_b_cnt.begin(); b_it != m_b_cnt.end(); ++b_it) {
		(*b_it)->resetPosition();
	}

	std::vector<std::shared_ptr < int32ScanPar>> ::iterator i_it;
	for (i_it = m_int32_cnt.begin(); i_it != m_int32_cnt.end(); ++i_it) {
		(*i_it)->resetPosition();
	}

	std::vector<std::shared_ptr < fScanPar>> ::iterator f_it;
	for (f_it = m_f_cnt.begin(); f_it != m_f_cnt.end(); ++f_it) {
		(*f_it)->resetPosition();
	}

	std::vector<std::shared_ptr < dScanPar>> ::iterator d_it;
	for (d_it = m_d_cnt.begin(); d_it != m_d_cnt.end(); ++d_it) {
		(*d_it)->resetPosition();
	}

	m_simpleScanItems = std::size_t(0);
}

/******************************************************************************/
/**
 * Retrieves a parameter set by filling the current parameter combinations
 * into a parSet object.
 *
 * @param mode Indicates whether parameters are identified by name or by id
 */
std::shared_ptr <parSet> GParameterScan::getParameterSet(std::size_t &mode) {
	// Create a new parSet object
	std::shared_ptr <parSet> result(new parSet());

	bool modeSet = false;

	// Extract the relevant data and store it in a parSet object
	// 1) For boolean objects
	std::vector<std::shared_ptr < bScanPar>>::iterator b_it;
	for (b_it = m_b_cnt.begin(); b_it != m_b_cnt.end(); ++b_it) {
		NAMEANDIDTYPE var = (*b_it)->getVarAddress();

		if (modeSet) {
			if (std::get<0>(var) != mode) {
				throw gemfony_exception(
					g_error_streamer(DO_LOG,  time_and_place)
						<< "In GParameterScan::getParameterSet(): Error!" << std::endl
						<< "Expected mode " << mode << " but got " << std::get<0>(var) << std::endl
				);
			}
		} else {
			mode = std::get<0>(var);
			modeSet = true;
		}

		singleBPar item((*b_it)->getCurrentItem(m_gr), std::get<0>(var), std::get<1>(var), std::get<2>(var));
		(result->bParVec).push_back(item);
	}
	// 2) For std::int32_t objects
	std::vector<std::shared_ptr < int32ScanPar>>::iterator i_it;
	for (i_it = m_int32_cnt.begin(); i_it != m_int32_cnt.end(); ++i_it) {
		NAMEANDIDTYPE var = (*i_it)->getVarAddress();

		if (modeSet) {
			if (std::get<0>(var) != mode) {
				throw gemfony_exception(
					g_error_streamer(DO_LOG,  time_and_place)
						<< "In GParameterScan::getParameterSet(): Error!" << std::endl
						<< "Expected mode " << mode << " but got " << std::get<0>(var) << std::endl
				);
			}
		} else {
			mode = std::get<0>(var);
			modeSet = true;
		}

		singleInt32Par item((*i_it)->getCurrentItem(m_gr), std::get<0>(var), std::get<1>(var), std::get<2>(var));
		(result->iParVec).push_back(item);
	}
	// 3) For float objects
	std::vector<std::shared_ptr < fScanPar>>::iterator f_it;
	for (f_it = m_f_cnt.begin(); f_it != m_f_cnt.end(); ++f_it) {
		NAMEANDIDTYPE var = (*f_it)->getVarAddress();

		if (modeSet) {
			if (std::get<0>(var) != mode) {
				throw gemfony_exception(
					g_error_streamer(DO_LOG,  time_and_place)
						<< "In GParameterScan::getParameterSet(): Error!" << std::endl
						<< "Expected mode " << mode << " but got " << std::get<0>(var) << std::endl
				);
			}
		} else {
			mode = std::get<0>(var);
			modeSet = true;
		}

		singleFPar item((*f_it)->getCurrentItem(m_gr), std::get<0>(var), std::get<1>(var), std::get<2>(var));
		(result->fParVec).push_back(item);
	}
	// 4) For double objects
	std::vector<std::shared_ptr < dScanPar>>::iterator d_it;
	for (d_it = m_d_cnt.begin(); d_it != m_d_cnt.end(); ++d_it) {
		NAMEANDIDTYPE var = (*d_it)->getVarAddress();

		if (modeSet) {
			if (std::get<0>(var) != mode) {
				throw gemfony_exception(
					g_error_streamer(DO_LOG,  time_and_place)
						<< "In GParameterScan::getParameterSet(): Error!" << std::endl
						<< "Expected mode " << mode << " but got " << std::get<0>(var) << std::endl
				);
			}
		} else {
			mode = std::get<0>(var);
			modeSet = true;
		}

		singleDPar item((*d_it)->getCurrentItem(m_gr), std::get<0>(var), std::get<1>(var), std::get<2>(var));
		(result->dParVec).push_back(item);
	}

	return result;
}

/******************************************************************************/
/**
 * Switches to the next parameter set
 *
 * @return A boolean indicating whether there indeed is a following
 * parameter set (true) or whether we have reached the end of the
 * collection (false)
 */
bool GParameterScan::switchToNextParameterSet() {
	auto it = m_all_par_cnt.begin();

	// Switch to the next parameter set
	while (true) {
		if ((*it)->goToNextItem()) { // Will trigger if a warp has occurred
			if (it + 1 == m_all_par_cnt.end()) return false; // All possible combinations were found
			else ++it; // Try the next parameter object
		} else {
			return true; // We have successfully switched to the next parameter set
		}
	}

	// Make the compiler happy
	return false;
}

/******************************************************************************/
/**
 * Fills all parameter objects into the allParVec_ vector
 */
void GParameterScan::fillAllParVec() {
	// 1) For boolean objects
	for(const auto& item_ptr: m_b_cnt)     { m_all_par_cnt.push_back(item_ptr); }
	// 2) For std::int32_t objects
	for(const auto& item_ptr: m_int32_cnt) { m_all_par_cnt.push_back(item_ptr); }
	// 3) For float objects
	for(const auto& item_ptr: m_f_cnt)     { m_all_par_cnt.push_back(item_ptr); }
	// 4) For double objects
	for(const auto& item_ptr: m_d_cnt)     { m_all_par_cnt.push_back(item_ptr); }
}

/******************************************************************************/
/**
 * Clears the allParVec_ vector
 */
void GParameterScan::clearAllParVec() {
	m_all_par_cnt.clear();
}

/******************************************************************************/
/**
 * A custom halt criterion for the optimization, allowing to stop the loop
 * when no items are left to be scanned
 */
bool GParameterScan::customHalt_() const {
	if (this->m_cycleLogicHalt) {
		glogger
			<< "Terminating the loop as no items are left to be" << std::endl
			<< "processed in parameter scan." << std::endl
			<< GLOGGING;
		return true;
	} else {
		return false;
	}
}

/******************************************************************************/
/**
 * Adds local configuration options to a GParserBuilder object
 *
 * @param gpb The GParserBuilder object to which configuration options should be added
 */
void GParameterScan::addConfigurationOptions_(
	Gem::Common::GParserBuilder &gpb
) {
	// Call our parent class'es function
	G_OptimizationAlgorithm_Base::addConfigurationOptions_(gpb);

	gpb.registerFileParameter<std::size_t>(
		"size" // The name of the first variable
		, DEFAULTPOPULATIONSIZE
		, [this](std::size_t dps) { this->setDefaultPopulationSize(dps); }
	)
		<< "The total size of the population";

	gpb.registerFileParameter<std::string>(
		"parameterOptions"
		, std::string("d(0, -10., 10., 100), d(1, -10., 10., 100)")
		, [this](std::string parSpecs) { this->setParameterSpecs(parSpecs); }
	)
		<< "Specification of the parameters to be used in the parameter scan" << std::endl;

	gpb.registerFileParameter<bool>(
		"scanRandomly" // The name of the variable
		, true // The default value
		, [this](bool sr) { this->setScanRandomly(sr); }
	)
		<< "Indicates whether scans of individual variables should be done randomly" << std::endl
		<< "(1) or on a grid (0)";

	// Override the default value of maxStallIteration, as the parent
	// default does not make sense for us (we do not need stall iterations)
	gpb.resetFileParameterDefaults(
		"maxStallIteration", DEFAULTMAXPARSCANSTALLIT
	);
}

/******************************************************************************/
/**
 * Triggers fitness calculation of a number of individuals. This function performs the same task as done
 * in GParameterScan, albeit by delegating work to the broker. Items are evaluated up to a maximum position
 * in the vector. Note that we always start the evaluation with the first item in the vector.
 *
 * @param finalPos The position in the vector up to which the fitness calculation should be performed
 * @return The best fitness found amongst all parents
 */
void GParameterScan::runFitnessCalculation_() {
	using namespace Gem::Courtier;

#ifdef DEBUG
	GParameterScan::iterator it;
	for(it=this->begin(); it!=this->end(); ++it) {
		// Make sure the evaluated individuals have the dirty flag set
		if(not (*it)->is_due_for_processing()) {
			throw gemfony_exception(
				g_error_streamer(DO_LOG,  time_and_place)
					<< "In GParameterScan::runFitnessCalculation():" << std::endl
					<< "Found individual in position " << std::distance(this->begin(), it)
					<< ", which has not been marked as due for processing" << std::endl
			);
		}
	}
#endif /* DEBUG */

	//--------------------------------------------------------------------------------
	// Submit all work items and wait for their return

	setProcessingFlag(this->m_data_cnt, std::make_tuple(std::size_t(0), this->m_data_cnt.size()));
	auto status = this->workOn(
		m_data_cnt
		, true // resubmit unprocessed items
		, "GParameterScan::runFitnessCalculation()"
	);

	//--------------------------------------------------------------------------------
	// Some error checks

	// Check if all work items have returned or whether there were errors. Both cannot
	// be accepted in a parameter scan.
	if (not status.is_complete || status.has_errors) {
		throw gemfony_exception(
			g_error_streamer(DO_LOG,  time_and_place)
				<< "In GParameterScan::runFitnessCalculation(): Error!" << std::endl
				<< "No complete set of items received or erroneous items found" << std::endl
		);
	}

	//--------------------------------------------------------------------------------
}

/******************************************************************************/
/**
 * Analyzes the parameters to be scanned. Note that this function will clear any
 * existing parameter definitions, as parStr represents a new set of parameters
 * to be scanned.
 */
void GParameterScan::setParameterSpecs(std::string parStr) {
	// Check that the parameter string isn't empty
	if (parStr.empty()) {
		throw gemfony_exception(
			g_error_streamer(DO_LOG,  time_and_place)
				<< "In GParameterScan::addParameterSpecs(): Error!" << std::endl
				<< "Parameter string " << parStr << " is empty" << std::endl
		);
	}

	//---------------------------------------------------------------------------
	// Clear the parameter vectors
	m_d_cnt.clear();
	m_f_cnt.clear();
	m_int32_cnt.clear();
	m_b_cnt.clear();

	// Parse the parameter string
	GParameterPropertyParser ppp(parStr);

	//---------------------------------------------------------------------------
	// Assign the parameter definitions to our internal parameter vectors.
	// We distinguish between a simple scan, where only a number of work items
	// will be initialized randomly repeatedly, and scans of individual variables.
	m_simpleScanItems = ppp.getNSimpleScanItems();
	if (0 == m_simpleScanItems) { // Only act if no "simple scan" was requested
		// Retrieve double parameters
		std::tuple<
			std::vector<parPropSpec<double>>::const_iterator, std::vector<parPropSpec<double>>::const_iterator
		> t_d = ppp.getIterators<double>();

		auto d_cit = std::get<0>(t_d);
		auto d_end = std::get<1>(t_d);
		for (; d_cit != d_end; ++d_cit) { // Note: d_cit is already set to the begin of the double parameter arrays
			m_d_cnt.push_back(std::shared_ptr<dScanPar>(new dScanPar(*d_cit, m_scanRandomly)));
		}

		// Retrieve float parameters
		std::tuple<
			std::vector<parPropSpec<float>>::const_iterator, std::vector<parPropSpec<float>>::const_iterator
		> t_f = ppp.getIterators<float>();

		auto f_cit = std::get<0>(t_f);
		auto f_end = std::get<1>(t_f);
		for (; f_cit != f_end; ++f_cit) { // Note: f_cit is already set to the begin of the double parameter arrays
			m_f_cnt.push_back(std::shared_ptr<fScanPar>(new fScanPar(*f_cit, m_scanRandomly)));
		}

		// Retrieve integer parameters
		std::tuple<
			std::vector<parPropSpec<std::int32_t>>::const_iterator, std::vector<parPropSpec<std::int32_t>>::const_iterator
		> t_i = ppp.getIterators<std::int32_t>();

		auto i_cit = std::get<0>(t_i);
		auto i_end = std::get<1>(t_i);
		for (; i_cit != i_end; ++i_cit) { // Note: i_cit is already set to the begin of the double parameter arrays
			m_int32_cnt.push_back(std::shared_ptr<int32ScanPar>(new int32ScanPar(*i_cit, m_scanRandomly)));
		}

		// Retrieve boolean parameters
		std::tuple<
			std::vector<parPropSpec<bool>>::const_iterator, std::vector<parPropSpec<bool>>::const_iterator
		> t_b = ppp.getIterators<bool>();

		auto b_cit = std::get<0>(t_b);
		auto b_end = std::get<1>(t_b);
		for (; b_cit != b_end; ++b_cit) { // Note: b_cit is already set to the begin of the double parameter arrays
			m_b_cnt.push_back(std::shared_ptr<bScanPar>(new bScanPar(*b_cit, m_scanRandomly)));
		}
	}

	//---------------------------------------------------------------------------
}

/******************************************************************************/
/**
 * Specified the number of simple scans an puts the class in "simple scan" mode
 */
void GParameterScan::setNSimpleScans(std::size_t simpleScanItems) {
	m_simpleScanItems = simpleScanItems;
}

/******************************************************************************/
/**
 * Retrieves the number of simple scans (or 0, if disabled)
 */
std::size_t GParameterScan::getNSimpleScans() const {
	return m_simpleScanItems;
}

/******************************************************************************/
/**
 * Retrieves the number of simple scans performed so far
 */
std::size_t GParameterScan::getNScansPerformed() const {
	return m_scansPerformed;
}

/******************************************************************************/
/**
 * Allows to specify whether the parameter space should be scanned randomly
 * or on a grid
 */
void GParameterScan::setScanRandomly(bool scanRandomly) {
	m_scanRandomly = scanRandomly;
}

/******************************************************************************/
/**
 * Allows to check whether the parameter space should be scanned randomly
 * or on a grid
 */
bool GParameterScan::getScanRandomly() const {
	return m_scanRandomly;
}

/******************************************************************************/
/**
 * Does some preparatory work before the optimization starts
 */
void GParameterScan::init() {
	// To be performed before any other action
	G_OptimizationAlgorithm_Base::init();

	// Reset the custom halt criterion
	m_cycleLogicHalt = false;

	// No scans have been peformed so far
	m_scansPerformed = 0;

	// Make sure we start with a fresh central vector of parameter objects
	this->clearAllParVec();

	// Copy all parameter objects to the central vector for easier handling
	this->fillAllParVec();
}

/******************************************************************************/
/**
 * Does any necessary finalization work
 */
void GParameterScan::finalize() {
	// Last action
	G_OptimizationAlgorithm_Base::finalize();
}

/******************************************************************************/
/**
 * Retrieve a GPersonalityTraits object belonging to this algorithm
 */
std::shared_ptr <GPersonalityTraits> GParameterScan::getPersonalityTraits_() const {
	return std::shared_ptr<GParameterScan_PersonalityTraits>(new GParameterScan_PersonalityTraits());
}

/******************************************************************************/
/**
 * Gives individuals an opportunity to update their internal structures. This
 * function has no deeper meaning for a parameter scan and is hence empty.
 */
void GParameterScan::actOnStalls_() {
	/* nothing */
}

/******************************************************************************/
/**
 * Resizes the population to the desired level and does some error checks.
 */
void GParameterScan::adjustPopulation_() {
	// Check how many individuals we already have
	std::size_t nStart = this->size();

	// Do some error checking ...

	// An empty population is an error
	if (nStart == 0) {
		throw gemfony_exception(
			g_error_streamer(DO_LOG,  time_and_place)
				<< "In GParameterScan::adjustPopulation(): Error!" << std::endl
				<< "You didn't add any individuals to the collection. We need at least one." << std::endl
		);
	}

	// We want exactly one individual in the beginning. All other registered
	// individuals will be discarded.
	if (nStart > 1) {
		this->resize(1);
		nStart = 1;
	}

	// Check that we have a valid default population size
	if (0 == this->getDefaultPopulationSize()) {
		throw gemfony_exception(
			g_error_streamer(DO_LOG,  time_and_place)
				<< "In GParameterScan::adjustPopulation(): Error!" << std::endl
				<< "Default-size of the population is 0" << std::endl
		);
	}

	// Create the desired number of (identical) individuals in the population.
	for (std::size_t ind = 1; ind < this->getDefaultPopulationSize(); ind++) {
		this->push_back(this->at(0)->GObject::clone<GParameterSet>());
	}
}

/******************************************************************************/
/**
 * Applies modifications to this object. This is needed for testing purposes
 */
bool GParameterScan::modify_GUnitTests_() {
#ifdef GEM_TESTING
	bool result = false;

	// Call the parent class'es function
	if (G_OptimizationAlgorithm_Base::modify_GUnitTests_()) result = true;

	return result;
#else /* GEM_TESTING */
	Gem::Common::condnotset("GParameterScan::modify_GUnitTests", "GEM_TESTING");
   return false;
#endif /* GEM_TESTING */
}

/******************************************************************************/
/**
 * Performs self tests that are expected to succeed. This is needed for testing purposes
 */
void GParameterScan::specificTestsNoFailureExpected_GUnitTests_() {
#ifdef GEM_TESTING
	// Call the parent class'es function
	G_OptimizationAlgorithm_Base::specificTestsNoFailureExpected_GUnitTests_();
#else /* GEM_TESTING */ // If this function is called when GEM_TESTING isn't set, throw
	Gem::Common::condnotset("GParameterScan::specificTestsNoFailureExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
}

/******************************************************************************/
/**
 * Performs self tests that are expected to fail. This is needed for testing purposes
 */
void GParameterScan::specificTestsFailuresExpected_GUnitTests_() {
#ifdef GEM_TESTING
	// Call the parent class'es function
	G_OptimizationAlgorithm_Base::specificTestsFailuresExpected_GUnitTests_();
#else /* GEM_TESTING */ // If this function is called when GEM_TESTING isn't set, throw
	Gem::Common::condnotset("GParameterScan::specificTestsNoFailureExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
}

/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */



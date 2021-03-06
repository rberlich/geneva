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

#pragma once

// Global checks, defines and includes needed for all of Geneva
#include "common/GGlobalDefines.hpp"

// Standard headers go here
#include <memory>

// Boost headers go here

// Geneva headers go here
#include "common/GExceptions.hpp"
#include "common/GPlotDesigner.hpp"
#include "geneva/GParameterSet.hpp"
#include "geneva/G_OptimizationAlgorithm_Base.hpp"
#include "geneva/GOptimizationEnums.hpp"
#include "geneva/G_OptimizationAlgorithm_GradientDescent_PersonalityTraits.hpp"
#include "geneva/G_OptimizationAlgorithm_Base.hpp"

#ifdef GEM_TESTING

#include "geneva/GTestIndividual1.hpp"

#endif /* GEM_TESTING */

namespace Gem {
namespace Geneva {

/**
 * The default number of simultaneous starting points for the gradient descent
 */
const std::size_t DEFAULTGDSTARTINGPOINTS = 1;
const double DEFAULTFINITESTEP = 0.001;
const double DEFAULTSTEPSIZE = 0.1;

/******************************************************************************/
/**
 * The GGradientDescent class implements a steepest descent algorithm. It is possible
 * to search for optima starting from several positions simultaneously. This class limits
 * itself to evaluation through the Courtier framework, i.e. all evaluation of individuals
 * is delegated to the Broker (which may in turn use other means, such as threads or
 * networked execution for the evaluation step).
 */
class GGradientDescent
    :
        public G_OptimizationAlgorithm_Base
{
    ///////////////////////////////////////////////////////////////////////
    friend class boost::serialization::access;

    template<typename Archive>
    void serialize(Archive &ar, const unsigned int) {
        using boost::serialization::make_nvp;

        ar
        & make_nvp(
            "G_OptimizationAlgorithm_Base"
            , boost::serialization::base_object<G_OptimizationAlgorithm_Base>(*this))
        & BOOST_SERIALIZATION_NVP(nStartingPoints_)
        & BOOST_SERIALIZATION_NVP(nFPParmsFirst_)
        & BOOST_SERIALIZATION_NVP(finiteStep_)
        & BOOST_SERIALIZATION_NVP(stepSize_);
    }

    ///////////////////////////////////////////////////////////////////////

public:
    /** @brief The default constructor */
    G_API_GENEVA GGradientDescent();
    /** @brief Initialization with the number of starting points and the size of the finite step */
    G_API_GENEVA GGradientDescent(const std::size_t &, const double &, const double &);
    /** @brief A standard copy constructor */
    G_API_GENEVA GGradientDescent(const GGradientDescent &) = default;
    /** @brief The destructor */
    G_API_GENEVA ~GGradientDescent() override = default;

    /** @brief Retrieves the number of starting points of the algorithm */
    G_API_GENEVA std::size_t getNStartingPoints() const;
    /** @brief Allows to set the number of starting points for the gradient descent */
    G_API_GENEVA void setNStartingPoints(std::size_t);

    /** @brief Set the size of the finite step of the adaption process */
    G_API_GENEVA void setFiniteStep(double);
    /** @brief Retrieve the size of the finite step of the adaption process */
    G_API_GENEVA double getFiniteStep() const;

    /** @brief Sets a multiplier for the adaption process */
    G_API_GENEVA void setStepSize(double);
    /** @brief Retrieves the current step size */
    G_API_GENEVA double getStepSize() const;

protected:
    /***************************************************************************/
    // Virtual or overridden protected functions

    /** @brief Adds local configuration options to a GParserBuilder object */
    G_API_GENEVA void addConfigurationOptions_(
        Gem::Common::GParserBuilder &gpb
    ) override;

    /** @brief Loads the data of another population */
    G_API_GENEVA void load_(const GObject *) override;

    /** @brief Allow access to this classes compare_ function */
    friend void Gem::Common::compare_base_t<GGradientDescent>(
        GGradientDescent const &
        , GGradientDescent const &
        , Gem::Common::GToken &
    );

    /** @brief Searches for compliance with expectations with respect to another object of the same type */
    G_API_GENEVA void compare_(
        const GObject & // the other object
        , const Gem::Common::expectation & // the expectation for this object, e.g. equality
        , const double & // the limit for allowed deviations of floating point types
    ) const override;

    /** @brief Resets the settings of this population to what was configured when the optimize()-call was issued */
    G_API_GENEVA void resetToOptimizationStart_() override;

    /** @brief Does some preparatory work before the optimization starts */
    G_API_GENEVA void init() override;
    /** @brief Does any necessary finalization work */
    G_API_GENEVA void finalize() override;

    /** @brief Updates the individual parameters of children */
    virtual G_API_GENEVA void updateChildParameters() BASE;

    /** @brief Performs a step of the parent individuals */
    virtual G_API_GENEVA void updateParentIndividuals() BASE;

    /** @brief Applies modifications to this object. This is needed for testing purposes */
    G_API_GENEVA bool modify_GUnitTests_() override;
    /** @brief Performs self tests that are expected to succeed. This is needed for testing purposes */
    G_API_GENEVA void specificTestsNoFailureExpected_GUnitTests_() override;
    /** @brief Performs self tests that are expected to fail. This is needed for testing purposes */
    G_API_GENEVA void specificTestsFailuresExpected_GUnitTests_() override;

    /***************************************************************************/

private:
    /***************************************************************************/
    // Virtual or overridden private functions

    /** @brief Emits a name for this class / object */
    G_API_GENEVA std::string name_() const override;
    /** @brief Creates a deep clone of this object */
    G_API_GENEVA GObject *clone_() const override;

    /** @brief The actual business logic to be performed during each iteration. Returns the best achieved fitness */
    G_API_GENEVA std::tuple<double, double> cycleLogic_() override;
    /** @brief Triggers fitness calculation of a number of individuals */
    G_API_GENEVA void runFitnessCalculation_() override;

    /** @brief Returns information about the type of optimization algorithm */
    G_API_GENEVA std::string getAlgorithmPersonalityType_() const override;
    /** @brief Returns the name of this optimization algorithm */
    G_API_GENEVA std::string getAlgorithmName_() const override;

    /** @brief Retrieves the number of processable items for the current iteration */
    G_API_GENEVA std::size_t getNProcessableItems_() const override;

    /** @brief Retrieve a GPersonalityTraits object belonging to this algorithm */
    G_API_GENEVA std::shared_ptr<GPersonalityTraits> getPersonalityTraits_() const override;
    /** @brief Resizes the population to the desired level and does some error checks */
    G_API_GENEVA void adjustPopulation_() override;

    /** @brief Gives individuals an opportunity to update their internal structures */
    G_API_GENEVA void actOnStalls_() override;

    /***************************************************************************/
    // Data

    std::size_t nStartingPoints_ = DEFAULTGDSTARTINGPOINTS; ///< The number of starting positions in the parameter space
    std::size_t nFPParmsFirst_ = 0; ///< The amount of floating point values in the first individual

    double finiteStep_ = DEFAULTFINITESTEP; ///< The size of the incremental adaption of the feature vector
    double stepSize_ = DEFAULTSTEPSIZE; ///< A multiplicative factor for the adaption
    long double stepRatio_ = (
        DEFAULTSTEPSIZE / DEFAULTFINITESTEP
    ); ///< The ratio of stepSize_ and finiteStep_. NOTE: long double; Will be recalculated in init()

    std::vector<double> dblLowerParameterBoundaries_
        = std::vector<double>(); ///< Holds lower boundaries of double parameters; Will be extracted in init()
    std::vector<double> dblUpperParameterBoundaries_
        = std::vector<double>(); ///< Holds upper boundaries of double parameters; Will be extracted in init()
    std::vector<double> adjustedFiniteStep_
        = std::vector<double>(); ///< A step-size normalized to each parameter range; Will be recalculated in init()

    /** @brief Lets individuals know about their position in the population */
    void markIndividualPositions();
};

/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */


BOOST_CLASS_EXPORT_KEY(Gem::Geneva::GGradientDescent)


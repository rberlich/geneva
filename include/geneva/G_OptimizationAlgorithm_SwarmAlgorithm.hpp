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

// Boost headers go here

// Geneva headers go here
#include "common/GExceptions.hpp"
#include "common/GPlotDesigner.hpp"
#include "geneva/GParameterSet.hpp"
#include "geneva/G_OptimizationAlgorithm_Base.hpp"
#include "geneva/GOptimizationEnums.hpp"
#include "geneva/G_OptimizationAlgorithm_SwarmAlgorithm_PersonalityTraits.hpp"

#ifdef GEM_TESTING
#include "geneva/GTestIndividual1.hpp"
#endif /* GEM_TESTING */

namespace Gem {
namespace Geneva {


/******************************************************************************/
/**
 * The GSwarmAlgorithm class implements a swarm optimization algorithm, based on the infrastructure
 * provided by the G_OptimizationAlgorithm_Base class. Its population is based on a constant number
 * of neighborhoods, whose amount of members is allowed to vary. This happens so that late
 * arrivals in case of networked execution can still be integrated into later iterations.
 *
 * TODO: Mark checkpoints so the serialization mode can be determined automatically (e.g. using file extension ??)
 */
class GSwarmAlgorithm
	:public G_OptimizationAlgorithm_Base
{
	 ///////////////////////////////////////////////////////////////////////
	 friend class boost::serialization::access;

	 template<typename Archive>
	 void serialize(Archive & ar, const unsigned int) {
		 using boost::serialization::make_nvp;

		 ar
		 & make_nvp("G_OptimizationAlgorithm_Base", boost::serialization::base_object<G_OptimizationAlgorithm_Base>(*this))
		 & BOOST_SERIALIZATION_NVP(m_n_neighborhoods)
		 & BOOST_SERIALIZATION_NVP(m_default_n_neighborhood_members)
		 & BOOST_SERIALIZATION_NVP(m_n_neighborhood_members_cnt)
		 & BOOST_SERIALIZATION_NVP(m_global_best_ptr)
		 & BOOST_SERIALIZATION_NVP(m_neighborhood_bests_cnt)
		 & BOOST_SERIALIZATION_NVP(m_c_personal)
		 & BOOST_SERIALIZATION_NVP(m_c_neighborhood)
		 & BOOST_SERIALIZATION_NVP(m_c_global)
		 & BOOST_SERIALIZATION_NVP(m_c_velocity)
		 & BOOST_SERIALIZATION_NVP(m_update_rule)
		 & BOOST_SERIALIZATION_NVP(m_random_fill_up)
		 & BOOST_SERIALIZATION_NVP(m_repulsion_threshold)
		 & BOOST_SERIALIZATION_NVP(m_dbl_lower_parameter_boundaries_cnt)
		 & BOOST_SERIALIZATION_NVP(m_dbl_upper_parameter_boundaries_cnt)
		 & BOOST_SERIALIZATION_NVP(m_dbl_vel_max_cnt)
		 & BOOST_SERIALIZATION_NVP(m_velocity_range_percentage);
	 }
	 ///////////////////////////////////////////////////////////////////////

public:
	 /** @brief The default constructor */
	 G_API_GENEVA GSwarmAlgorithm();
	 /** @brief Initialization with neighborhood sizes and amount of individuals in each neighborhood */
	 G_API_GENEVA GSwarmAlgorithm(const std::size_t&, const std::size_t&);
	 /** @brief A standard copy constructor */
	 G_API_GENEVA GSwarmAlgorithm(const GSwarmAlgorithm&);
	 /** @brief The destructor */
	 G_API_GENEVA ~GSwarmAlgorithm() override = default;

	 /** @brief Sets the number of neighborhoods and the number of members in them */
	 G_API_GENEVA void setSwarmSizes(std::size_t, std::size_t);

	 /** @brief Allows to set a static multiplier for personal distances */
	 G_API_GENEVA void setCPersonal(double);
	 /** @brief Allows to retrieve the static multiplier for personal distances */
	 G_API_GENEVA double getCPersonal() const;

	 /** @brief Allows to set a static multiplier for neighborhood distances */
	 G_API_GENEVA void setCNeighborhood(double);
	 /** @brief Allows to retrieve the static multiplier for neighborhood distances */
	 G_API_GENEVA double getCNeighborhood() const;

	 /** @brief Allows to set a static multiplier for global distances */
	 G_API_GENEVA void setCGlobal(double);
	 /** @brief Allows to retrieve the static multiplier for global distances */
	 G_API_GENEVA double getCGlobal() const;

	 /** @brief Allows to set a static multiplier for velocities */
	 G_API_GENEVA void setCVelocity(double);
	 /** @brief Allows to retrieve the static multiplier for velocities */
	 G_API_GENEVA double getCVelocity() const;

	 /** @brief Allows to set the velocity range percentage */
	 G_API_GENEVA void setVelocityRangePercentage(double);
	 /** @brief Allows to retrieve the velocity range percentage */
	 G_API_GENEVA double getVelocityRangePercentage() const;

	 /** @brief Retrieves the number of neighborhoods */
	 G_API_GENEVA std::size_t getNNeighborhoods() const;
	 /** @brief Retrieves the default number of individuals in each neighborhood */
	 G_API_GENEVA std::size_t getDefaultNNeighborhoodMembers() const;
	 /** @brief Retrieves the current number of individuals in a given neighborhood */
	 G_API_GENEVA std::size_t getCurrentNNeighborhoodMembers(const std::size_t&) const;

	 /** @brief Allows to specify the update rule to be used by the swarm */
	 G_API_GENEVA void setUpdateRule(updateRule);
	 /** @brief Allows to retrieve the update rule currently used by the swarm */
	 G_API_GENEVA updateRule getUpdateRule() const;

	 /** @brief Allows to specify the number of stalls as of which the algorithm switches to repulsive mode */
	 G_API_GENEVA void setRepulsionThreshold(std::uint32_t);
	 /** @brief Allows to retrieve the number of stalls as of which the algorithm switches to repulsive mode */
	 G_API_GENEVA std::uint32_t getRepulsionThreshold() const;

	 /** @brief All individuals automatically added to a neighborhood will have equal value */
	 G_API_GENEVA void setNeighborhoodsEqualFillUp();
	 /** @brief All individuals automatically added to a neighborhood will have a random value */
	 G_API_GENEVA void setNeighborhoodsRandomFillUp(bool = true);
	 /** @brief Allows to check whether neighborhoods are filled up with random individuals */
	 G_API_GENEVA bool neighborhoodsFilledUpRandomly() const;

	 /***************************************************************************/
	 /**
	  * Retrieves the best individual of a neighborhood and casts it to the desired type. Note that this
	  * function will only be accessible to the compiler if parameterset_type is a derivative of GParameterSet,
	  * thanks to the magic of std::enable_if and type_traits
	  *
	  * @param neighborhood The neighborhood, whose best individual should be returned
	  * @return A converted shared_ptr to the best individual of a given neighborhood
	  */
	 template <typename parameterset_type>
	 std::shared_ptr<parameterset_type> getBestNeighborhoodIndividual(
		 std::size_t neighborhood
		 , typename std::enable_if<std::is_base_of<GParameterSet, parameterset_type>::value>::type *dummy = nullptr
	 ){
#ifdef DEBUG
		 // Check that the neighborhood is in a valid range
		 if(neighborhood >= m_n_neighborhoods) {
			 throw gemfony_exception(
				 g_error_streamer(DO_LOG, time_and_place)
					 << "In GSwarmAlgorithm::getBestNeighborhoodIndividual<>() : Error" << std::endl
					 << "Requested neighborhood which does not exist: " << neighborhood << " / " << m_n_neighborhoods << std::endl
			 );
		 }
#endif /* DEBUG */

		 // Does error checks on the conversion internally
		 return Gem::Common::convertSmartPointer<GParameterSet, parameterset_type>(m_neighborhood_bests_cnt[neighborhood]);
	 }

protected:
	/***************************************************************************/
	// Virtual or overridden protected functions

	 /** @brief Adds local configuration options to a GParserBuilder object */
	 G_API_GENEVA void addConfigurationOptions_ (
		 Gem::Common::GParserBuilder& gpb
	 ) override;
	 /** @brief Loads the data of another population */
	 G_API_GENEVA void load_(const GObject *) override;

	/** @brief Allow access to this classes compare_ function */
	friend void Gem::Common::compare_base_t<GSwarmAlgorithm>(
		GSwarmAlgorithm const &
		, GSwarmAlgorithm const &
		, Gem::Common::GToken &
	);

	/** @brief Searches for compliance with expectations with respect to another object of the same type */
	G_API_GENEVA void compare_(
		const GObject& // the other object
		, const Gem::Common::expectation& // the expectation for this object, e.g. equality
		, const double& // the limit for allowed deviations of floating point types
	) const override;

	 /** @brief Resets the settings of this population to what was configured when the optimize()-call was issued */
	 G_API_GENEVA void resetToOptimizationStart_() override;

	 /** @brief Does some preparatory work before the optimization starts */
	 G_API_GENEVA void init() override;
	 /** @brief Does any necessary finalization work */
	 G_API_GENEVA void finalize() override;

	 /** @brief Updates the best individuals found */
	 virtual G_API_GENEVA std::tuple<double, double> findBests() BASE;
	 /** @brief Triggers an update of all individual's positions */
	 virtual G_API_GENEVA void updatePositions() BASE;

	 /** @brief Fixes an incomplete population */
	 virtual G_API_GENEVA void adjustNeighborhoods() BASE;

	 /** @brief Applies modifications to this object. This is needed for testing purposes */
	 G_API_GENEVA bool modify_GUnitTests_() override;
	 /** @brief Performs self tests that are expected to succeed. This is needed for testing purposes */
	 G_API_GENEVA void specificTestsNoFailureExpected_GUnitTests_() override;
	 /** @brief Performs self tests that are expected to fail. This is needed for testing purposes */
	 G_API_GENEVA void specificTestsFailuresExpected_GUnitTests_() override;

	 /***************************************************************************/

	 /** @brief Checks whether each neighborhood has the default size */
	 G_API_GENEVA bool neighborhoodsHaveNominalValues() const;

	 /** @brief Helper function that returns the id of the first individual of a neighborhood */
	 G_API_GENEVA std::size_t getFirstNIPos(const std::size_t&) const;
	 /** @brief Helper function that returns the id of the first individual of a neighborhood, using a vector of neighborhood sizes */
	 G_API_GENEVA std::size_t getFirstNIPosVec(const std::size_t&, const std::vector<std::size_t>&) const;
	 /** @brief Helper function that returns the id of the last individual of a neighborhood */
	 G_API_GENEVA std::size_t getLastNIPos(const std::size_t&) const;

	 /** @brief Triggers an update of an individual's positions */
	 G_API_GENEVA void updateIndividualPositions(
		 const std::size_t&
		 , std::shared_ptr<GParameterSet>
		 , std::shared_ptr<GParameterSet>
		 , std::shared_ptr<GParameterSet>
		 , std::shared_ptr<GParameterSet>
		 , std::tuple<double, double, double, double>
	 );

	 /** @brief Adjusts the velocity vector so that its values don't exceed the allowed value range */
	 G_API_GENEVA void pruneVelocity(std::vector<double>&);

	 /** Updates the personal best of an individual */
	 G_API_GENEVA void updatePersonalBest(std::shared_ptr<GParameterSet>);
	 /** Updates the personal best of an individual, if a better solution was found */
	 G_API_GENEVA void updatePersonalBestIfBetter(std::shared_ptr<GParameterSet>);

	 std::size_t m_n_neighborhoods = (DEFAULTNNEIGHBORHOODS ? DEFAULTNNEIGHBORHOODS : 1); ///< The number of neighborhoods in the population
	 std::size_t m_default_n_neighborhood_members = ((DEFAULTNNEIGHBORHOODMEMBERS <= 1) ? 2 : DEFAULTNNEIGHBORHOODMEMBERS); ///< The desired number of individuals belonging to each neighborhood
	 std::vector<std::size_t> m_n_neighborhood_members_cnt = std::vector<std::size_t>(m_n_neighborhoods, 0); ///< The current number of individuals belonging to each neighborhood

	 std::shared_ptr<GParameterSet> m_global_best_ptr; ///< The globally best individual

	 std::vector<std::shared_ptr<GParameterSet>> m_neighborhood_bests_cnt = std::vector<std::shared_ptr<GParameterSet>>(m_n_neighborhoods); ///< The collection of best individuals from each neighborhood
	 std::vector<std::shared_ptr<GParameterSet>> m_velocities_cnt = std::vector<std::shared_ptr<GParameterSet>>(); ///< Holds velocities, as calculated in the previous iteration

	 double m_c_personal = DEFAULTCPERSONAL; ///< A factor for multiplication of personal best distances
	 double m_c_neighborhood = DEFAULTCNEIGHBORHOOD; ///< A factor for multiplication of neighborhood best distances
	 double m_c_global = DEFAULTCGLOBAL; ///< A factor for multiplication of global best distances
	 double m_c_velocity = DEFAULTCVELOCITY; ///< A factor for multiplication of velocities

	 updateRule m_update_rule = DEFAULTUPDATERULE; ///< Specifies how the parameters are updated
	 bool m_random_fill_up = true; ///< Specifies whether neighborhoods are filled up with random values

	 std::uint32_t m_repulsion_threshold = DEFREPULSIONTHRESHOLD; ///< The number of stalls until the swarm algorithm switches to repulsion instead of attraction

	 std::vector<double> m_dbl_lower_parameter_boundaries_cnt = std::vector<double>(); ///< Holds lower boundaries of double parameters
	 std::vector<double> m_dbl_upper_parameter_boundaries_cnt = std::vector<double>(); ///< Holds upper boundaries of double parameters
	 std::vector<double> m_dbl_vel_max_cnt = std::vector<double>(); ///< Holds the maximum allowed values of double-type velocities

	 double m_velocity_range_percentage = DEFAULTVELOCITYRANGEPERCENTAGE; ///< Indicates the percentage of a value range used for the initialization of the velocity

	 std::vector<std::shared_ptr<GParameterSet>> m_last_iteration_individuals_cnt; ///< A temporary copy of the last iteration's individuals

private:
	 /***************************************************************************/
	 // Virtual or overridden private functions

	 /** @brief Emits a name for this class / object */
	 G_API_GENEVA std::string name_() const override;
	 /** @brief Creates a deep clone of this object */
	 G_API_GENEVA GObject *clone_() const override;

	 /** @brief The actual business logic to be performed during each iteration; Returns the best achieved fitness */
	 G_API_GENEVA std::tuple<double, double> cycleLogic_() override;
	 /** @brief Updates the fitness of all individuals */
	 G_API_GENEVA void runFitnessCalculation_() override;

	 /** @brief Returns information about the type of optimization algorithm */
	 G_API_GENEVA std::string getAlgorithmPersonalityType_() const override;
	 /** @brief Returns the name of this optimization algorithm */
	 G_API_GENEVA std::string getAlgorithmName_() const override;

     /** @brief Retrieves the number of processable items for the current iteration */
     G_API_GENEVA std::size_t getNProcessableItems_() const override;

	 /** @brief Retrieve a GPersonalityTraits object belonging to this algorithm */
	 G_API_GENEVA std::shared_ptr<GPersonalityTraits> getPersonalityTraits_() const override;
     /** @brief Gives individuals an opportunity to update their internal structures */
     G_API_GENEVA void actOnStalls_() override;

	 /** @brief Resizes the population to the desired level and does some error checks */
	 G_API_GENEVA void adjustPopulation_() override;

	 /***************************************************************************/

	 /** @brief Helper function that checks the content of two nNeighborhoodMembers_ arrays */
	 bool nNeighborhoodMembersEqual(const std::vector<std::size_t>&, const std::vector<std::size_t>&) const;

	 /** @brief Small helper function that helps to fill up a neighborhood, if there is just one entry in it */
	 void fillUpNeighborhood1();
};

/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */

BOOST_CLASS_EXPORT_KEY(Gem::Geneva::GSwarmAlgorithm)


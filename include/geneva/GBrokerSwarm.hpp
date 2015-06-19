/**
 * @file GBrokerSwarm.hpp
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

// Global checks, defines and includes needed for all of Geneva
#include "common/GGlobalDefines.hpp"

// Standard headers go here

// Boost headers go here

#ifndef GBROKERSWARM_HPP_
#define GBROKERSWARM_HPP_

// Geneva headers go here
#include "common/GExceptions.hpp"
#include "courtier/GBufferPortT.hpp"
#include "courtier/GBrokerConnector2T.hpp"
#include "geneva/GSwarmPersonalityTraits.hpp"
#include "geneva/GBaseSwarm.hpp"

namespace Gem {
namespace Geneva {

/******************************************************************************/
/**
 * This class implements a swarm algorithm with the ability to delegate certain
 * tasks to remote clients, using Geneva's broker infrastructure.
 */
class GBrokerSwarm
	: public GBaseSwarm
		, public Gem::Courtier::GBrokerConnector2T<Gem::Geneva::GParameterSet>
{
	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int){
		using boost::serialization::make_nvp;

		ar
		& BOOST_SERIALIZATION_BASE_OBJECT_NVP(GBaseSwarm)
		& make_nvp("GBrokerConnector2T_GParameterSet",
					  boost::serialization::base_object<Gem::Courtier::GBrokerConnector2T<GParameterSet>>(*this));
	}
	///////////////////////////////////////////////////////////////////////

public:
	/** @brief The default constructor */
	G_API_GENEVA GBrokerSwarm();
	/** @brief The standard constructor */
	G_API_GENEVA GBrokerSwarm(const std::size_t&, const std::size_t&);
	/** @brief A standard copy constructor */
	G_API_GENEVA GBrokerSwarm(const GBrokerSwarm&);
	/** @brief The standard destructor */
	virtual G_API_GENEVA ~GBrokerSwarm();

	/** @brief The standard assignment operator */
	G_API_GENEVA const GBrokerSwarm& operator=(const GBrokerSwarm&);

	/** @brief Checks for equality with another GBrokerSwarm object */
	G_API_GENEVA bool operator==(const GBrokerSwarm&) const;
	/** @brief Checks for inequality with another GBrokerSwarm object */
	G_API_GENEVA bool operator!=(const GBrokerSwarm&) const;

	/** @brief Searches for compliance with expectations with respect to another object of the same type */
	virtual G_API_GENEVA void compare(
		const GObject& // the other object
		, const Gem::Common::expectation& // the expectation for this object, e.g. equality
		, const double& // the limit for allowed deviations of floating point types
	) const override;

	/** @brief Checks whether a given algorithm type likes to communicate via the broker */
	virtual G_API_GENEVA bool usesBroker() const override;

	/** @brief Adds local configuration options to a GParserBuilder object */
	virtual G_API_GENEVA void addConfigurationOptions (
		Gem::Common::GParserBuilder& gpb
	) override;

	/** @brief Allows to assign a name to the role of this individual(-derivative) */
	virtual G_API_GENEVA std::string getIndividualCharacteristic() const override;

	/** @brief Emits a name for this class / object */
	virtual G_API_GENEVA std::string name() const override;

protected:
	/***************************************************************************/
	/** @brief Loads the data of another GTransfer Population */
	virtual G_API_GENEVA void load_(const GObject *) override;
	/** @brief Creates a deep copy of this object */
	virtual G_API_GENEVA GObject *clone_() const override;

	/** @brief Performs any necessary initialization work before the start of the optimization cycle */
	virtual G_API_GENEVA void init() override;
	/** @brief Performs any necessary finalization work after the end of the optimization cycle */
	virtual G_API_GENEVA void finalize() override;

	/** @brief Updates all individual's positions */
	virtual G_API_GENEVA void updatePositions() override;
	/** @brief Triggers the fitness calculation of all individuals */
	virtual G_API_GENEVA void runFitnessCalculation() override;

	/** @brief Fixes the population after a job submission */
	virtual G_API_GENEVA void adjustNeighborhoods() override;
	/** @brief Checks whether each neighborhood has the default size */
	G_API_GENEVA bool neighborhoodsHaveNominalValues() const;

private:
	/***************************************************************************/

	std::vector<std::shared_ptr<GParameterSet>> oldIndividuals_; ///< A temporary copy of the last iteration's individuals
	std::vector<std::shared_ptr<GParameterSet>> oldWorkItems_; ///< Temporarily holds old returned work items

	/***************************************************************************/
	/**
	 * A simple comparison operator that helps to sort individuals according to their
	 * affiliation to a neighborhood. Smaller neighborhood numbers should be in front.
	 */
	class indNeighborhoodComp {
	public:
		bool operator()(std::shared_ptr<GParameterSet> x, std::shared_ptr<GParameterSet> y) {
			return x->getPersonalityTraits<GSwarmPersonalityTraits>()->getNeighborhood() < y->getPersonalityTraits<GSwarmPersonalityTraits>()->getNeighborhood();
		}
	};

	/***************************************************************************/

public:
	/** @brief Applies modifications to this object. This is needed for testing purposes */
	virtual G_API_GENEVA bool modify_GUnitTests() override;
	/** @brief Performs self tests that are expected to succeed. This is needed for testing purposes */
	virtual G_API_GENEVA void specificTestsNoFailureExpected_GUnitTests() override;
	/** @brief Performs self tests that are expected to fail. This is needed for testing purposes */
	virtual G_API_GENEVA void specificTestsFailuresExpected_GUnitTests() override;
};

/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */

BOOST_CLASS_EXPORT_KEY(Gem::Geneva::GBrokerSwarm)

#endif /* GBROKERSWARM_HPP_ */

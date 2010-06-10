/**
 * @file GSwarmAdaptor.hpp
 */

/* Copyright (C) Dr. Ruediger Berlich and Karlsruhe Institute of Technology
 * (University of the State of Baden-Wuerttemberg and National Laboratory
 * of the Helmholtz Association)
 *
 * Contact: info [at] gemfony (dot) com
 *
 * This file is part of the Geneva library, Gemfony scientific's optimization
 * library.
 *
 * Geneva is free software: you can redistribute it and/or modify
 * it under the terms of version 3 of the GNU Affero General Public License
 * as published by the Free Software Foundation.
 *
 * Geneva is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with the Geneva library.  If not, see <http://www.gnu.org/licenses/>.
 *
 * For further information on Gemfony scientific and Geneva, visit
 * http://www.gemfony.com .
 */

// Standard headers go here
#include <cmath>
#include <string>
#include <sstream>

// Includes check for correct Boost version(s)
#include "GGlobalDefines.hpp"

// Boost headers go here
#include <boost/cstdint.hpp>
#include <boost/limits.hpp>
#include <boost/cast.hpp>

#ifndef GSWARMADAPTOR_HPP_
#define GSWARMADAPTOR_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif

// GenEvA headers go here
#include "GAdaptorT.hpp"
#include "GBoundedDouble.hpp"
#include "GDoubleGaussAdaptor.hpp"
#include "GObject.hpp"
#include "GExceptions.hpp"
#include "GEnums.hpp"

namespace Gem {
namespace GenEvA {

/************************************************************************************************/
/*
 * Default settings for a number of local variables
 */

/** @brief The default multiplier for velocities */
const double DEFAULTOMEGA = 0.95;
/** @brief The default multiplier for the difference between individual and local best */
const double DEFAULTC1 = 2.;
/** @brief The default multiplier for the difference between individual and global best */
const double DEFAULTC2 = 2.;

/************************************************************************************************/
/**
 * This adaptor implements the adaptions performed by swarm algorithms. Just like swarm algorithms
 * it is specific to double values.
 */
class GSwarmAdaptor
	: public GAdaptorT<double>
{
	/********************************************************************************************/
	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive & ar, const unsigned int) {
		using boost::serialization::make_nvp;
		ar & make_nvp("GAdaptorT_double", boost::serialization::base_object<GAdaptorT<double> >(*this))
		   & BOOST_SERIALIZATION_NVP(omega_)
		   & BOOST_SERIALIZATION_NVP(c1_)
		   & BOOST_SERIALIZATION_NVP(c2_);
	}
	///////////////////////////////////////////////////////////////////////

public:
	/********************************************************************************************/
	/** @brief The standard constructor */
	GSwarmAdaptor();
	/** @brief A standard copy constructor */
	GSwarmAdaptor(const GSwarmAdaptor&);
	/** @brief The standard destructor */
	~GSwarmAdaptor();

	/** @brief A standard assignment operator for GSwarmAdaptor objects */
	const GSwarmAdaptor& operator=(const GSwarmAdaptor&);
	/** @brief Checks for equality with another GSwarmAdaptor object */

	bool operator==(const GSwarmAdaptor&) const;
	/** @brief Checks for inequality with another GSwarmAdaptor object */
	bool operator!=(const GSwarmAdaptor&) const;
	/** @brief Checks whether a given expectation for the relationship between this object and another object is fulfilled. */
	boost::optional<std::string> checkRelationshipWith(const GObject&, const Gem::Util::expectation&, const double&, const std::string&, const std::string&, const bool&) const;

	/** @brief Retrieves the id of the adaptor */
	virtual Gem::GenEvA::adaptorId getAdaptorId() const;

	/** @brief Prevents the adaption mode to be reset. This function is a trap. */
	virtual void setAdaptionMode(boost::logic::tribool);

	/** @brief Sets the \omega parameter used to multiply velocities with */
	void setOmega(const double&);
	/** @brief Retrieves the \omega parameter used to multiply velocities with */
	double getOmega() const;

	/** @brief Sets the c1 parameter used as a multiplier for the direction to the local best. */
	void setC1(const double&);
	/** @brief Retrieves the c1 parameter used as a multiplier for the direction to the local best. */
	double getC1() const;

	/** @brief Sets the c2 parameter used as a multiplier for the direction to the global best. */
	void setC2(const double&);
	/** @brief Retrieves the c2 parameter used as a multiplier for the direction to the global best. */
	double getC2() const;

#ifdef GENEVATESTING
	/** @brief Applies modifications to this object (needed for testing purposes. */
	virtual bool modify_GUnitTests();
	/** @brief Performs self tests that are expected to succeed. */
	virtual void specificTestsNoFailureExpected_GUnitTests();
	/** @brief Performs self tests that are expected to fail. */
	virtual void specificTestsFailuresExpected_GUnitTests();
#endif /* GENEVATESTING */

protected:
	/********************************************************************************************/
	/** @brief Loads the data of another GSwarmAdaptor */
	void load_(const GObject *);
	/** @brief Ceates a deep clone of this object */
	GObject *clone_() const;

	/** @brief The actual adaption function */
	virtual void customAdaptions(double&);

private:
	double omega_; ///< The \omega parameter used as a multiplicator to velocities in swarm algorithms
	double c1_; ///< A multiplier for the directions to the local best individual
	double c2_; ///< A multiplier for the directions to the global best individual

	std::vector<double> velocity_; ///< The velocity term used in swarm algorithms
	std::vector<double> localBest_; ///< The locally best solution(s)
	std::vector<double> globalBest_; ///< The globally best solution(s)
};

/************************************************************************************************/

} /* namespace GenEvA */
} /* namespace Gem */

#endif /* GSWARMADAPTOR_HPP_ */

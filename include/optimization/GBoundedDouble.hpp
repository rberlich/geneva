/**
 * @file GBoundedDouble.hpp
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
#include <vector>
#include <sstream>
#include <cmath>

// Includes check for correct Boost version(s)
#include "common/GGlobalDefines.hpp"

// Boost headers go here

#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/cast.hpp> // For boost::numeric_cast<>
#include <boost/optional.hpp>

#ifndef GBOUNDEDDOUBLE_HPP_
#define GBOUNDEDDOUBLE_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif


// Geneva headers go here
#include "GBoundedNumT.hpp"

namespace Gem
{
namespace Geneva
{

  /******************************************************************************/
  /**
   * The GBoundedDouble class allows to limit the value range of a double value,
   * while applying adaptions to a continuous range. This is done by means of a
   * mapping from an internal representation to an externally visible value.
   */
class GBoundedDouble
	:public GBoundedNumT<double>
{
	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int){
	  using boost::serialization::make_nvp;

	  ar & make_nvp("GBoundedNumT_double", boost::serialization::base_object<GBoundedNumT<double> >(*this));
	}
	///////////////////////////////////////////////////////////////////////

public:
	/** @brief The default constructor */
	GBoundedDouble();
	/** @brief Initialization with boundaries only */
	GBoundedDouble(const double&, const double&);
	/** @brief Initialization with value and boundaries */
	GBoundedDouble(const double&, const double&, const double&);
	/** @brief The copy constructor */
	GBoundedDouble(const GBoundedDouble&);
	/** @brief Initialization by contained value */
	explicit GBoundedDouble(const double&);
	/** @brief The destructor */
	virtual ~GBoundedDouble();

	/** @brief An assignment operator */
	virtual double operator=(const double&);

	/** @brief A standard assignment operator */
	const GBoundedDouble& operator=(const GBoundedDouble&);

	/** @brief Checks for equality with another GBoundedDouble object */
	bool operator==(const GBoundedDouble&) const;
	/** @brief Checks for inequality with another GBoundedDouble object */
	bool operator!=(const GBoundedDouble&) const;

	/** @brief Checks whether this object fulfills a given expectation in relation to another object */
	virtual boost::optional<std::string> checkRelationshipWith(const GObject&, const Gem::Common::expectation&, const double&, const std::string&, const std::string&, const bool&) const;

#ifdef GENEVATESTING
	/** @brief Applies modifications to this object. This is needed for testing purposes */
	virtual bool modify_GUnitTests();
	/** @brief Performs self tests that are expected to succeed. This is needed for testing purposes */
	virtual void specificTestsNoFailureExpected_GUnitTests();
	/** @brief Performs self tests that are expected to fail. This is needed for testing purposes */
	virtual void specificTestsFailuresExpected_GUnitTests();
#endif /* GENEVATESTING */

protected:
	/** @brief Loads the data of another GObject */
	virtual void load_(const GObject*);
	/** @brief Creates a deep clone of this object. */
	virtual GObject* clone_() const;

	/** @brief Triggers random initialization of the parameter object */
	virtual void randomInit_();
	/** @brief Initializes double-based parameters with a given value */
	virtual void fixedValueInit_(const double&);
	/** @brief Multiplies double-based parameters with a given value */
	virtual void multiplyBy_(const double&);
};

  /******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */

#endif /* GBOUNDEDDOUBLE_HPP_ */

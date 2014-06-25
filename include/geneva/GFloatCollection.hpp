/**
 * @file GFloatCollection.hpp
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

// Standard header files go here

// Boost header files go here

#ifndef GFLOATCOLLECTION_HPP_
#define GFLOATCOLLECTION_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif

// Geneva header files go here
#include "geneva/GFPNumCollectionT.hpp"
#include "geneva/GFloatGaussAdaptor.hpp"

namespace Gem {
namespace Geneva {

/******************************************************************************/
/**
 * A collection of float objects without boundaries
 */
class GFloatCollection
	:public GFPNumCollectionT<float>
{
	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int){
	  using boost::serialization::make_nvp;

	  ar & make_nvp("GFPNumCollectionT_float", boost::serialization::base_object<GFPNumCollectionT<float> >(*this));
	}
	///////////////////////////////////////////////////////////////////////

public:
	/** @brief The default constructor */
	GFloatCollection();
	/** @brief The copy constructor */
	GFloatCollection(const GFloatCollection&);
	/** @brief Initialization with a number of random values in a given range */
	GFloatCollection(
      const std::size_t&
      , const float&
      , const float&
	);
	/** @brief Initialization with a number of predefined values in all positions */
	GFloatCollection(
      const std::size_t&
      , const float&
      , const float&
      , const float&
	);
	/** @brief The destructor */
	virtual ~GFloatCollection();

	/** @brief A standard assignment operator */
	const GFloatCollection& operator=(const GFloatCollection&);

	/** @brief Checks for equality with another GFloatCollection object */
	bool operator==(const GFloatCollection&) const;
	/** @brief Checks for inequality with another GFloatCollection object */
	bool operator!=(const GFloatCollection&) const;

	/** @brief Checks whether this object fulfills a given expectation in relation to another object */
	virtual boost::optional<std::string> checkRelationshipWith(
      const GObject&
      , const Gem::Common::expectation&
      , const double&
      , const std::string&
      , const std::string&
      , const bool&
	) const OVERRIDE;

   /** @brief Emits a name for this class / object */
   virtual std::string name() const OVERRIDE;

protected:
	/** @brief Loads the data of another GObject */
	virtual void load_(const GObject*) OVERRIDE;
	/** @brief Creates a deep clone of this object. */
	virtual GObject* clone_() const OVERRIDE;

   /** @brief Attach our local values to the vector. */
   virtual void floatStreamline(std::vector<float>&, const activityMode& am) const OVERRIDE;
   /** @brief Attach boundaries of type float to the vectors */
   virtual void floatBoundaries(std::vector<float>&, std::vector<float>&, const activityMode& am) const OVERRIDE;
   /** @brief Tell the audience that we own a number of float values */
   virtual std::size_t countFloatParameters(const activityMode& am) const OVERRIDE;
   /** @brief Assigns part of a value vector to the parameter */
   virtual void assignFloatValueVector(const std::vector<float>&, std::size_t&, const activityMode& am) OVERRIDE;
   /** @brief Attach our local values to the vector. */
   virtual void floatStreamline(std::map<std::string, std::vector<float> >&, const activityMode& am) const OVERRIDE;
   /** Assigns part of a value map to the parameter */
   void assignFloatValueVectors(const std::map<std::string, std::vector<float> >& parMap, const activityMode& am);


public:
	/** @brief Applies modifications to this object. This is needed for testing purposes */
	virtual bool modify_GUnitTests() OVERRIDE;
	/** @brief Fills the collection with some random data */
	void fillWithData(const std::size_t&);
	/** @brief Performs self tests that are expected to succeed. This is needed for testing purposes */
	virtual void specificTestsNoFailureExpected_GUnitTests() OVERRIDE;
	/** @brief Performs self tests that are expected to fail. This is needed for testing purposes */
	virtual void specificTestsFailuresExpected_GUnitTests() OVERRIDE;
};

/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */

BOOST_CLASS_EXPORT_KEY(Gem::Geneva::GFloatCollection)

#endif /* GFLOATCOLLECTION_HPP_ */

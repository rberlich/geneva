/**
 * @file GConstrainedDoubleCollection.hpp
 */

/*
 * Copyright (C) Gemfony scientific UG (haftungsbeschraenkt) and Karlsruhe
 * Institute of Technology (University of the State of Baden-Wuerttemberg
 * and National Laboratory of the Helmholtz Association).
 *
 * See the AUTHORS file in the top-level directory for a list of authors.
 *
 * Contact: contact [at] gemfony (dot) com
 *
 * This file is part of the Geneva library collection
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


// Standard header files go here
#include <string>
#include <sstream>
#include <vector>

// Includes check for correct Boost version(s)

// Boost header files go here

#ifndef GCONSTRAINEDDOUBLECOLLECTION_HPP_
#define GCONSTRAINEDDOUBLECOLLECTION_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif


// Geneva header files go here
#include "geneva/GConstrainedFPNumCollectionT.hpp"

namespace Gem {
namespace Geneva {

/**********************************************************************/
/**
 * This class represents a collection of double values with common
 * boundaries, all modified using the same algorithm. Note: If you want
 * to access or set the transformed value, use the value() and setValue()
 * functions. Using the subscript operator or at() function, or the
 * native iterator, will give you the "raw" data only.
 */
class GConstrainedDoubleCollection
	: public GConstrainedFPNumCollectionT<double>
{
	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int) {
		using boost::serialization::make_nvp;
		ar & make_nvp("GConstrainedFPNumCollectionT", boost::serialization::base_object<GConstrainedFPNumCollectionT<double> >(*this));
	}
	///////////////////////////////////////////////////////////////////////

public:
	/******************************************************************/
	/** @brief Initialize the lower and upper boundaries for data members of this class */
	GConstrainedDoubleCollection (const std::size_t&, const double&, const double&);
	/** @brief The standard copy constructor */
	GConstrainedDoubleCollection(const GConstrainedDoubleCollection&);
	/** @brief The standard destructor */
	virtual ~GConstrainedDoubleCollection();

	/**  @brief The standard assignment operator */
	const GConstrainedDoubleCollection& operator=(const GConstrainedDoubleCollection&);

	/** @brief Checks for equality with another GConstrainedDoubleCollection object */
	bool operator==(const GConstrainedDoubleCollection&) const;
	/** @brief Checks for inequality with another GConstrainedDoubleCollection object */
	bool operator!=(const GConstrainedDoubleCollection&) const;

	/** @brief Checks whether a given expectation is fulfilled */
	boost::optional<std::string> checkRelationshipWith(
			const GObject&
			, const Gem::Common::expectation&
			, const double&
			, const std::string&
			, const std::string&
			, const bool&
	) const;

	/** @brief Attach our local values to the vector. */
	virtual void doubleStreamline(std::vector<double>&) const;
	/** @brief Tell the audience that we own a number of double values */
	virtual std::size_t countDoubleParameters() const;
	/** @brief Assigns part of a value vector to the parameter */
	virtual void assignDoubleValueVector(const std::vector<double>&, std::size_t&);

protected:
	/******************************************************************/
	/** @brief Loads the data of another GConstrainedDoubleCollection object */
	virtual void load_(const GObject *);
	/** @brief Creates a deep clone of this object */
	GObject* clone_() const;

	/******************************************************************/
	/** @brief The default constructor. Intentionally protected	 */
	GConstrainedDoubleCollection();

#ifdef GENEVATESTING
public:
	/******************************************************************/
	/** @brief Applies modifications to this object */
	virtual bool modify_GUnitTests();
	/** @brief Performs self tests that are expected to succeed */
	virtual void specificTestsNoFailureExpected_GUnitTests();
	/** @brief Performs self tests that are expected to fail */
	virtual void specificTestsFailuresExpected_GUnitTests();
	/******************************************************************/
#endif /* GENEVATESTING */

};

/**********************************************************************/

} /* namespace Geneva */
} /* namespace Gem */

#ifdef GENEVATESTING
// Tests of this class (and parent classes)
/*************************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////
/*************************************************************************************************/
/** @brief We need to provide a specialization of the factory function that creates objects of this type. */
template <> boost::shared_ptr<Gem::Geneva::GConstrainedDoubleCollection>
TFactory_GUnitTests<Gem::Geneva::GConstrainedDoubleCollection>();

/*************************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////
/*************************************************************************************************/
#endif /* GENEVATESTING */

BOOST_CLASS_EXPORT_KEY(Gem::Geneva::GConstrainedDoubleCollection)

/**********************************************************************/

#endif /* GCONSTRAINEDDOUBLECOLLECTION_HPP_ */
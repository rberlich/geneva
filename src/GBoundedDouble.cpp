/**
 * @file GBoundedDouble.cpp
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

#include "GBoundedDouble.hpp"

/**
 * Included here so no conflicts occur. See explanation at
 * http://www.boost.org/libs/serialization/doc/special.html#derivedpointers
 */
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(Gem::GenEvA::GBoundedDouble)

namespace Gem {
namespace GenEvA {

/*******************************************************************************************/
/**
 * The default constructor
 */
GBoundedDouble::GBoundedDouble()
{ /* nothing */ }

/*******************************************************************************************/
/**
 * Initialization with boundaries only. The value is set randomly.
 *
 * @param lowerBoundary The lower boundary of the value range
 * @param upperBoundary The upper boundary of the value range
 */
GBoundedDouble::GBoundedDouble(const double& lowerBoundary, const double& upperBoundary)
	: GBoundedNumT<double>(lowerBoundary, upperBoundary)
{ /* nothing */ }

/*******************************************************************************************/
/**
 * Initialization with value and boundaries
 *
 * @param val Initialization value
 * @param lowerBoundary The lower boundary of the value range
 * @param upperBoundary The upper boundary of the value range
 */
GBoundedDouble::GBoundedDouble(const double& val,
		                       const double& lowerBoundary,
		                       const double& upperBoundary)
	: GBoundedNumT<double>(val, lowerBoundary, upperBoundary)
{ /* nothing */ }

/*******************************************************************************************/
/**
 * The copy constructor
 *
 * @param cp A copy of another GBoundedDouble object
 */
GBoundedDouble::GBoundedDouble(const GBoundedDouble& cp)
	: GBoundedNumT<double>(cp)
{ /* nothing */ }

/*******************************************************************************************/
/**
 * Initialization by contained value
 *
 * @param val A value used for the initialization
 */
GBoundedDouble::GBoundedDouble(const double& val)
	: GBoundedNumT<double>(val)
{ /* nothing */ }

/*******************************************************************************************/
/**
 * The destructor
 */
GBoundedDouble::~GBoundedDouble()
{ /* nothing */ }

/*******************************************************************************************/
/**
 * An assignment operator for the contained value type
 *
 * @param val The value to be assigned to this object
 * @return The value that was just assigned to this object
 */
const double& GBoundedDouble::operator=(const double& val) {
	return GBoundedNumT<double>::operator=(val);
}

/*******************************************************************************************/
/**
 * A standard assignment operator.
 *
 * @param cp A copy of another GBoundedDouble object
 * @return A constant reference to this object
 */
const GBoundedDouble& GBoundedDouble::operator=(const GBoundedDouble& cp){
	GBoundedDouble::load(&cp);
	return *this;
}

/*******************************************************************************************/
/**
 * Creates a deep clone of this object.
 *
 * @return A copy of this object, camouflaged as a GObject
 */
GObject* GBoundedDouble::clone_() const {
	return new GBoundedDouble(*this);
}

/*******************************************************************************************/
/**
 * Checks for equality with another GBoundedDouble object
 *
 * @param  cp A constant reference to another GBoundedDouble object
 * @return A boolean indicating whether both objects are equal
 */
bool GBoundedDouble::operator==(const GBoundedDouble& cp) const {
	return GBoundedDouble::isEqualTo(cp, boost::logic::indeterminate);
}

/*******************************************************************************************/
/**
 * Checks for inequality with another GBoundedDouble object
 *
 * @param  cp A constant reference to another GBoundedDouble object
 * @return A boolean indicating whether both objects are inequal
 */
bool GBoundedDouble::operator!=(const GBoundedDouble& cp) const {
	return !GBoundedDouble::isEqualTo(cp, boost::logic::indeterminate);
}

/*******************************************************************************************/
/**
 * Checks for equality with another GBoundedDouble object.  If T is an object type,
 * then it must implement operator!= .
 *
 * @param  cp A constant reference to another GBoundedDouble object
 * @return A boolean indicating whether both objects are equal
 */
bool GBoundedDouble::isEqualTo(const GObject& cp, const boost::logic::tribool& expected) const {
    using namespace Gem::Util;

	// Check that we are indeed dealing with a GParamterT reference
	const GBoundedDouble *p_load = GObject::conversion_cast(&cp,  this);

	// Check equality of the parent class
	if(!GBoundedNumT<double>::isEqualTo(*p_load, expected)) return false;

	// No local data

	return true;
}

/*******************************************************************************************/
/**
 * Checks for similarity with another GBoundedDouble object.
 *
 * @param  cp A constant reference to another GBoundedDouble object
 * @param limit A double value specifying the acceptable level of differences of floating point values
 * @return A boolean indicating whether both objects are similar to each other
 */
bool GBoundedDouble::isSimilarTo(const GObject& cp, const double& limit, const boost::logic::tribool& expected) const {
    using namespace Gem::Util;

	// Check that we are indeed dealing with a GParamterT reference
	const GBoundedDouble *p_load = GObject::conversion_cast(&cp,  this);

	// Check similarity of the parent class
	if(!GBoundedNumT<double>::isSimilarTo(*p_load, limit, expected)) return false;

	// No local data

	return true;
}

/*******************************************************************************************/
/**
 * Loads the data of another GObject
 *
 * @param cp A copy of another GBoundedDouble object, camouflaged as a GObject
 */
void GBoundedDouble::load(const GObject* cp){
	// Convert cp into local format (also checks for the type of cp)
	const GBoundedDouble *p_load = GObject::conversion_cast(cp, this);

	// Load our parent class'es data ...
	GBoundedNumT<double>::load(cp);

	// ... no local data
}

/*******************************************************************************************/

} /* namespace GenEvA */
} /* namespace Gem */

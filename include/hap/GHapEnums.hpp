/**
 * @file GRandomT.hpp
 */

/* Copyright (C) Dr. Ruediger Berlich and Karlsruhe Institute of Technology
 * (University of the State of Baden-Wuerttemberg and National Laboratory
 * of the Helmholtz Association)
 *
 * Contact: info [at] gemfony (dot) com
 *
 * This file is part of the Hap library, Gemfony scientific's library of
 * random number routines.
 *
 * Hap is free software: you can redistribute it and/or modify
 * it under the terms of version 3 of the GNU Affero General Public License
 * as published by the Free Software Foundation.
 *
 * Hap is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with the Hap library.  If not, see <http://www.gnu.org/licenses/>.
 *
 * For further information on Gemfony scientific and Hap, visit
 * http://www.gemfony.com .
 */

// Standard headers go here
#include <string>
#include <istream>
#include <ostream>

// Includes check for correct Boost version(s)
#include "GGlobalDefines.hpp"

// Boost headers go here

#include <boost/cstdint.hpp>

#ifndef GHAPENUMS_HPP_
#define GHAPENUMS_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif

// Hap headers go here

namespace Gem {
namespace Hap {

/**********************************************************************************************/
/**
 * Allowed specializations of Gem::Hap::GRandomT<T>
 */
enum gRandomTSpecialization {
	RANDOMPROXY = 0 // random numbers are taken from the factory
  , RANDOMLOCAL = 1 // random numbers are produced locally, using a seed taken from the seed manager or provided to the constructor
};

/**********************************************************************************************

/** @brief Puts a Gem::Hap::gRandomTSpecialization into a stream. Needed also for boost::lexical_cast<> */
std::ostream& operator<<(std::ostream&, const Gem::Hap::gRandomTSpecialization&);
/** @brief Reads a Gem::Hap::gRandomTSpecialization item from a stream. Needed also for boost::lexical_cast<> */
std::istream& operator>>(std::istream&, Gem::Hap::gRandomTSpecialization&);

/**********************************************************************************************/

} /* namespace Hap */
} /* namespace Gem */

#endif /* GHAPENUMS_HPP_ */

/**
 * @file
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

#pragma once

// Global checks, defines and includes needed for all of Geneva
#include "common/GGlobalDefines.hpp"

// Standard header files go here
#include <sstream>
#include <vector>

// Boost header files go here

// Geneva header files go here

namespace Gem {
namespace Geneva {

/******************************************************************************/
/**
 * A simple interface class for objects that can be evaluated.
 */
class G_Interface_Rateable {
public:
	 /** @brief The default constructor */
	 G_API_GENEVA G_Interface_Rateable() = default;

	 /** @brief Retrieves the stored raw fitness with a given id */
	 virtual G_API_GENEVA double raw_fitness(std::size_t = 0) const BASE = 0;
	 /** @brief Retrieves the stored transformed fitness with a given id */
	 virtual G_API_GENEVA double transformed_fitness(std::size_t = 0) const BASE = 0;


	 /** @brief Returns all raw fitness results in a std::vector */
	 virtual G_API_GENEVA std::vector<double> raw_fitness_vec() const BASE = 0;
	 /** @brief Returns all transformed fitness results in a std::vector */
	 virtual G_API_GENEVA std::vector<double> transformed_fitness_vec() const BASE = 0;

protected:
	/**
      * The destructor. Making this function protected and non-virtual follows
      * this discussion: http://www.gotw.ca/publications/mill18.htm
      */
	 G_API_GENEVA ~G_Interface_Rateable() BASE = default;
};

/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */


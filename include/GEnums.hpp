/**
 * \file
 */

/**
 * Copyright (C) 2004-2008 Dr. Ruediger Berlich
 * Copyright (C) 2007-2008 Forschungszentrum Karlsruhe GmbH
 *
 * This file is part of Geneva, Gemfony scientific's optimization library.
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
 */

#include <boost/version.hpp>

#if BOOST_VERSION < 103600
#error "Error: Boost should at least have version 1.36 !"
#endif /* BOOST_VERSION */

#include <boost/cstdint.hpp>

#ifndef GENUMS_H_
#define GENUMS_H_

namespace GenEvA
{
  /**
   * This enumeration is needed because
   * vector<bool>[i] returns a helper class,
   * which breaks GenEvA
   */
  enum bit {FALSE, TRUE}; // 0,1

  /**
   * Return values for member functions
   */
  const int16_t ExitOK = 0;
  const int16_t ExitBad = 1;

} /* namespace GenEvA */

#endif /*GENUMS_H_*/

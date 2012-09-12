/**
 * @file GCommandLineParser.hpp
 */

/*
 * Copyright (C) Gemfony scientific UG (haftungsbeschraenkt)
 *
 * See the AUTHORS file in the top-level directory for a list of authors.
 *
 * Contact: contact [at] gemfony (dot) com
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
 * http://www.gemfony.com .
 */

// Standard headers go here

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// Boost headers go here

#include <boost/version.hpp>

#if BOOST_VERSION < 103600
#error "Error: Boost should at least have version 1.36 !"
#endif /* BOOST_VERSION */

#include <boost/cstdint.hpp>
#include <boost/program_options.hpp>

#ifndef GCOMMANDLINEPARSER_HPP_
#define GCOMMANDLINEPARSER_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif

// Geneva headers go here
#include "common/GCommonEnums.hpp"

namespace Gem {
namespace Tests {

// Default settings
const float CMD_DEFAULTSIGMA1 = 1.f;
const float CMD_DEFAULTSIGMA2 = 1.f;
const float CMD_DEFAULTSIGMASIGMA1 = 0.001f;
const float CMD_DEFAULTSIGMASIGMA2 = 0.001f;
const float CMD_DEFAULTMINSIGMA1 = 0.002f;
const float CMD_DEFAULTMAXSIGMA1 = 4.f;
const float CMD_DEFAULTMINSIGMA2 = 0.002f;
const float CMD_DEFAULTMAXSIGMA2 = 4.f;
const float CMD_DEFAULTDELTA = 0.5f;
const float CMD_DEFAULTSIGMADELTA = 0.8f;
const float CMD_DEFAULTMINDELTA = 0.001f;
const float CMD_DEFAULTMAXDELTA = 2.f;
const boost::uint32_t CMD_DEFAULTMAXITER = 100000;
const std::string CMD_DEFAULTRESULTFILE = "result.C";
const bool CMD_DEFAULTVERBOSE = true;
const boost::uint32_t CMD_DEFAULTADAPTIONTHRESHOLD=1;

namespace po = boost::program_options;

bool parseCommandLine(
		int argc, char **argv
		, float& sigma1
		, float& sigmaSigma1
		, float& minSigma1
		, float& maxSigma1
		, float& sigma2
		, float& sigmaSigma2
		, float& minSigma2
		, float& maxSigma2
		, float& delta
		, float& sigmaDelta
		, float& minDelta
		, float& maxDelta
		, boost::uint32_t& adaptionThreshold
		, std::string& resultFile
		, boost::uint32_t& maxIter
		, bool& verbose
);

} /* namespace Tests */
} /* namespace Gem */

#endif /* GCOMMANDLINEPARSER_HPP_ */
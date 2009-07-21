/**
 * @file GCommandLineParser.hpp
 */

/* Copyright (C) 2004-2008 Dr. Ruediger Berlich
 * Copyright (C) 2008 Forschungszentrum Karlsruhe GmbH
 *
 * This file is part of the Geneva library.
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

#include <boost/program_options.hpp>

#ifndef GCOMMANDLINEPARSER_HPP_
#define GCOMMANDLINEPARSER_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif

// GenEvA headers go here
#include "GEnums.hpp"
#include "GSerializationHelperFunctions.hpp"

namespace Gem
{
namespace GenEvA
{

// Default settings
const std::size_t DEFAULTPARABOLADIMENSION=1000;
const double DEFAULTPARABOLAMIN=-100.;
const double DEFAULTPARABOLAMAX=100.;
const boost::uint16_t DEFAULTNPRODUCERTHREADS=10;
const std::size_t DEFAULTPOPULATIONSIZE=100;
const std::size_t DEFAULTNPARENTS=5; // Allow to explore the parameter space from many starting points
const boost::uint32_t DEFAULTMAXGENERATIONS=2000;
const boost::uint32_t DEFAULTMAXSTALLGENERATIONS=100;
const long DEFAULTMAXMINUTES=10;
const boost::uint32_t DEFAULTREPORTGENERATION=1;
const recoScheme DEFAULTRSCHEME=VALUERECOMBINE;
const bool DEFAULTVERBOSE=true;
const boost::uint32_t DEFAULTADAPTIONTHRESHOLD=1;
const bool DEFAULTPARALLEL=true;
const bool DEFAULTMAXIMIZE=false;
const sortingMode DEFAULTSORTINGSCHEME=MUPLUSNU;
const double DEFAULTQTHRESHOLD=0.;
const std::size_t DEFAULTARRAYSIZE=1000;
const bool DEFAULTPRODUCTIONPLACE=true; // remote production

namespace po = boost::program_options;

bool parseCommandLine(int argc, char **argv,
					  std::size_t& parabolaDimension,
					  double& parabolaMin,
					  double& parabolaMax,
					  boost::uint32_t& adaptionThreshold,
					  boost::uint16_t& nProducerThreads,
					  std::size_t& populationSize,
					  std::size_t& nParents,
					  boost::uint32_t& maxGenerations,
					  boost::uint32_t& maxStallGenerations,
					  double &qualityThreshold,
					  long& maxMinutes,
					  boost::uint32_t& reportGeneration,
					  recoScheme& rScheme,
					  sortingMode& smode,
					  bool& parallel,
					  bool& maximize,
					  std::size_t& arraySize,
					  bool& productionPlace,
					  bool& verbose);

} /* namespace GenEvA */
} /* namespace Gem */

#endif /* GCOMMANDLINEPARSER_HPP_ */

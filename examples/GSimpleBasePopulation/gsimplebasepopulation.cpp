/**
 * @file GSimpleBasePopulation.cpp
 */

/* Copyright (C) 2004-2008 Dr. Ruediger Berlich
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

// Standard header files go here
#include <iostream>
#include <cmath>
#include <sstream>

// Boost header files go here
#include <boost/function.hpp>

// GenEvA header files go here
#include "GBasePopulation.hpp"
#include "GDoubleCollection.hpp"
#include "GParameterSet.hpp"
#include "GDoubleGaussAdaptor.hpp"
#include "GLogger.hpp"
#include "GLogTargets.hpp"
#include "GBoostThreadPopulation.hpp"

// The individual that should be optimized
#include "GParabolaIndidvidual.hpp"

using namespace Gem::GenEvA;

/************************************************************************************************/
/**
 * The main function. We search for the minimum of a parabola.
 */
int main(int argc, char **argv){
	// Add some log levels to the logger
	LOGGER.addLogLevel(Gem::GLogFramework::CRITICAL);
	LOGGER.addLogLevel(Gem::GLogFramework::WARNING);
	LOGGER.addLogLevel(Gem::GLogFramework::INFORMATIONAL);
	LOGGER.addLogLevel(Gem::GLogFramework::PROGRESS);

	// Add log targets to the system
	LOGGER.addTarget(new Gem::GLogFramework::GDiskLogger("GSimpleBasePopulation.log"));
	LOGGER.addTarget(new Gem::GLogFramework::GConsoleLogger());

	// Random numbers are our most valuable good. Set the number of threads
	GRANDOMFACTORY.setNProducerThreads(10);

	// Set up a single parabola individual
	boost::shared_ptr<GParabolaIndividual> parabolaIndividual(new GParabolaIndividual(1000, -100.,100.));

	// Now we've got our first individual and can create a population.
	// You can choose between a simple, non-parallel population and a
	// multi-threaded population.

	// Uncomment the next line and comment out the GBoostThreadPopulation lines
	// to get the slower, serial execution.

	// GBasePopulation pop;

	GBoostThreadPopulation pop;
	pop.setNThreads(4);

	pop.append(parabolaIndividual);

	// Specify some population settings
	pop.setPopulationSize(100,5); // 100 individuals, 5 parents
	pop.setMaxGeneration(2000); // A maximum of 2000 generations is allowed
	pop.setMaxTime(boost::posix_time::minutes(5)); // Calculation should be finished after 5 minutes
	pop.setReportGeneration(1); // Emit information during every generation
	pop.setRecombinationMethod(VALUERECOMBINE); // The best parents have higher chances of survival

	// Do the actual optimization
	pop.optimize();

	std::cout << "Done ..." << std::endl;

	return 0;
}

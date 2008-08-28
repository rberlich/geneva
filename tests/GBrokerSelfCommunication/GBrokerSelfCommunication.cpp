/**
 * @file GBrokerPopulation.cpp
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
#include <iterator>
#include <cmath>
#include <sstream>
#include <string>
#include <vector>

// Boost header files go here
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

// GenEvA header files go here
#include "GRandom.hpp"
#include "GBasePopulation.hpp"
#include "GDoubleCollection.hpp"
#include "GParameterSet.hpp"
#include "GDoubleGaussAdaptor.hpp"
#include "GLogger.hpp"
#include "GLogTargets.hpp"
#include "GBrokerPopulation.hpp"
#include "GIndividualBroker.hpp"
#include "GAsioTCPConsumer.hpp"
#include "GAsioTCPClient.hpp"
#include "GEnums.hpp"
#include "GThreadGroup.hpp"

// The individual that should be optimized.
// Represents the projection of an m-dimensional
// data set to an n-dimensional data set.
#include "GProjectionIndividual.hpp"

// Parses the command line for all required options
#include "GCommandLineParser.hpp"

using namespace Gem::GenEvA;
using namespace Gem::Util;
using namespace Gem::GLogFramework;

/************************************************************************************************/
/**
 * The main function. We search for the minimum of a parabola, with the help of multiple clients,
 * possibly running on different machines.
 */
int main(int argc, char **argv){
	std::string ip;
	std::size_t nData=10000, nDimOrig=5, nDimTarget=2;
	std::size_t nClients;
	double radius;
	unsigned short port=10000;
	std::size_t populationSize, nParents;
	boost::uint16_t nProducerThreads;
	boost::uint32_t maxGenerations, reportGeneration;
	long maxMinutes;
	bool verbose;
	recoScheme rScheme;

	////////////////////////////////////////////////////////////////////////////////////////////////
	// Command-line parsing
	if(!parseCommandLine(argc, argv,
						 nData,
				         nDimOrig,
				         nDimTarget,
				         radius,
				         nClients,
				         nProducerThreads,
				         populationSize,
				         nParents,
				         maxGenerations,
				         maxMinutes,
				         reportGeneration,
				         rScheme,
				         verbose))
		{ std::terminate(); }

	////////////////////////////////////////////////////////////////////////////////////////////////
	// Creation of an input file for this example
	GProjectionIndividual::createSphereFile("sphere.xml",
											nData,
											nDimOrig,
											nDimTarget,
											radius);

	////////////////////////////////////////////////////////////////////////////////////////////////
	// Set-up of local resources

	// Add some log levels to the logger
	LOGGER.addLogLevel(Gem::GLogFramework::CRITICAL);
	LOGGER.addLogLevel(Gem::GLogFramework::WARNING);
	LOGGER.addLogLevel(Gem::GLogFramework::INFORMATIONAL);
	LOGGER.addLogLevel(Gem::GLogFramework::PROGRESS);

	// Add log targets to the system
	LOGGER.addTarget(boost::shared_ptr<GBaseLogTarget>(new GDiskLogger("GBrokerPopulation.log")));
	LOGGER.addTarget(boost::shared_ptr<GBaseLogTarget>(new GConsoleLogger()));

	// Random numbers are our most valuable good. Set the number of threads
	GRANDOMFACTORY.setNProducerThreads(nProducerThreads);

	// Global settings
	ip="localhost";
	port=10000;

	////////////////////////////////////////////////////////////////////////////////////////////////
	// Start of server

	// Create a consumer and enrol it with the broker
	boost::shared_ptr<GAsioTCPConsumer> gatc(new GAsioTCPConsumer(port));
	// gatc->setSerializationMode(BINARYSERIALIZATION);
	GINDIVIDUALBROKER.enrol(gatc);

	// Set up a single projection individual
	boost::shared_ptr<GProjectionIndividual> projectionIndividual(new GProjectionIndividual("sphere.xml"));

	// Create the actual population
	GBrokerPopulation pop;

	// Make the individual known to the population
	pop.append(projectionIndividual);

	// Specify some population settings
	pop.setPopulationSize(populationSize,nParents);
	pop.setMaxGeneration(maxGenerations);
	pop.setMaxTime(boost::posix_time::minutes(maxMinutes));
	pop.setReportGeneration(reportGeneration);
	pop.setRecombinationMethod(rScheme);

	// Do the actual optimization
	pop.optimize();

	////////////////////////////////////////////////////////////////////////////////////////////////
	// Start of clients

	// We create a thread group of nClients threads and start the clients one by one.
	// Note that boost::bind knows how to handle a boost::shared_ptr, and that the
	// GAsioTCPClients will be destroyed automatically at the end of the execution.
	Gem::Util::GThreadGroup gtg;

	std::vector<boost::shared_ptr<GAsioTCPClient> > clientCollection;
	for(std::size_t i=0; i<nClients; i++){
		boost::shared_ptr<GAsioTCPClient> p(new GAsioTCPClient(ip,boost::lexical_cast<std::string>(port)));
		gtb.create_thread(boost::bind(&GAsioTCPClient::run,p));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////
	std::cout << "Done ..." << std::endl;

	return 0;
}

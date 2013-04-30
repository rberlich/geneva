/**
 * @file GSimulatedAnnealingFactory.hpp
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

// Standard header files go here
#include <string>

// Boost header files go here
#include <boost/date_time.hpp>
#include <boost/filesystem.hpp>
#include <boost/function.hpp>

#ifndef GSIMULATEDANNEALINGFACTORY_HPP_
#define GSIMULATEDANNEALINGFACTORY_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif

// Geneva headers go here
#include "courtier/GCourtierEnums.hpp"
#include "geneva/GOptimizationAlgorithmFactoryT.hpp"
#include "geneva/GBaseSA.hpp"
#include "geneva/GSerialSA.hpp"
#include "geneva/GMultiThreadedSA.hpp"
#include "geneva/GBrokerSA.hpp"

namespace Gem {
namespace Geneva {

/******************************************************************************/
/**
 * This class is a specialization of the GFactoryT<> class for evolutionary algorithms.
 */
class GSimulatedAnnealingFactory
   : public GOptimizationAlgorithmFactoryT<GOptimizationAlgorithmT<GParameterSet> >
{
public:
   /** @brief The standard constructor */
   GSimulatedAnnealingFactory(
         const std::string&
         , const parMode&
   );
   /** @brief Adds a content creator in addition to the standard values */
   GSimulatedAnnealingFactory(
         const std::string&
         , const parMode&
         , boost::shared_ptr<Gem::Common::GFactoryT<GParameterSet> >
   );
   /** @brief The destructor */
   virtual ~GSimulatedAnnealingFactory();

protected:
   /** @brief Creates individuals of this type */
   virtual boost::shared_ptr<GOptimizationAlgorithmT<GParameterSet> > getObject_(Gem::Common::GParserBuilder&, const std::size_t&);
   /** @brief Allows to act on the configuration options received from the configuration file */
   virtual void postProcess_(boost::shared_ptr<GOptimizationAlgorithmT<GParameterSet> >&);

private:
   /** @brief The default constructor. Intentionally private and undefined */
   GSimulatedAnnealingFactory();
};

/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */

#endif /* GSIMULATEDANNEALINGFACTORY_HPP_ */

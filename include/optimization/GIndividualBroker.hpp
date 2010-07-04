/**
 * @file GIndividualBroker.hpp
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

// Standard headers go here

#include <sstream>

// Includes check for correct Boost version(s)
#include "GGlobalDefines.hpp"

// Boost headers go here

#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/utility.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/thread.hpp>

#ifndef GINDIVIDUALBROKER_HPP_
#define GINDIVIDUALBROKER_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif


// GenEvA headers go here
#include "GBrokerT.hpp"
#include "GIndividual.hpp"

namespace Gem {
namespace GenEvA {

/**************************************************************************************/
/**
 * A broker class that is specialized on GIndividual objects
 */
class GIndividualBroker
	:public Gem::Util::GBrokerT<boost::shared_ptr<Gem::GenEvA::GIndividual> >
{
public:
	GIndividualBroker()
		:Gem::Util::GBrokerT<boost::shared_ptr<Gem::GenEvA::GIndividual> >()
	{ /* nothing */	}

	~GIndividualBroker()
	{
		std::cout << "GIndividualBroker has terminated" << std::endl;
	}
};

/**************************************************************************************/
/**
 * We require the global GIndividualBroker object to be a singleton. This
 * ensures that one and only one Broker object exists that is constructed
 * before main begins. All external communication should refer to GINDIVIDUALBROKER.
 */
#define GINDIVIDUALBROKER GBROKER(boost::shared_ptr<Gem::GenEvA::GIndividual>)

/**************************************************************************************/

} /* namespace GenEvA */
} /* namespace Gem */

#endif /* GINDIVIDUALBROKER_HPP_ */

/**
 * @file GConsumer.hpp
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

// Standard headers go here

#include <sstream>

// Includes check for correct Boost version(s)
#include "GGlobalDefines.hpp"

// Boost headers go here

#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/utility.hpp>
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/exception.hpp>
#include <boost/thread/mutex.hpp>

#ifndef GCONSUMER_HPP_
#define GCONSUMER_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif


// GenEvA headers go here

namespace Gem
{
namespace Util
{

/**************************************************************************************/
/**
 * This class forms the basis of a hierarchy of classes that take
 * objects from GBrokerT and process them, either locally or remotely.
 * Derived classes such as the GAsioTCPConsumer form the single point
 * of contact for remote clients. We do not want this class and its
 * derivatives to be copyable, hence we derive it from the
 * boost::noncopyable class. GConsumer::process() is started in a separate
 * thread by the broker. GConsumer::shutdown() is called by the broker
 * when the consumer is supposed to shut down.
 */
class GConsumer
	:private boost::noncopyable
{
public:
	/**********************************************************************************/
	/**
	 * The initialization of a consumer requires access to a brokero
	 */
	GConsumer()
	{ /* nothing */ }

	/**********************************************************************************/
	/**
	 * The standard destructor
	 */
	virtual ~GConsumer()
	{ /* nothing */ }

	/**********************************************************************************/
	/** @brief The actual business logic */
	virtual void process() = 0;
	/** @brief To be called from GConsumer::process() */
	virtual void shutdown() = 0;

private:
	GConsumer(const GConsumer&); ///< Intentionally left undefined
	const GConsumer& operator=(const GConsumer&); ///< Intentionally left undefined
};

/**************************************************************************************/

} /* namespace Util */
} /* namespace Gem */

#endif /*GCONSUMER_HPP_*/

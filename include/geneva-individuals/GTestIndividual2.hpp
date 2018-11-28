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
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <list>
#include <algorithm> // for std::sort

// Boost header files go here
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/cast.hpp>

// Geneva header files go here
#include "geneva/GParameterSet.hpp"
#include "geneva/GDoubleCollection.hpp"
#include "geneva/GDoubleObject.hpp"
#include "geneva/GDoubleGaussAdaptor.hpp"
#include "geneva/GConstrainedDoubleCollection.hpp"
#include "geneva/GConstrainedDoubleObjectCollection.hpp"
#include "geneva/GConstrainedDoubleObject.hpp"
#include "geneva/GOptimizationEnums.hpp"
#include "geneva/G_OptimizationAlgorithm_GradientDescent_PersonalityTraits.hpp"
#include "geneva/G_OptimizationAlgorithm_SwarmAlgorithm_PersonalityTraits.hpp"
#include "common/GExceptions.hpp"
#include "common/GCommonEnums.hpp"

namespace Gem {
namespace Tests {

/**
 * The types of objects to be tested in this class
 */
enum class PERFOBJECTTYPE : Gem::Common::ENUMBASETYPE {
	 PERFGDOUBLEOBJECT = 0,
	 PERFGCONSTRDOUBLEOBJECT = 1,
	 PERFGCONSTRAINEDDOUBLEOBJECTCOLLECTION = 2,
	 PERFGDOUBLECOLLECTION = 3,
	 PERFGCONSTRAINEDDOUBLECOLLECTION = 4
};

const PERFOBJECTTYPE POTMIN = PERFOBJECTTYPE::PERFGDOUBLEOBJECT;
const PERFOBJECTTYPE POTMAX = PERFOBJECTTYPE::PERFGCONSTRAINEDDOUBLEOBJECTCOLLECTION;
const std::size_t NPERFOBJECTTYPES = 5;

/******************************************************************************/
/** @brief Puts a Gem::Common::logType into a stream. Needed also for boost::lexical_cast<> */
G_API_INDIVIDUALS std::ostream &operator<<(std::ostream &, const Gem::Tests::PERFOBJECTTYPE&);
/** @brief Reads a Gem::Common::logType from a stream. Needed also for boost::lexical_cast<> */
G_API_INDIVIDUALS std::istream &operator>>(std::istream &, Gem::Tests::PERFOBJECTTYPE&);

/******************************************************************************/
/**
 * This individual serves as the basis for unit tests of the individual hierarchy. At the time
 * of writing, it was included in order to be able to set the individual's personality without
 * weakening data protection.
 */
class GTestIndividual2 : public Gem::Geneva::GParameterSet {
	 ///////////////////////////////////////////////////////////////////////
	 friend class boost::serialization::access;

	 template<typename Archive>
	 void serialize(Archive &ar, const unsigned int) {
		 using boost::serialization::make_nvp;

		 ar
		 & BOOST_SERIALIZATION_BASE_OBJECT_NVP(GParameterSet);
	 }
	 ///////////////////////////////////////////////////////////////////////

public:
	 /** @brief The default constructor */
	 G_API_INDIVIDUALS GTestIndividual2(const std::size_t &, const PERFOBJECTTYPE &);
	 /** @brief The copy constructor */
	 G_API_INDIVIDUALS GTestIndividual2(const GTestIndividual2 &);

	 /** @brief The standard destructor */
	 virtual G_API_INDIVIDUALS ~GTestIndividual2();

protected:
	 /** @brief Loads the data of another GTestIndividual2 */
	 virtual G_API_INDIVIDUALS void load_(const GObject *) final;

	/** @brief Allow access to this classes compare_ function */
	friend void Gem::Common::compare_base_t<GTestIndividual2>(
		GTestIndividual2 const &
		, GTestIndividual2 const &
		, Gem::Common::GToken &
	);

	/** @brief Searches for compliance with expectations with respect to another object of the same type */
	virtual G_API_INDIVIDUALS void compare_(
		const GObject & // the other object
		, const Gem::Common::expectation & // the expectation for this object, e.g. equality
		, const double & // the limit for allowed deviations of floating point types
	) const final;

	 /** @brief The actual fitness calculation takes place here. */
	 virtual G_API_INDIVIDUALS double fitnessCalculation() final;

private:
	 /** @brief Creates a deep clone of this object */
	 virtual G_API_INDIVIDUALS GObject *clone_() const final;

	 /** @brief The default constructor -- protected, as it is only needed for (de-)serialization purposes */
	 GTestIndividual2();

public:
	 /** @brief Applies modifications to this object. */
	 virtual G_API_INDIVIDUALS bool modify_GUnitTests();

	 /** @brief Performs self tests that are expected to succeed. */
	 virtual G_API_INDIVIDUALS void specificTestsNoFailureExpected_GUnitTests();

	 /** @brief Performs self tests that are expected to fail. */
	 virtual G_API_INDIVIDUALS void specificTestsFailuresExpected_GUnitTests();
};

/******************************************************************************/

} /* namespace Tests */
} /* namespace Gem */

BOOST_CLASS_EXPORT_KEY(Gem::Tests::GTestIndividual2)


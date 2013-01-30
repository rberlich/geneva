/**
 * @file GSAPersonalityTraits.hpp
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

// Boost headers go here

#ifndef GSAPERSONALITYTRAITS_HPP_
#define GSAPERSONALITYTRAITS_HPP_


// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif


// Geneva headers go here
#include "geneva/GBaseParChildPersonalityTraits.hpp"

namespace Gem {
namespace Geneva {

/******************************************************************************/
/**
 * This class allows to add variables and functions to GPersonalityTraits that
 * are specific to simulated annealing. Note that at the current time this class
 * adds no additional data. Since each optimization algorithm needs its own
 * personality type, though, we provide this default implementation. The base
 * class is the same as for evolutionary algorithms, as in Geneva Simulated Annealing
 * uses the same framework.
 */
class GSAPersonalityTraits
   : public GBaseParChildPersonalityTraits
{
   ///////////////////////////////////////////////////////////////////////
   friend class boost::serialization::access;

   template<typename Archive>
   void serialize(Archive & ar, const unsigned int){
     using boost::serialization::make_nvp;
     ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(GBaseParChildPersonalityTraits);
   }
   ///////////////////////////////////////////////////////////////////////

public:
   /** @brief The default constructor */
   GSAPersonalityTraits();
   /** @brief The copy contructor */
   GSAPersonalityTraits(const GSAPersonalityTraits&);
   /** @brief The standard destructor */
   virtual ~GSAPersonalityTraits();

   /** @brief A standard assignment operator */
   const GSAPersonalityTraits& operator=(const GSAPersonalityTraits&);

   /** @brief Checks for equality with another GSAPersonalityTraits object */
   bool operator==(const GSAPersonalityTraits&) const;
   /** @brief Checks for inequality with another GSAPersonalityTraits object */
   bool operator!=(const GSAPersonalityTraits&) const;

   /** @brief Checks whether this object fulfills a given expectation in relation to another object */
   virtual boost::optional<std::string> checkRelationshipWith(
         const GObject&
         , const Gem::Common::expectation&
         , const double&
         , const std::string&
         , const std::string&
         , const bool&
   ) const;

protected:
   /** @brief Loads the data of another GSAPersonalityTraits object */
   virtual void load_(const GObject*);
   /** @brief Creates a deep clone of this object */
   virtual GObject* clone_() const;

private:

public:
   /** @brief Applies modifications to this object. This is needed for testing purposes */
   virtual bool modify_GUnitTests();
   /** @brief Performs self tests that are expected to succeed. This is needed for testing purposes */
   virtual void specificTestsNoFailureExpected_GUnitTests();
   /** @brief Performs self tests that are expected to fail. This is needed for testing purposes */
   virtual void specificTestsFailuresExpected_GUnitTests();
};

/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */

BOOST_CLASS_EXPORT_KEY(Gem::Geneva::GSAPersonalityTraits)

#endif /* GSAPERSONALITYTRAITS_HPP_ */
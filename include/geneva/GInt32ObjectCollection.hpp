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

// Boost header files go here

// Geneva header files go here
#include "geneva/GInt32Object.hpp"
#include "geneva/GParameterTCollectionT.hpp"
#include "geneva/GInt32GaussAdaptor.hpp"

namespace Gem {
namespace Geneva {

/******************************************************************************/
/**
 * A collection of GInt32Object objects, ready for use in a
 * GParameterSet derivative.
 */
class GInt32ObjectCollection
    : public GParameterTCollectionT<GInt32Object>
{
    ///////////////////////////////////////////////////////////////////////
    friend class boost::serialization::access;

    template<typename Archive>
    void serialize(Archive &ar, const unsigned int) {
        using boost::serialization::make_nvp;

        ar
        & make_nvp(
            "GParameterTCollectionT_ioc"
            , boost::serialization::base_object<GParameterTCollectionT<GInt32Object>>(*this));
    }
    ///////////////////////////////////////////////////////////////////////

public:
    /** @brief The default constructor */
    G_API_GENEVA GInt32ObjectCollection() = default;
    /** @brief Initialization with a number of GInt32Object objects */
    G_API_GENEVA GInt32ObjectCollection(const std::size_t &, std::shared_ptr<GInt32Object>);
    /** @brief The copy constructor */
    G_API_GENEVA GInt32ObjectCollection(const GInt32ObjectCollection &) = default;
    /** @brief The destructor */
    G_API_GENEVA ~GInt32ObjectCollection() override = default;

protected:
    /** @brief Loads the data of another GObject */
    G_API_GENEVA void load_(const GObject *) override;

    /** @brief Allow access to this classes compare_ function */
    friend void Gem::Common::compare_base_t<GInt32ObjectCollection>(
        GInt32ObjectCollection const &
        , GInt32ObjectCollection const &
        , Gem::Common::GToken &
    );

    /** @brief Searches for compliance with expectations with respect to another object of the same type */
    G_API_GENEVA void compare_(
        const GObject & // the other object
        , const Gem::Common::expectation & // the expectation for this object, e.g. equality
        , const double & // the limit for allowed deviations of floating point types
    ) const override;

    /** @brief Applies modifications to this object. This is needed for testing purposes */
    G_API_GENEVA bool modify_GUnitTests_() override;
    /** @brief Performs self tests that are expected to succeed. This is needed for testing purposes */
    G_API_GENEVA void specificTestsNoFailureExpected_GUnitTests_() override;
    /** @brief Performs self tests that are expected to fail. This is needed for testing purposes */
    G_API_GENEVA void specificTestsFailuresExpected_GUnitTests_() override;

private:
    /** @brief Emits a name for this class / object */
    G_API_GENEVA std::string name_() const override;
    /** @brief Creates a deep clone of this object. */
    G_API_GENEVA GObject *clone_() const override;

    /** @brief Fills the collection with GInt32Object objects */
    G_API_GENEVA void fillWithObjects_(const std::size_t &);
};

/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */

BOOST_CLASS_EXPORT_KEY(Gem::Geneva::GInt32ObjectCollection)


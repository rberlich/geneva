/**
 * @file GDoubleGaussAdaptor.hpp
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

// Global checks, defines and includes needed for all of Geneva
#include "common/GGlobalDefines.hpp"

// Standard headers go here

// Boost headers go here

#ifndef GDOUBLEGAUSSADAPTOR_HPP_
#define GDOUBLEGAUSSADAPTOR_HPP_


// Geneva headers go here
#include "geneva/GFPGaussAdaptorT.hpp"

namespace Gem {
namespace Geneva {

/******************************************************************************/
/**
 * The GDoubleGaussAdaptor represents an adaptor used for the adaption of
 * double values through the addition of gaussian-distributed random numbers.
 * See the documentation of GNumGaussAdaptorT<T> for further information on adaptors
 * in the Geneva context. This class is at the core of evolutionary strategies,
 * as implemented by this library. It is now implemented through a generic
 * base class that can also be used to adapt other numeric types.
 */
class GDoubleGaussAdaptor
	:public GFPGaussAdaptorT<double>
{
	 ///////////////////////////////////////////////////////////////////////
	 friend class boost::serialization::access;

	 template<typename Archive>
	 void serialize(Archive & ar, const unsigned int){
		 using boost::serialization::make_nvp;

		 ar
		 & make_nvp("GFPGaussAdaptorT_double", boost::serialization::base_object<GFPGaussAdaptorT<double>>(*this));
	 }
	 ///////////////////////////////////////////////////////////////////////

public:
	 /** @brief The default constructor */
	 G_API_GENEVA GDoubleGaussAdaptor();
	 /** @brief The copy constructor */
	 G_API_GENEVA GDoubleGaussAdaptor(const GDoubleGaussAdaptor&);
	 /** @brief Initialization with a adaption probability */
	 explicit G_API_GENEVA GDoubleGaussAdaptor(const double&);
	 /** @brief Initialization with a number of values belonging to the width of the gaussian */
	 G_API_GENEVA GDoubleGaussAdaptor(
		 const double&
		 , const double&
		 , const double&
		 , const double&
	 );
	 /** @brief Initialization with a number of values belonging to the width of the gaussian and the adaption probability */
	 G_API_GENEVA GDoubleGaussAdaptor(
		 const double&
		 , const double&
		 , const double&
		 , const double&
		 , const double&
	 );
	 /** @brief The destructor */
	 virtual G_API_GENEVA ~GDoubleGaussAdaptor();

	 /** @brief The standard assignment operator */
	 G_API_GENEVA const GDoubleGaussAdaptor& operator=(const GDoubleGaussAdaptor&);

	 /** @brief Checks for equality with another GDoubleGaussAdaptor object */
	 G_API_GENEVA bool operator==(const GDoubleGaussAdaptor&) const;
	 /** @brief Checks for inequality with another GDoubleGaussAdaptor object */
	 G_API_GENEVA bool operator!=(const GDoubleGaussAdaptor&) const;

	 /** @brief Searches for compliance with expectations with respect to another object of the same type */
	 virtual G_API_GENEVA void compare(
		 const GObject& // the other object
		 , const Gem::Common::expectation& // the expectation for this object, e.g. equality
		 , const double& // the limit for allowed deviations of floating point types
	 ) const override;

	 /** @brief Retrieves the id of this adaptor */
	 virtual G_API_GENEVA Gem::Geneva::adaptorId getAdaptorId() const override;

	 /** @brief Emits a name for this class / object */
	 virtual G_API_GENEVA std::string name() const override;

protected:
	 /** @brief Loads the data of another GObject */
	 virtual G_API_GENEVA void load_(const GObject*) override;

private:
	 /** @brief Creates a deep clone of this object. */
	 virtual G_API_GENEVA GObject* clone_() const override;

public:
	 /** @brief Applies modifications to this object. This is needed for testing purposes */
	 virtual G_API_GENEVA bool modify_GUnitTests() override;
	 /** @brief Performs self tests that are expected to succeed. This is needed for testing purposes */
	 virtual G_API_GENEVA void specificTestsNoFailureExpected_GUnitTests() override;
	 /** @brief Performs self tests that are expected to fail. This is needed for testing purposes */
	 virtual G_API_GENEVA void specificTestsFailuresExpected_GUnitTests() override;
};

/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */

BOOST_CLASS_EXPORT_KEY(Gem::Geneva::GDoubleGaussAdaptor)

#endif /* GDOUBLEGAUSSADAPTOR_HPP_ */

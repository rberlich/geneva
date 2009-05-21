/**
 * @file GIntFlipAdaptorT.hpp
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
#include <cmath>
#include <string>
#include <sstream>
#include <utility> // For std::pair

// Includes check for correct Boost version(s)
#include "GGlobalDefines.hpp"

// Boost headers go here

#include <boost/cstdint.hpp>
#include <boost/limits.hpp>
#include <boost/cast.hpp>

#ifndef GINTFLIPADAPTORT_HPP_
#define GINTFLIPADAPTORT_HPP_

// GenEvA headers go here

#include "GAdaptorT.hpp"
#include "GBoundedDouble.hpp"
#include "GDoubleGaussAdaptor.hpp"
#include "GObject.hpp"
#include "GRandom.hpp"
#include "GenevaExceptions.hpp"
#include "GEnums.hpp"

namespace Gem {
namespace GenEvA {

/************************************************************************************************/
/**
 * GIntFlipAdaptorT represents an adaptor used for the mutation of integer
 * types, by flipping an integer number to the next larger or smaller number
 * with a given probability. The integer type used needs to be specified as
 * a template parameter. Note that a specialization of this class, as defined
 * in GIntFlipAdaptorT.cpp, allows to deal with booleans instead of "standard"
 * integer types.
 */
template<typename T>
class GIntFlipAdaptorT:
	public GAdaptorT<T>
{
	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int) {
		using boost::serialization::make_nvp;
		ar & make_nvp("GAdaptorT", boost::serialization::base_object<GAdaptorT<T> >(*this));
		ar & make_nvp("mutProb_", mutProb_);
	}
	///////////////////////////////////////////////////////////////////////

public:
	/********************************************************************************************/
	/**
	 * The standard constructor. It passes the adaptor's standard name to the
	 * parent class and initializes the internal variables.
	 */
	GIntFlipAdaptorT()
		:GAdaptorT<T> (),
		 mutProb_(DEFAULTMUTPROB, 0., 1.) // probability is in the range [0:1[
	{
		// Uses whatever GDoubleGaussAdaptor believes should be the default values
		boost::shared_ptr<GAdaptorT<double> > gaussAdaptor(new GDoubleGaussAdaptor());
		mutProb_.addAdaptor(gaussAdaptor);
	}

	/********************************************************************************************/
	/**
	 * This constructor takes an argument, that specifies the (initial) probability
	 * for the mutation of a bit value.
	 *
	 * @param prob The probability for a bit-flip
	 */
	explicit GIntFlipAdaptorT(const double& prob)
		:GAdaptorT<T>(),
		 mutProb_(prob, 0., 1.) // probability is in the range [0:1]
	{
		// Uses whatever GDoubleGaussAdaptor believes should be the default values
		boost::shared_ptr<GAdaptorT<double> > gaussAdaptor(new GDoubleGaussAdaptor());
		gaussAdaptor->setRnrGenerationMode(Gem::Util::RNRFACTORY);
		mutProb_.addAdaptor(gaussAdaptor);
	}

	/********************************************************************************************/
	/**
	 * A standard copy constructor.
	 *
	 * @param cp Another GIntFlipAdaptorT object
	 */
	GIntFlipAdaptorT(const GIntFlipAdaptorT<T>& cp)
		:GAdaptorT<T>(cp),
		 mutProb_(cp.mutProb_)
	{ /* nothing */	}

	/********************************************************************************************/
	/**
	 * The standard destructor. Empty, as we have no local, dynamically
	 * allocated data.
	 */
	~GIntFlipAdaptorT()
	{ /* nothing */ }

	/********************************************************************************************/
	/**
	 * A standard assignment operator for GIntFlipAdaptorT objects,
	 *
	 * @param cp A copy of another GIntFlipAdaptorT object
	 */
	const GIntFlipAdaptorT<T>& operator=(const GIntFlipAdaptorT<T>& cp)
	{
		GIntFlipAdaptorT<T>::load(&cp);
		return *this;
	}

	/********************************************************************************************/
	/**
	 * This function loads the data of another GIntFlipAdaptorT, camouflaged as a GObject.
	 *
	 * @param A copy of another GIntFlipAdaptorT, camouflaged as a GObject
	 */
	void load(const GObject *cp)
	{
		// Convert GObject pointer to local format
		const GIntFlipAdaptorT<T> *gifa = this->conversion_cast(cp, this);

		// Load the data of our parent class ...
		GAdaptorT<T>::load(cp);

		// ... and then our own
		mutProb_ = gifa->mutProb_;
	}


	/********************************************************************************************/
	/**
	 * This function creates a deep copy of this object
	 *
	 * @return A deep copy of this object
	 */
	GObject *clone() const
	{
		return new GIntFlipAdaptorT<T>(*this);
	}

	/********************************************************************************************/
	/**
	 * Checks for equality with another GIntFlipAdaptorT<T> object
	 *
	 * @param  cp A constant reference to another GIntFlipAdaptorT<T> object
	 * @return A boolean indicating whether both objects are equal
	 */
	bool operator==(const GIntFlipAdaptorT<T>& cp) const {
		return GIntFlipAdaptorT<T>::isEqualTo(cp, boost::logic::indeterminate);
	}

	/********************************************************************************************/
	/**
	 * Checks for inequality with another GIntFlipAdaptorT<T> object
	 *
	 * @param  cp A constant reference to another GIntFlipAdaptorT<T> object
	 * @return A boolean indicating whether both objects are inequal
	 */
	bool operator!=(const GIntFlipAdaptorT<T>& cp) const {
		return !GIntFlipAdaptorT<T>::isEqualTo(cp, boost::logic::indeterminate);
	}

	/********************************************************************************************/
	/**
	 * Checks for equality with another GIntFlipAdaptorT<T> object Equality means
	 * that all individual sub-values are equal and that the parent class is equal.
	 *
	 * @param  cp A constant reference to another GIntFlipAdaptorT<T> object
	 * @return A boolean indicating whether both objects are equal
	 */
	virtual bool isEqualTo(const GObject& cp, const boost::logic::tribool& expected = boost::logic::indeterminate) const {
	    using namespace Gem::Util;

		// Check that we are indeed dealing with a GIntFlipAdaptorT reference
		const GIntFlipAdaptorT<T> *gifat_load = GObject::conversion_cast(&cp,  this);

		// Check our parent class
		if(!GAdaptorT<T>::isEqualTo(*gifat_load, expected)) return false;

		// And then our local data
		if(!mutProb_.isEqualTo(gifat_load->mutProb_, expected)) 	return false;

		return true;
	}

	/********************************************************************************************/
	/**
	 * Checks for similarity with another GIntFlipAdaptorT<T> object. Similarity means
	 * that all double values are similar to each other within a given limit and that all other
	 * values are equal. Also, parent classes must be similar to each other.
	 *
	 * @param  cp A constant reference to another GIntFlipAdaptorT<T> object
	 * @param limit A double value specifying the acceptable level of differences of floating point values
	 * @return A boolean indicating whether both objects are similar to each other
	 */
	virtual bool isSimilarTo(const GObject& cp, const double& limit, const boost::logic::tribool& expected = boost::logic::indeterminate) const {
	    using namespace Gem::Util;

		// Check that we are indeed dealing with a GIntFlipAdaptorT reference
		const GIntFlipAdaptorT<T> *gifat_load = GObject::conversion_cast(&cp,  this);

		// First check our parent class
		if(!GAdaptorT<T>::isSimilarTo(*gifat_load, limit, expected))  return false;

		// Then our local data
		if(!mutProb_.isSimilarTo(gifat_load->mutProb_, limit, expected)) return false;

		return true;
	}

	/********************************************************************************************/
	/**
	 * Determines whether production of random numbers should happen remotely
	 * (RNRFACTORY) or locally (RNRLOCAL). This function is one of the rare cases where it is
	 * necessary to re-implent this function. We want to propagate the random number generation
	 * mode to our local mutProb_ object's adaptor.
	 *
	 * @param rnrGenMode A parameter which indicates where random numbers should be produced
	 */
	virtual void setRnrGenerationMode(const Gem::Util::rnrGenerationMode& rnrGenMode) {
		// Set the parent number's mode
		GAdaptorT<T>::setRnrGenerationMode(rnrGenMode);

		// Set mutProb_'s adaptor's generation mode
		boost::shared_ptr<GDoubleGaussAdaptor> gaussAdaptor
					= mutProb_.adaptor_cast<GDoubleGaussAdaptor>();
		gaussAdaptor->setRnrGenerationMode(rnrGenMode);
	}

	/********************************************************************************************/
	/**
	 * Retrieves the current value of the mutation probability
	 *
	 * @return The current value of the mutation probability
	 */
	double getMutationProbability() {
		return mutProb_.value();
	}

	/*************************************************************************/
	/**
	 * Sets the mutation probability to a given value. This function will throw
	 * if the probability is not in the allowed range.
	 *
	 * @param val The new value of the probability for integer flips
	 */
	void setMutationProbability(const double& probability) {
		// Check the supplied probability value
		if(probability < 0. || probability > 1.) {
			std::ostringstream error;
			error << "In GIntFlipAdaptor::setMutationProbability(double) : Error!" << std::endl
				  << "Bad probability value given: " << probability << std::endl;

			// throw an exception. Add some information so that if the exception
			// is caught through a base object, no information is lost.
			throw geneva_error_condition(error.str());
		}

		mutProb_ = probability;
	}

	/********************************************************************************************/
	/**
	 * The mutation of a GDouble object has a number of parameters that can
	 * be set with this function. See the documentation of the GDouble class
	 * for further information.
	 *
	 * @param sgm Sigma for gauss mutation
	 * @param sgmSgm Parameter which determines the amount of evolutionary adaption of sigma
	 * @param minSgm The minimal value sigma may assume
	 * @param maxSgm The maximim value sigma may assume
	 */
	void setMutationParameters(const double& sgm, const double& sgmSgm,
							   const double& minSgm, const double& maxSgm) {
		boost::shared_ptr<GDoubleGaussAdaptor> gaussAdaptor
			= mutProb_.adaptor_cast<GDoubleGaussAdaptor>();
		// Then set the values as requested.
		gaussAdaptor->setAll(sgm,sgmSgm,minSgm,maxSgm);
	}

	/********************************************************************************************/
	/**
	 * Retrieves the id of the adaptor.
	 *
	 * @return The id of the adaptor
	 */
	virtual Gem::GenEvA::adaptorId getAdaptorId() const {
		std::ostringstream error;
		error << "In Gem::GenEvA::adaptorId GIntFlipAdaptorT::getAdaptorId(): Error!" << std::endl
			  << "Function used with a type it was not designed for" << std::endl;
		throw (Gem::GenEvA::geneva_error_condition(error.str()));
	}

protected:
	/********************************************************************************************/
	/**
	 * The mutation probability is implemented as a GDouble. It thus can take
	 * care of its own mutation within its boundaries [0.,1.] .
	 */
	void adaptMutation() {
		mutProb_.mutate();
	}

	/********************************************************************************************/
	/**
	 * We want to flip the value only in a given percentage of cases. Thus
	 * we calculate a probability between 0 and 1 and compare it with the desired
	 * mutation probability. Please note that evenRandom returns a value in the
	 * range of [0,1[, so we make a tiny error here. This function assumes
	 * an integer type. It hence flips the value up or down. A specialization
	 * for booleans is provided in GIntFlipAdaptorT.cpp .
	 *
	 * @param value The bit value to be mutated
	 */
	virtual void customMutations(T& value) {
		double probe = this->gr.evenRandom(0.,1.);
		if(probe < mutProb_.value()) {
			bool up = this->gr.boolRandom();
			if(up){
#if defined (CHECKOVERFLOWS) || defined (DEBUG)
				if(std::numeric_limits<T>::max() == value) value -= 1;
				else value += 1;
#else
				value += 1;
#endif
			}
			else {
#if defined (CHECKOVERFLOWS) || defined (DEBUG)
				if(std::numeric_limits<T>::min() == value) value += 1;
				else value -= 1;
#else
				value -= 1;
#endif
			}
		}
	}

private:
	/********************************************************************************************/

	GBoundedDouble mutProb_; ///< internal representation of the mutation probability
};

// Declaration of some specializations
template<> void GIntFlipAdaptorT<bool>::customMutations(bool&);
template<> void GIntFlipAdaptorT<short>::customMutations(short&);
template<> void GIntFlipAdaptorT<unsigned short>::customMutations(unsigned short&);
template<> void GIntFlipAdaptorT<boost::uint8_t>::customMutations(boost::uint8_t&);
template<> void GIntFlipAdaptorT<boost::int8_t>::customMutations(boost::int8_t&);
template<> void GIntFlipAdaptorT<char>::customMutations(char&);
template<> Gem::GenEvA::adaptorId GIntFlipAdaptorT<bool>::getAdaptorId() const;
template<> Gem::GenEvA::adaptorId GIntFlipAdaptorT<boost::int32_t>::getAdaptorId() const;
template<> Gem::GenEvA::adaptorId GIntFlipAdaptorT<char>::getAdaptorId() const;

} /* namespace GenEvA */
} /* namespace Gem */

#endif /* GINTFLIPADAPTORT_HPP_ */

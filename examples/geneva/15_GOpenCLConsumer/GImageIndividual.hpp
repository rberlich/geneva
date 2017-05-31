/**
 * @file GImageIndividual.hpp
 */

/*
 * Copyright (C) Gemfony scientific UG (haftungsbeschraenkt)
 *
 * See the AUTHORS file in the top-level directory for a list of authors.
 *
 * Contact: contact [at] gemfony (dot) com
 *
 * This file is part of the Geneva/P library collection. No part of
 * this code may be distributed without prior, written consent by
 * Gemfony scientific.
 */

// Standard header files go here

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <iterator>
#include <cfloat>
#include <cmath>
#include <list>
#include <algorithm> // for std::sort
#include <utility> // For std::pair
#include <tuple>
#include <memory>

// Includes check for correct Boost version(s)
#include "common/GGlobalDefines.hpp"

// Boost header files go here
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/cast.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/cstdint.hpp>

#ifndef GIMAGEINDIVIDUAL_HPP_
#define GIMAGEINDIVIDUAL_HPP_

// Geneva header files go here
#include "geneva/GParameterSet.hpp"
#include "geneva/GDoubleGaussAdaptor.hpp"
#include "geneva/GConstrainedDoubleObject.hpp"
#include "common/GExceptions.hpp"
#include "common/GGlobalOptionsT.hpp"
#include "common/GHelperFunctionsT.hpp"
#include "common/GSingletonT.hpp"
#include "common/GParserBuilder.hpp"
#include "common/GFactoryT.hpp"
#include "common/GMathHelperFunctions.hpp"
#include "common/GMathHelperFunctionsT.hpp"
#include "common/GCanvas.hpp"

namespace Gem {
namespace Geneva {

const std::size_t GII_DEF_NTRIANGLES = std::size_t(300);
const double GII_DEF_STARTSIZE = 0.;
const double GII_DEF_MINSIZE = 0.;
const double GII_DEF_MAXSIZE = 0.3;
const double GII_DEF_MINOPAQUENESS = 0.3;
const double GII_DEF_MAXOPAQUENESS = 0.6;
const double GII_DEF_ADPROB = 0.05;
const double GII_DEF_ADAPTADPROB = 0.1;
const double GII_DEF_MINADPROB = 0.05;
const double GII_DEF_MAXADPROB = 1.;
const double GII_DEF_SIGMA = 0.1;
const double GII_DEF_SIGMASIGMA = 0.8;
const double GII_DEF_MINSIGMA = 0.;
const double GII_DEF_MAXSIGMA = 0.2;
const double GII_DEF_LOC_MINADPROB = 0.05;
const double GII_DEF_LOC_MAXADPROB = 1.;
const double GII_DEF_LOC_SIGMA = 0.1;
const double GII_DEF_LOC_SIGMASIGMA = 0.8;
const double GII_DEF_LOC_MINSIGMA = 0.;
const double GII_DEF_LOC_MAXSIGMA = 0.2;
const std::size_t GII_DEF_COLORDEPTH = 8;
const std::size_t GII_DEF_NCOLORS    = Gem::Common::PowSmallPosInt<2,GII_DEF_COLORDEPTH>::result;
const std::size_t GII_DEF_MAXCOLOR   = GII_DEF_NCOLORS - 1;
const double GII_DEF_BGRED = 0.9;
const double GII_DEF_BGGREEN = 0.9;
const double GII_DEF_BGBLUE = 0.9;
const bool GII_DEF_ALPHASORT=true;

typedef std::tuple<std::size_t, std::size_t> SCREENSIZETYPE;

/******************************************************************************/
/**
 * This individual searches for a matching set of triangles
 * that most closely resembles a given picture. This individual
 * was developed for evaluation using OpenCL on a GPU and uses
 * OpenCL data types. It is meant to be used with a Consumer type
 * that understands how to talk to the GPU.
 */
class GImageIndividual
   : public Gem::Geneva::GParameterSet
{
   ///////////////////////////////////////////////////////////////////////
   friend class boost::serialization::access;

   template<typename Archive>
   void serialize(Archive & ar, const unsigned int) {
      using boost::serialization::make_nvp;

      ar
      & BOOST_SERIALIZATION_BASE_OBJECT_NVP(GParameterSet)
      & BOOST_SERIALIZATION_NVP(adProb_)
      & BOOST_SERIALIZATION_NVP(adaptAdProb_)
      & BOOST_SERIALIZATION_NVP(minAdProb_)
      & BOOST_SERIALIZATION_NVP(maxAdProb_)
      & BOOST_SERIALIZATION_NVP(bgRed_)
      & BOOST_SERIALIZATION_NVP(bgGreen_)
      & BOOST_SERIALIZATION_NVP(bgBlue_)
      & BOOST_SERIALIZATION_NVP(nTriangles_)
      & BOOST_SERIALIZATION_NVP(alphaSort_);
   }
   ///////////////////////////////////////////////////////////////////////

public:
   /******************************************************************************/
   /** @brief The default constructor */
   GImageIndividual();
   /** @brief A standard copy constructor */
   GImageIndividual(const GImageIndividual&);
   /** @brief The standard destructor */
   virtual ~GImageIndividual();

   /** @brief Fills the object with parameters */
   void init(
		const std::size_t& nTriangles
		, const double& bgRed
		, const double& bgGreen
		, const double& bgBlue
		, const double& startSize
		, const double& minSize
		, const double& maxSize
		, const double& minOpaqueness
		, const double& maxOpaqueness
		, const bool&   alphaSort
		, const double& sigma
		, const double& sigmaSigma
		, const double& minSigma
		, const double& maxSigma
		, const double& adProb
		, const double& adaptAdProb
		, const double& minAdProb
		, const double& maxAdProb
	);

   /** @brief A standard assignment operator */
   const GImageIndividual& operator=(const GImageIndividual&);

   /** @brief Checks for equality with another GImageIndividual object */
   bool operator==(const GImageIndividual& cp) const;
   /** @brief Checks for inequality with another GImageIndividual object */
   bool operator!=(const GImageIndividual& cp) const;

	/** @brief Searches for compliance with expectations with respect to another object of the same type */
	 virtual void compare(
		 const GObject&
		 , const Gem::Common::expectation&
		 , const double&
	 ) const override;

   /** @brief Retrieves the number of triangles */
   std::size_t getNTriangles() const;
   /** @brief Retrieves an array with the triangle data, using the circular triangle definition */
	std::vector<Gem::Common::t_circle> getTriangleData() const;
   /** @brief Retrieves the background colors */
   std::tuple<float,float,float> getBackGroundColor() const;

   /** @brief Converts the triangle data into a GCanvas object */
   std::shared_ptr<Gem::Common::GCanvas<GII_DEF_COLORDEPTH> > toCanvas(const std::tuple<std::size_t, std::size_t>&) const;

   /** @brief Writes an image with the current setup to disc */
   void writeImage(
      const std::string& = std::string("image_")
      , const std::string& = std::string("./")
      , const std::tuple<std::size_t, std::size_t>& = SCREENSIZETYPE(1024,768)
   );

protected:
   /******************************************************************************/
   /** @brief Loads the data of another GImageIndividual */
   virtual void load_(const GObject*) override;
   /** @brief Creates a deep clone of this object */
   virtual GObject* clone_() const override;

   /** @brief The actual fitness calculation takes place here. */
   virtual double fitnessCalculation() override;

private:
   /******************************************************************************/
   // Parameters

   double adProb_; ///< The adaption probability
   double adaptAdProb_; ///< The "strength" of adProb-adaption
   double minAdProb_; ///< The minimum allowed value for adProb_
   double maxAdProb_; ///< The maximum allowed value for adProb_

   double bgRed_;   ///< The red channel of the background color
   double bgGreen_; ///< The green channel of the background color
   double bgBlue_;  ///< The blue channel of the background color

   std::size_t nTriangles_; ///< The number of triangles
   bool alphaSort_; ///< Indicates whether triangles should be sorted according to their alpha channel

public:
   /** @brief Applies modifications to this object. */
   virtual bool modify_GUnitTests() override;
   /** @brief Performs self tests that are expected to succeed. */
   virtual void specificTestsNoFailureExpected_GUnitTests() override;
   /** @brief Performs self tests that are expected to fail. */
   virtual void specificTestsFailuresExpected_GUnitTests() override;
};

/******************************************************************************/
//////////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * A factory for GImageIndividual objects
 */
class GImageIndividualFactory
: public Gem::Common::GFactoryT<GImageIndividual>
{
public:
 	/** @brief The default constructor. Intentionally undefined */
 	GImageIndividualFactory() = delete;
   /** @brief The standard constructor */
   GImageIndividualFactory(const std::string&);
   /** @brief The destructor */
   virtual ~GImageIndividualFactory();

   // Some getters for parsed variables
   double getStartSize() const;
   double getAdProb() const;
   double getAdaptAdProb() const;
   void setAdaptAdProb(double adaptAdProb);
   std::tuple<double,double> getAdProbRange() const;
   void setAdProbRange(double minAdProb, double maxAdProb);
   double getMaxOpaqueness() const;
   double getMaxSigma() const;
   double getMaxSize() const;
   double getMinOpaqueness() const;
   double getMinSigma() const;
   double getMinSize() const;
   double getSigma() const;
   double getSigmaSigma() const;
   double getBGRed() const;
   double getBGGreen() const;
   double getBGBlue() const;
   std::size_t getNTriangles() const;
   bool getAlphaSort() const;

protected:
   /** @brief Creates individuals of this type */
   virtual std::shared_ptr<GImageIndividual> getObject_(Gem::Common::GParserBuilder&, const std::size_t&);
   /** @brief Allows to describe local configuration options in derived classes */
   virtual void describeLocalOptions_(Gem::Common::GParserBuilder&);
   /** @brief Allows to act on the configuration options received from the configuration file */
   virtual void postProcess_(std::shared_ptr<GImageIndividual>&);

private:
   Gem::Common::GOneTimeRefParameterT<double> startSize_;
   Gem::Common::GOneTimeRefParameterT<double> minSize_;
   Gem::Common::GOneTimeRefParameterT<double> maxSize_;
   Gem::Common::GOneTimeRefParameterT<double> minOpaqueness_;
   Gem::Common::GOneTimeRefParameterT<double> maxOpaqueness_;
   Gem::Common::GOneTimeRefParameterT<double> sigma_;
   Gem::Common::GOneTimeRefParameterT<double> sigmaSigma_;
   Gem::Common::GOneTimeRefParameterT<double> minSigma_;
   Gem::Common::GOneTimeRefParameterT<double> maxSigma_;
   Gem::Common::GOneTimeRefParameterT<double> adProb_;
   Gem::Common::GOneTimeRefParameterT<double> adaptAdProb_;
   Gem::Common::GOneTimeRefParameterT<double> minAdProb_;
   Gem::Common::GOneTimeRefParameterT<double> maxAdProb_;
   Gem::Common::GOneTimeRefParameterT<double> bgRed_;
   Gem::Common::GOneTimeRefParameterT<double> bgGreen_;
   Gem::Common::GOneTimeRefParameterT<double> bgBlue_;
   Gem::Common::GOneTimeRefParameterT<std::size_t> nTriangles_;
   Gem::Common::GOneTimeRefParameterT<bool> alphaSort_;
};

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */

#ifdef GEM_TESTING

template <> std::shared_ptr<Gem::Geneva::GImageIndividual> TFactory_GUnitTests<Gem::Geneva::GImageIndividual>();

#endif /* GEM_TESTING */

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/

BOOST_CLASS_EXPORT_KEY(Gem::Geneva::GImageIndividual)

#endif /* GIMAGEINDIVIDUAL_HPP_ */
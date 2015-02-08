/**
 * @file GParameterTCollectionT.hpp
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

// Standard header files go here

// Boost header files go here

#ifndef GPARAMETERTCOLLECTIONT_HPP_
#define GPARAMETERTCOLLECTIONT_HPP_

// Geneva header files go here
#include "common/GHelperFunctionsT.hpp"
#include "geneva/GParameterBase.hpp"
#include "geneva/GParameterT.hpp"
#include "geneva/GStdPtrVectorInterfaceT.hpp"

namespace Gem {
namespace Geneva {

/******************************************************************************/
/**
 * This class shares many similarities with the GParameterCollectionT class. Instead
 * of individual values that can be modified with adaptors, however, it assumes that
 * the objects stored in it have their own adapt() function. This class has been designed
 * as a collection of GParameterT objects, hence the name.  As an example, one can create a
 * collection of GConstrainedDoubleObject objects with this class rather than a simple GDoubleCollection.
 * In order to facilitate memory management, the GParameterT objects are stored
 * in boost::shared_ptr objects.
 */
template<typename T>
class GParameterTCollectionT
	: public GParameterBase
	, public GStdPtrVectorInterfaceT<T>
{
	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int) {
		using boost::serialization::make_nvp;

		// Save the data
		ar
		& make_nvp("GParameterBase", boost::serialization::base_object<GParameterBase>(*this))
		& make_nvp("GStdPtrVectorInterfaceT_T", boost::serialization::base_object<GStdPtrVectorInterfaceT<T> >(*this));
	}
	///////////////////////////////////////////////////////////////////////

	// Make sure T is a derivative of GParameterBase
	BOOST_MPL_ASSERT((boost::is_base_of<GParameterBase, T>));

public:
	/***************************************************************************/
	/**
	 * Allows to find out which type is stored in this class
	 */
	typedef T collection_type;

	/***************************************************************************/
	/**
	 * The default constructor
	 */
	GParameterTCollectionT()
	{ /* nothing */ }

	/***************************************************************************/
	/**
	 * Initialization with a number of copies of a given GParameterBase derivative
	 *
	 * @param nCp The amount of copies of the GParameterBase derivative to be stored in this object
	 * @param tmpl_ptr The object that serves as the template of all others
	 */
	GParameterTCollectionT(
	      const std::size_t& nCp
	      , boost::shared_ptr<T> tmpl_ptr
	) {
		for(std::size_t i=0; i<nCp; i++) {
			this->push_back(tmpl_ptr->template clone<T>());
		}
	}

	/***************************************************************************/
	/**
	 * The copy constructor
	 *
	 * @param cp A copy of another GParameterTCollectionT<T> object
	 */
	GParameterTCollectionT(const GParameterTCollectionT<T>& cp)
		: GParameterBase(cp)
		, GStdPtrVectorInterfaceT<T>(cp)
	{ /* nothing */ }

	/***************************************************************************/
	/**
	 * The standard destructor
	 */
	virtual ~GParameterTCollectionT()
	{ /* nothing */ }

	/***************************************************************************/
	/**
	 * A standard assignment operator.
	 *
	 * @param cp A copy of another GParameterTCollectionT<T> object
	 * @return A constant reference to this object
	 */
	const GParameterTCollectionT<T>& operator=(const GParameterTCollectionT<T>& cp)
	{
		GParameterTCollectionT<T>::load_(&cp);
		return *this;
	}

	/***************************************************************************/
	/**
	 * Checks for equality with another GParameterTCollectionT<T> object
	 *
	 * @param  cp A constant reference to another GParameterTCollectionT<T> object
	 * @return A boolean indicating whether both objects are equal
	 */
	bool operator==(const GParameterTCollectionT<T>& cp) const {
		using namespace Gem::Common;
		// Means: The expectation of equality was fulfilled, if no error text was emitted (which converts to "true")
		return !checkRelationshipWith(cp, CE_EQUALITY, 0.,"GParameterTCollectionT<T>::operator==","cp", CE_SILENT);
	}

	/***************************************************************************/
	/**
	 * Checks for inequality with another GParameterTCollectionT<T> object
	 *
	 * @param  cp A constant reference to another GParameterTCollectionT<T> object
	 * @return A boolean indicating whether both objects are inequal
	 */
	bool operator!=(const GParameterTCollectionT<T>& cp) const {
		using namespace Gem::Common;
		// Means: The expectation of inequality was fulfilled, if no error text was emitted (which converts to "true")
		return !checkRelationshipWith(cp, CE_INEQUALITY, 0.,"GParameterTCollectionT<T>::operator==","cp", CE_SILENT);
	}

	/***************************************************************************/
	/**
	 * Checks whether a given expectation for the relationship between this object and another object
	 * is fulfilled.
	 *
	 * @param cp A constant reference to another object, camouflaged as a GObject
	 * @param e The expected outcome of the comparison
	 * @param limit The maximum deviation for floating point values (important for similarity checks)
	 * @param caller An identifier for the calling entity
	 * @param y_name An identifier for the object that should be compared to this one
	 * @param withMessages Whether or not information should be emitted in case of deviations from the expected outcome
	 * @return A boost::optional<std::string> object that holds a descriptive string if expectations were not met
	 */
	boost::optional<std::string> checkRelationshipWith(
      const GObject& cp
      , const Gem::Common::expectation& e
      , const double& limit
      , const std::string& caller
      , const std::string& y_name
      , const bool& withMessages
	) const OVERRIDE {
	    using namespace Gem::Common;

		// Check that we are indeed dealing with a GParamterBase reference
		const GParameterTCollectionT<T>  *p_load = GObject::gobject_conversion<GParameterTCollectionT<T> >(&cp);

		// Will hold possible deviations from the expectation, including explanations
	    std::vector<boost::optional<std::string> > deviations;

		// Check our parent classes' data ...
		deviations.push_back(GParameterBase::checkRelationshipWith(cp, e, limit, "GParameterTCollectionT<T>", y_name, withMessages));
		deviations.push_back(GStdPtrVectorInterfaceT<T>::checkRelationshipWith(*p_load, e, limit, "GParameterTCollectionT<T>", y_name, withMessages));

		// no local data ...

		return evaluateDiscrepancies("GParameterTCollectionT<T>", caller, deviations, e);
	}

	/***************************************************************************/
	/**
	 * Allows to adapt the values stored in this class. We assume here that
	 * each item has its own adapt function. Hence we do not need to use or
	 * store own adaptors.
	 *
	 * @return The number of adaptions that were carried out
	 */
	virtual std::size_t adaptImpl() OVERRIDE {
	   std::size_t nAdapted = 0;

		typename GParameterTCollectionT<T>::iterator it;
		for(it=this->begin(); it!=this->end(); ++it) {
			nAdapted += (*it)->adapt();
		}

		return nAdapted;
	}

	/* ----------------------------------------------------------------------------------
	 * Tested in GDoubleObjectCollection::specificTestsNoFailureExpected_GUnitTests()
	 * ----------------------------------------------------------------------------------
	 */

	/***************************************************************************/
	/**
	 * Allows to identify whether we are dealing with a collection or an individual parameter
	 * (which is obviously not the case here). This function needs to be overloaded for parameter
	 * collections so that its inverse (GParameterBase::isParameterCollection() ) returns the
	 * correct value.
	 *
	 * @return A boolean indicating whether this GParameterBase-derivative is an individual parameter
	 */
	virtual bool isIndividualParameter() const OVERRIDE {
		return false;
	}

	/***************************************************************************/
	/**
	 * Assigns a random number generator from another object to all objects stored in this
	 * collection and to the object itself.
	 *
	 * @param gr_cp A reference to another object's GRandomBase object derivative
	 */
	virtual void assignGRandomPointer(Gem::Hap::GRandomBase *gr_cp) OVERRIDE {
		// Do some error checking
		if(!gr_cp) {
		   glogger
		   << "In GParameterTCollectionT<T>::assignGRandomPointer():" << std::endl
         << "Tried to assign a NULL pointer" << std::endl
         << GEXCEPTION;
		}

		// Assign the foreign pointer to all objects stored in this collection
		typename GParameterTCollectionT<T>::iterator it;
		for(it=this->begin(); it!=this->end(); ++it) {
			(*it)->assignGRandomPointer(gr_cp);
		}

		// Assign the foreign pointer to this object as well
		GParameterBase::assignGRandomPointer(gr_cp);
	}

	/* ----------------------------------------------------------------------------------
	 * Tested in GParameterTCollectionT<T>::specificTestsNoFailureExpected_GUnitTests()
	 * Throw tested in GParameterTCollectionT<T>::specificTestsFailuresExpected_GUnitTests()
	 * ----------------------------------------------------------------------------------
	 */

	/***************************************************************************/
	/**
	 * Re-connects the local random number generator to gr and distributes the call
	 * to all objects contained in this collection class.
	 */
	virtual void resetGRandomPointer() OVERRIDE {
		// Reset all objects stored in this collection
		typename GParameterTCollectionT<T>::iterator it;
		for(it=this->begin(); it!=this->end(); ++it) {
			(*it)->resetGRandomPointer();
		}

		// Reset our parent class
		GParameterBase::resetGRandomPointer();
	}

	/* ----------------------------------------------------------------------------------
	 * Tested in GParameterTCollectionT<T>::specificTestsNoFailureExpected_GUnitTests()
	 * ----------------------------------------------------------------------------------
	 */

	/***************************************************************************/
	/**
	 * Checks whether solely the local random number generator is used. The function returns
	 * false if at least one component of this class does not use a local random number
	 * generator
	 *
	 * @bool A boolean indicating whether solely the local random number generator is used
	 */
	virtual bool usesLocalRNG() const OVERRIDE {
		bool result = true;

		// Check all components of this class
		typename GParameterTCollectionT<T>::const_iterator it;
		for(it=this->begin(); it!=this->end(); ++it) {
			if(!(*it)->usesLocalRNG()) result = false;
		}

		// Check our parent class
		if(!GParameterBase::usesLocalRNG()) result = false;

		return result;
	}

	/* ----------------------------------------------------------------------------------
	 * Tested in GParameterTCollectionT<T>::specificTestsNoFailureExpected_GUnitTests()
	 * ----------------------------------------------------------------------------------
	 */

	/***************************************************************************/
	/**
	 * Checks whether all relevant objects use the assigned random number generator.
	 *
	 * @return A boolean indicating whether an assigned random number generator is used
	 */
	virtual bool assignedRNGUsed() const OVERRIDE {
		bool result = true;

		// Check all components of this class
		typename GParameterTCollectionT<T>::const_iterator it;
		for(it=this->begin(); it!=this->end(); ++it) {
			if(!(*it)->assignedRNGUsed()) result = false;
		}

		// Check our parent class
		if(!GParameterBase::assignedRNGUsed()) result = false;

		return result;
	}

	/* ----------------------------------------------------------------------------------
	 * Tested in GParameterTCollectionT<T>::specificTestsNoFailureExpected_GUnitTests()
	 * ----------------------------------------------------------------------------------
	 */

   /***************************************************************************/
   /**
    * Emits a name for this class / object
    */
   virtual std::string name() const OVERRIDE {
      return std::string("GParameterTCollectionT");
   }

   /***************************************************************************/
   /**
    * Triggers updates when the optimization process has stalled
    */
   virtual bool updateAdaptorsOnStall(const std::size_t& nStalls) OVERRIDE {
      bool updatePerformed = false;

      typename GParameterTCollectionT<T>::iterator it;
      for(it=this->begin(); it!=this->end(); ++it) {
         if((*it)->updateAdaptorsOnStall(nStalls)) {
            updatePerformed = true;
         }
      }

      return updatePerformed;
   }

   /***************************************************************************/
   /**
    * Converts the local data to a boost::property_tree node
    *
    * @param ptr The boost::property_tree object the data should be saved to
    * @param id The id assigned to this object
    */
   virtual void toPropertyTree (
      pt::ptree& ptr
      , const std::string& baseName
   ) const OVERRIDE {
      // Check that the object isn't empty
      if(this->empty()) {
         glogger
         << "In GParameterTCollectionT<T>::toPropertyTree(): Error!" << std::endl
         << "Object is empty!" << std::endl
         << GEXCEPTION;
      }

      ptr.put(baseName + ".name", this->getParameterName());
      ptr.put(baseName + ".type", this->name());
      ptr.put(baseName + ".isLeaf", this->isLeaf());
      ptr.put(baseName + ".nVals", this->size());

      // Loop over all parameter objects and ask them to add their
      // data to our ptree object
      std::string base;
      std::size_t pos;
      typename GParameterTCollectionT<T>::const_iterator cit;
      for(cit=this->begin(); cit!=this->end(); ++cit) {
         pos = cit - this->begin();
         base = baseName + ".values.value" + boost::lexical_cast<std::string>(pos);
         (*cit)->toPropertyTree(ptr, base);
      }
   }

   /******************************************************************************/
   /**
    * Retrieves information from adaptors with a given property
    *
    * @param adaoptorName The name of the adaptor to be queried
    * @param property The property for which information is sought
    * @param data A vector, to which the properties should be added
    */
   virtual void queryAdaptor(
      const std::string& adaptorName
      , const std::string& property
      , std::vector<boost::any>& data
   ) const OVERRIDE {
      typename GParameterTCollectionT<T>::const_iterator cit;
      for(cit=this->begin(); cit!=this->end(); ++cit) {
         (*cit)->queryAdaptor(adaptorName, property, data);
      }
   }

protected:
	/***************************************************************************/
	/**
	 * Loads the data of another GParameterTCollectionT<T> object, camouflaged as a GObject
	 *
	 * @param cp A copy of another GParameterTCollectionT<T> object, camouflaged as a GObject
	 */
	virtual void load_(const GObject* cp) OVERRIDE {
		// Convert cp into local format
		const GParameterTCollectionT<T> *p_load = GObject::gobject_conversion<GParameterTCollectionT<T> >(cp);

		// Load our parent class'es data ...
		GParameterBase::load_(cp);
		GStdPtrVectorInterfaceT<T>::operator=(*p_load);
	}

	/***************************************************************************/
	/**
	 * Creates a deep clone of this object. Declared purely virtual, as this class is not
	 * intended to be used directly.
	 */
	virtual GObject* clone_() const = 0;

	/***************************************************************************/
	/**
	 * Re-implementation of a corresponding function in GStdPtrVectorInterface.
	 * Making the vector wrapper purely virtual allows the compiler to perform
	 * further optimizations.
	 */
	virtual void dummyFunction() OVERRIDE { /* nothing */ }

   /***************************************************************************/
   /**
    * This function distributes the random initialization to other objects
    */
   virtual void randomInit_(const activityMode& am) OVERRIDE {
      typename GParameterTCollectionT<T>::iterator it;
      for(it=this->begin(); it!=this->end(); ++it) {
         // Note that we do not call the randomInit_() function. First of all, we
         // do not have access to it. Secondly it might be that re-initialization of
         // a specific object is not desired.
         (*it)->GParameterBase::randomInit(am);
      }
   }

   /***************************************************************************/
   /**
    * Attach parameters of type float to the vector. This function distributes this task to
    * objects contained in the container.
    *
    * @param parVec The vector to which the float parameters will be attached
    */
   virtual void floatStreamline(
      std::vector<float>& parVec
      , const activityMode& am
   ) const OVERRIDE {
      typename GParameterTCollectionT<T>::const_iterator cit;
      for(cit=this->begin(); cit!=this->end(); ++cit) {
         (*cit)->template streamline<float>(parVec, am);
      }
   }

   /* ----------------------------------------------------------------------------------
    * So far untested
    * ----------------------------------------------------------------------------------
    */

   /***************************************************************************/
   /**
    * Attach parameters of type double to the vector. This function distributes this task to
    * objects contained in the container.
    *
    * @param parVec The vector to which the double parameters will be attached
    */
   virtual void doubleStreamline(
      std::vector<double>& parVec
      , const activityMode& am
   ) const OVERRIDE {
      typename GParameterTCollectionT<T>::const_iterator cit;
      for(cit=this->begin(); cit!=this->end(); ++cit) {
         (*cit)->template streamline<double>(parVec, am);
      }
   }

   /* ----------------------------------------------------------------------------------
    * So far untested
    * ----------------------------------------------------------------------------------
    */

   /***************************************************************************/
   /**
    * Attach parameters of type boost::int32_t to the vector. This function distributes this task
    * to objects contained in the container.
    *
    * @param parVec The vector to which the boost::int32_t parameters will be attached
    */
   virtual void int32Streamline(
      std::vector<boost::int32_t>& parVec
      , const activityMode& am
   ) const OVERRIDE {
      typename GParameterTCollectionT<T>::const_iterator cit;
      for(cit=this->begin(); cit!=this->end(); ++cit) {
         (*cit)->template streamline<boost::int32_t>(parVec, am);
      }
   }

   /* ----------------------------------------------------------------------------------
    * So far untested
    * ----------------------------------------------------------------------------------
    */

   /***************************************************************************/
   /**
    * Attach parameters of type bool to the vector.  This function distributes this task
    * to objects contained in the container.
    *
    * @param parVec The vector to which the boolean parameters will be attached
    */
   virtual void booleanStreamline(
      std::vector<bool>& parVec
      , const activityMode& am
   ) const OVERRIDE {
      typename GParameterTCollectionT<T>::const_iterator cit;
      for(cit=this->begin(); cit!=this->end(); ++cit) {
         (*cit)->template streamline<bool>(parVec, am);
      }
   }

   /* ----------------------------------------------------------------------------------
    * So far untested
    * ----------------------------------------------------------------------------------
    */

   /***************************************************************************/
   /**
    * Attach parameters of type float to the map. This function distributes this task to
    * objects contained in the container.
    *
    * @param parVec The map to which the float parameters will be attached
    */
   virtual void floatStreamline(
      std::map<std::string, std::vector<float> >& parVec
      , const activityMode& am
   ) const OVERRIDE {
      typename GParameterTCollectionT<T>::const_iterator cit;
      for(cit=this->begin(); cit!=this->end(); ++cit) {
         (*cit)->template streamline<float>(parVec, am);
      }
   }

   /* ----------------------------------------------------------------------------------
    * So far untested
    * ----------------------------------------------------------------------------------
    */

   /***************************************************************************/
   /**
    * Attach parameters of type double to the map. This function distributes this task to
    * objects contained in the container.
    *
    * @param parVec The map to which the double parameters will be attached
    */
   virtual void doubleStreamline(
      std::map<std::string, std::vector<double> >& parVec
      , const activityMode& am
   ) const OVERRIDE {
      typename GParameterTCollectionT<T>::const_iterator cit;
      for(cit=this->begin(); cit!=this->end(); ++cit) {
         (*cit)->template streamline<double>(parVec, am);
      }
   }

   /* ----------------------------------------------------------------------------------
    * So far untested
    * ----------------------------------------------------------------------------------
    */

   /***************************************************************************/
   /**
    * Attach parameters of type boost::int32_t to the map. This function distributes this task
    * to objects contained in the container.
    *
    * @param parVec The map to which the boost::int32_t parameters will be attached
    */
   virtual void int32Streamline(
      std::map<std::string, std::vector<boost::int32_t> >& parVec
      , const activityMode& am
   ) const OVERRIDE {
      typename GParameterTCollectionT<T>::const_iterator cit;
      for(cit=this->begin(); cit!=this->end(); ++cit) {
         (*cit)->template streamline<boost::int32_t>(parVec, am);
      }
   }

   /* ----------------------------------------------------------------------------------
    * So far untested
    * ----------------------------------------------------------------------------------
    */

   /***************************************************************************/
   /**
    * Attach parameters of type bool to the map.  This function distributes this task
    * to objects contained in the container.
    *
    * @param parVec The map to which the boolean parameters will be attached
    */
   virtual void booleanStreamline(
      std::map<std::string, std::vector<bool> >& parVec
      , const activityMode& am
   ) const OVERRIDE {
      typename GParameterTCollectionT<T>::const_iterator cit;
      for(cit=this->begin(); cit!=this->end(); ++cit) {
         (*cit)->template streamline<bool>(parVec, am);
      }
   }

   /* ----------------------------------------------------------------------------------
    * So far untested
    * ----------------------------------------------------------------------------------
    */

   /***************************************************************************/
   /**
    * Attach boundaries of type float to the vectors
    *
    * @param lBndVec A vector of lower float parameter boundaries
    * @param uBndVec A vector of upper float parameter boundaries
    */
   virtual void floatBoundaries(
      std::vector<float>& lBndVec
      , std::vector<float>& uBndVec
      , const activityMode& am
   ) const OVERRIDE {
      typename GParameterTCollectionT<T>::const_iterator cit;
      for(cit=this->begin(); cit!=this->end(); ++cit) {
         (*cit)->template boundaries<float>(lBndVec, uBndVec, am);
      }
   }

   /***************************************************************************/
   /**
    * Attach boundaries of type double to the vectors
    *
    * @param lBndVec A vector of lower double parameter boundaries
    * @param uBndVec A vector of upper double parameter boundaries
    */
   virtual void doubleBoundaries(
      std::vector<double>& lBndVec
      , std::vector<double>& uBndVec
      , const activityMode& am
   ) const OVERRIDE {
      typename GParameterTCollectionT<T>::const_iterator cit;
      for(cit=this->begin(); cit!=this->end(); ++cit) {
         (*cit)->template boundaries<double>(lBndVec, uBndVec, am);
      }
   }

   /***************************************************************************/
   /**
    * Attach boundaries of type boost::int32_t to the vectors
    *
    * @param lBndVec A vector of lower boost::int32_t parameter boundaries
    * @param uBndVec A vector of upper boost::int32_t parameter boundaries
    */
   virtual void int32Boundaries(
      std::vector<boost::int32_t>& lBndVec
      , std::vector<boost::int32_t>& uBndVec
      , const activityMode& am
   ) const OVERRIDE {
      typename GParameterTCollectionT<T>::const_iterator cit;
      for(cit=this->begin(); cit!=this->end(); ++cit) {
         (*cit)->template boundaries<boost::int32_t>(lBndVec, uBndVec, am);
      }
   }

   /***************************************************************************/
   /**
    * Attach boundaries of type bool to the vectors. This function has been added for
    * completeness - at the very least it can give an indication of the number of boolean
    * parameters. Note, though, that there is a function that lets you count these parameters
    * directly.
    *
    * @param lBndVec A vector of lower bool parameter boundaries
    * @param uBndVec A vector of upper bool parameter boundaries
    */
   virtual void booleanBoundaries(
      std::vector<bool>& lBndVec
      , std::vector<bool>& uBndVec
      , const activityMode& am
   ) const OVERRIDE {
      typename GParameterTCollectionT<T>::const_iterator cit;
      for(cit=this->begin(); cit!=this->end(); ++cit) {
         (*cit)->template boundaries<bool>(lBndVec, uBndVec, am);
      }
   }

   /***************************************************************************/
   /**
    * Count the number of float parameters. This function returns the responses from all
    * objects contained in this collection.
    *
    * @param am An enum indicating whether only information about active, inactive or all parameters of this type should be extracted
    * @return The number of float parameters in this collection
    */
   virtual std::size_t countFloatParameters(
      const activityMode& am
   ) const OVERRIDE {
      std::size_t result = 0;

      typename GParameterTCollectionT<T>::const_iterator cit;
      for(cit=this->begin(); cit!=this->end(); ++cit) {
         result += (*cit)->template countParameters<float>(am);
      }

      return result;
   }

   /***************************************************************************/
   /**
    * Count the number of double parameters. This function returns the responses from all
    * objects contained in this collection.
    *
    * @param am An enum indicating whether only information about active, inactive or all parameters of this type should be extracted
    * @return The number of double parameters in this collection
    */
   virtual std::size_t countDoubleParameters(
      const activityMode& am
   ) const OVERRIDE {
      std::size_t result = 0;

      typename GParameterTCollectionT<T>::const_iterator cit;
      for(cit=this->begin(); cit!=this->end(); ++cit) {
         result += (*cit)->template countParameters<double>(am);
      }

      return result;
   }

   /***************************************************************************/
   /**
    * Count the number of boost::int32_t parameters. This function returns the responses from all
    * objects contained in this collection.
    *
    * @param am An enum indicating whether only information about active, inactive or all parameters of this type should be extracted
    * @return The number of boost::int32_t parameters in this collection
    */
   virtual std::size_t countInt32Parameters(
      const activityMode& am
   ) const OVERRIDE {
      std::size_t result = 0;

      typename GParameterTCollectionT<T>::const_iterator cit;
      for(cit=this->begin(); cit!=this->end(); ++cit) {
         result += (*cit)->template countParameters<boost::int32_t>(am);
      }

      return result;
   }

   /***************************************************************************/
   /**
    * Count the number of bool parameters. This function returns the responses from all
    * objects contained in this collection.
    *
    * @param am An enum indicating whether only information about active, inactive or all parameters of this type should be extracted
    * @return The number of bool parameters in this collection
    */
   virtual std::size_t countBoolParameters(
      const activityMode& am
   ) const OVERRIDE {
      std::size_t result = 0;

      typename GParameterTCollectionT<T>::const_iterator cit;
      for(cit=this->begin(); cit!=this->end(); ++cit) {
         result += (*cit)->template countParameters<bool>(am);
      }

      return result;
   }

   /***************************************************************************/
   /**
    * Assigns part of a value vector to the parameter
    */
   virtual void assignFloatValueVector(
      const std::vector<float>& parVec
      , std::size_t& pos
      , const activityMode& am
   ) OVERRIDE {
      typename GParameterTCollectionT<T>::iterator it;
      for(it=this->begin(); it!=this->end(); ++it) {
         (*it)->template assignValueVector<float>(parVec, pos, am);
      }
   }

   /* ----------------------------------------------------------------------------------
    * So far untested
    * ----------------------------------------------------------------------------------
    */

   /***************************************************************************/
   /**
    * Assigns part of a value vector to the parameter
    */
   virtual void assignDoubleValueVector(
      const std::vector<double>& parVec
      , std::size_t& pos
      , const activityMode& am
   ) OVERRIDE {
      typename GParameterTCollectionT<T>::iterator it;
      for(it=this->begin(); it!=this->end(); ++it) {
         (*it)->template assignValueVector<double>(parVec, pos, am);
      }
   }

   /* ----------------------------------------------------------------------------------
    * So far untested
    * ----------------------------------------------------------------------------------
    */

   /***************************************************************************/
   /**
    * Assigns part of a value vector to the parameter
    */
   virtual void assignInt32ValueVector(
      const std::vector<boost::int32_t>& parVec
      , std::size_t& pos
      , const activityMode& am
   ) OVERRIDE {
      typename GParameterTCollectionT<T>::iterator it;
      for(it=this->begin(); it!=this->end(); ++it) {
         (*it)->template assignValueVector<boost::int32_t>(parVec, pos, am);
      }
   }

   /* ----------------------------------------------------------------------------------
    * So far untested
    * ----------------------------------------------------------------------------------
    */

   /***************************************************************************/
   /**
    * Assigns part of a value vector to the parameter
    */
   virtual void assignBooleanValueVector(
      const std::vector<bool>& parVec
      , std::size_t& pos
      , const activityMode& am
   ) OVERRIDE {
      typename GParameterTCollectionT<T>::iterator it;
      for(it=this->begin(); it!=this->end(); ++it) {
         (*it)->template assignValueVector<bool>(parVec, pos, am);
      }
   }

   /* ----------------------------------------------------------------------------------
    * So far untested
    * ----------------------------------------------------------------------------------
    */

   /***************************************************************************/
   /**
    * Assigns part of a value vector to the parameter
    */
   virtual void assignFloatValueVectors(
      const std::map<std::string, std::vector<float> >& parMap
      , const activityMode& am
   ) OVERRIDE {
      typename GParameterTCollectionT<T>::iterator it;
      for(it=this->begin(); it!=this->end(); ++it) {
         (*it)->template assignValueVectors<float>(parMap, am);
      }
   }

   /* ----------------------------------------------------------------------------------
    * So far untested
    * ----------------------------------------------------------------------------------
    */

   /***************************************************************************/
   /**
    * Assigns part of a value vector to the parameter
    */
   virtual void assignDoubleValueVectors(
      const std::map<std::string, std::vector<double> >& parMap
      , const activityMode& am
   ) OVERRIDE {
      typename GParameterTCollectionT<T>::iterator it;
      for(it=this->begin(); it!=this->end(); ++it) {
         (*it)->template assignValueVectors<double>(parMap, am);
      }
   }

   /* ----------------------------------------------------------------------------------
    * So far untested
    * ----------------------------------------------------------------------------------
    */

   /***************************************************************************/
   /**
    * Assigns part of a value vector to the parameter
    */
   virtual void assignInt32ValueVectors(
      const std::map<std::string
      , std::vector<boost::int32_t> >& parMap
      , const activityMode& am
   ) OVERRIDE {
      typename GParameterTCollectionT<T>::iterator it;
      for(it=this->begin(); it!=this->end(); ++it) {
         (*it)->template assignValueVectors<boost::int32_t>(parMap, am);
      }
   }

   /* ----------------------------------------------------------------------------------
    * So far untested
    * ----------------------------------------------------------------------------------
    */

   /***************************************************************************/
   /**
    * Assigns part of a value vector to the parameter
    */
   virtual void assignBooleanValueVectors(
      const std::map<std::string
      , std::vector<bool> >& parMap
      , const activityMode& am
   ) OVERRIDE {
      typename GParameterTCollectionT<T>::iterator it;
      for(it=this->begin(); it!=this->end(); ++it) {
         (*it)->template assignValueVectors<bool>(parMap, am);
      }
   }

   /* ----------------------------------------------------------------------------------
    * So far untested
    * ----------------------------------------------------------------------------------
    */

   /***************************************************************************/
   /**
    * Multiplication with a random value in a given range
    */
   virtual void floatMultiplyByRandom(
      const float& min
      , const float& max
      , const activityMode& am
   ) OVERRIDE {
      typename GParameterTCollectionT<T>::iterator it;
      for(it=this->begin(); it!=this->end(); ++it) {
         (*it)->template multiplyByRandom<float>(min, max, am);
      }
   }

   /***************************************************************************/
   /**
    * Multiplication with a random value in a given range
    */
   virtual void doubleMultiplyByRandom(
      const double& min
      , const double& max
      , const activityMode& am
   ) OVERRIDE {
      typename GParameterTCollectionT<T>::iterator it;
      for(it=this->begin(); it!=this->end(); ++it) {
         (*it)->template multiplyByRandom<double>(min, max, am);
      }
   }

   /***************************************************************************/
   /**
    * Multiplication with a random value in a given range
    */
   virtual void int32MultiplyByRandom(
      const boost::int32_t& min
      , const boost::int32_t& max
      , const activityMode& am
   ) OVERRIDE {
      typename GParameterTCollectionT<T>::iterator it;
      for(it=this->begin(); it!=this->end(); ++it) {
         (*it)->template multiplyByRandom<boost::int32_t>(min, max, am);
      }
   }

   /***************************************************************************/
   /**
    * Multiplication with a random value in the range [0,1[
    */
   virtual void floatMultiplyByRandom(const activityMode& am) OVERRIDE {
      typename GParameterTCollectionT<T>::iterator it;
      for(it=this->begin(); it!=this->end(); ++it) {
         (*it)->template multiplyByRandom<float>(am);
      }
   }

   /***************************************************************************/
   /**
    * Multiplication with a random value in the range [0,1[
    */
   virtual void doubleMultiplyByRandom(const activityMode& am) OVERRIDE {
      typename GParameterTCollectionT<T>::iterator it;
      for(it=this->begin(); it!=this->end(); ++it) {
         (*it)->template multiplyByRandom<double>(am);
      }
   }

   /***************************************************************************/
   /**
    * Multiplication with a random value in the range [0,1[
    */
   virtual void int32MultiplyByRandom(const activityMode& am) OVERRIDE {
      typename GParameterTCollectionT<T>::iterator it;
      for(it=this->begin(); it!=this->end(); ++it) {
         (*it)->template multiplyByRandom<boost::int32_t>(am);
      }
   }

   /***************************************************************************/
   /**
    * Multiplication with a constant value
    */
   virtual void floatMultiplyBy(
      const float& value
      , const activityMode& am
   ) OVERRIDE {
      typename GParameterTCollectionT<T>::iterator it;
      for(it=this->begin(); it!=this->end(); ++it) {
         (*it)->template multiplyBy<float>(value, am);
      }
   }

   /***************************************************************************/
   /**
    * Multiplication with a constant value
    */
   virtual void doubleMultiplyBy(
      const double& value
      , const activityMode& am
   ) OVERRIDE {
      typename GParameterTCollectionT<T>::iterator it;
      for(it=this->begin(); it!=this->end(); ++it) {
         (*it)->template multiplyBy<double>(value, am);
      }
   }

   /***************************************************************************/
   /**
    * Multiplication with a constant value
    */
   virtual void int32MultiplyBy(
      const boost::int32_t& value
      , const activityMode& am
   ) OVERRIDE {
      typename GParameterTCollectionT<T>::iterator it;
      for(it=this->begin(); it!=this->end(); ++it) {
         (*it)->template multiplyBy<boost::int32_t>(value, am);
      }
   }

   /***************************************************************************/
   /**
    * Initialization with a constant value
    */
   virtual void floatFixedValueInit(
      const float& value
      , const activityMode& am
   ) OVERRIDE {
      typename GParameterTCollectionT<T>::iterator it;
      for(it=this->begin(); it!=this->end(); ++it) {
         (*it)->template fixedValueInit<float>(value, am);
      }
   }

   /***************************************************************************/
   /**
    * Initialization with a constant value
    */
   virtual void doubleFixedValueInit(
      const double& value
      , const activityMode& am
   ) OVERRIDE {
      typename GParameterTCollectionT<T>::iterator it;
      for(it=this->begin(); it!=this->end(); ++it) {
         (*it)->template fixedValueInit<double>(value, am);
      }
   }

   /***************************************************************************/
   /**
    * Initialization with a constant value
    */
   virtual void int32FixedValueInit(
      const boost::int32_t& value
      , const activityMode& am
   ) OVERRIDE {
      typename GParameterTCollectionT<T>::iterator it;
      for(it=this->begin(); it!=this->end(); ++it) {
         (*it)->template fixedValueInit<boost::int32_t>(value, am);
      }
   }

   /***************************************************************************/
   /**
    * Initialization with a constant value
    */
   virtual void booleanFixedValueInit(
      const bool& value
      , const activityMode& am
   ) OVERRIDE {
      typename GParameterTCollectionT<T>::iterator it;
      for(it=this->begin(); it!=this->end(); ++it) {
         (*it)->template fixedValueInit<bool>(value, am);
      }
   }

   /***************************************************************************/
   /**
    * Adds the "same-type" parameters of another GParameterBase object to this one
    */
   virtual void floatAdd(
      boost::shared_ptr<GParameterBase> p_base
      , const activityMode& am
   ) OVERRIDE {
      // We first need to convert p_base into the local type
      boost::shared_ptr<GParameterTCollectionT<T> > p
         = GParameterBase::parameterbase_cast<GParameterTCollectionT<T> >(p_base);

      // Check that both collections have the same size
      if(this->size() != p->size()) {
         glogger
         << "In GParameterTCollectionT<T>::floatAdd(): Error!" << std::endl
         << "Collections have a different size: " << this->size() << " / " << p->size() << std::endl
         << GEXCEPTION;
      }

      typename GParameterTCollectionT<T>::iterator it;
      typename GParameterTCollectionT<T>::iterator p_it;
      for(it=this->begin(), p_it=p->begin(); it!=this->end(); ++it, ++p_it) {
         (*it)->template add<float>(*p_it, am);
      }
   }

   /***************************************************************************/
   /**
    * Adds the "same-type" parameters of another GParameterBase object to this one
    */
   virtual void doubleAdd(
      boost::shared_ptr<GParameterBase> p_base
      , const activityMode& am
   ) OVERRIDE {
      // We first need to convert p_base into the local type
      boost::shared_ptr<GParameterTCollectionT<T> > p
         = GParameterBase::parameterbase_cast<GParameterTCollectionT<T> >(p_base);

      // Check that both collections have the same size
      if(this->size() != p->size()) {
         glogger
         << "In GParameterTCollectionT<T>::doubleAdd(): Error!" << std::endl
         << "Collections have a different size: " << this->size() << " / " << p->size() << std::endl
         << GEXCEPTION;
      }

      typename GParameterTCollectionT<T>::iterator it;
      typename GParameterTCollectionT<T>::iterator p_it;
      for(it=this->begin(), p_it=p->begin(); it!=this->end(); ++it, ++p_it) {
         (*it)->template add<double>(*p_it, am);
      }
   }

   /***************************************************************************/
   /**
    * Adds the "same-type" parameters of another GParameterBase object to this one
    */
   virtual void int32Add(
      boost::shared_ptr<GParameterBase> p_base
      , const activityMode& am
   ) OVERRIDE {
      // We first need to convert p_base into the local type
      boost::shared_ptr<GParameterTCollectionT<T> > p
         = GParameterBase::parameterbase_cast<GParameterTCollectionT<T> >(p_base);

      // Check that both collections have the same size
      if(this->size() != p->size()) {
         glogger
         << "In GParameterTCollectionT<T>::int32Add(): Error!" << std::endl
         << "Collections have a different size: " << this->size() << " / " << p->size() << std::endl
         << GEXCEPTION;
      }

      typename GParameterTCollectionT<T>::iterator it;
      typename GParameterTCollectionT<T>::iterator p_it;
      for(it=this->begin(), p_it=p->begin(); it!=this->end(); ++it, ++p_it) {
         (*it)->template add<boost::int32_t>(*p_it, am);
      }
   }

   /***************************************************************************/
   /**
    * Subtracts the "same-type" parameters of another GParameterBase object from this one
    */
   virtual void floatSubtract(
      boost::shared_ptr<GParameterBase> p_base
      , const activityMode& am
   ) OVERRIDE {
      // We first need to convert p_base into the local type
      boost::shared_ptr<GParameterTCollectionT<T> > p
         = GParameterBase::parameterbase_cast<GParameterTCollectionT<T> >(p_base);

      // Check that both collections have the same size
      if(this->size() != p->size()) {
         glogger
         << "In GParameterTCollectionT<T>::floatSubtract(): Error!" << std::endl
         << "Collections have a different size: " << this->size() << " / " << p->size() << std::endl
         << GEXCEPTION;
      }

      typename GParameterTCollectionT<T>::iterator it;
      typename GParameterTCollectionT<T>::iterator p_it;
      for(it=this->begin(), p_it=p->begin(); it!=this->end(); ++it, ++p_it) {
         (*it)->template subtract<float>(*p_it, am);
      }
   }

   /***************************************************************************/
   /**
    * Subtracts the "same-type" parameters of another GParameterBase object from this one
    */
   virtual void doubleSubtract(
      boost::shared_ptr<GParameterBase> p_base
      , const activityMode& am
   ) OVERRIDE {
      // We first need to convert p_base into the local type
      boost::shared_ptr<GParameterTCollectionT<T> > p
         = GParameterBase::parameterbase_cast<GParameterTCollectionT<T> >(p_base);

      // Check that both collections have the same size
      if(this->size() != p->size()) {
         glogger
         << "In GParameterTCollectionT<T>::doubleSubtract(): Error!" << std::endl
         << "Collections have a different size: " << this->size() << " / " << p->size() << std::endl
         << GEXCEPTION;
      }

      typename GParameterTCollectionT<T>::iterator it;
      typename GParameterTCollectionT<T>::iterator p_it;
      for(it=this->begin(), p_it=p->begin(); it!=this->end(); ++it, ++p_it) {
         (*it)->template subtract<double>(*p_it, am);
      }
   }

   /***************************************************************************/
   /**
    * Subtracts the "same-type" parameters of another GParameterBase object from this one
    */
   virtual void int32Subtract(
      boost::shared_ptr<GParameterBase> p_base
      , const activityMode& am
   ) OVERRIDE {
      // We first need to convert p_base into the local type
      boost::shared_ptr<GParameterTCollectionT<T> > p
         = GParameterBase::parameterbase_cast<GParameterTCollectionT<T> >(p_base);

      // Check that both collections have the same size
      if(this->size() != p->size()) {
         glogger
         << "In GParameterTCollectionT<T>::int32Subtract(): Error!" << std::endl
         << "Collections have a different size: " << this->size() << " / " << p->size() << std::endl
         << GEXCEPTION;
      }

      typename GParameterTCollectionT<T>::iterator it;
      typename GParameterTCollectionT<T>::iterator p_it;
      for(it=this->begin(), p_it=p->begin(); it!=this->end(); ++it, ++p_it) {
         (*it)->template subtract<boost::int32_t>(*p_it, am);
      }
   }

public:
	/***************************************************************************/
	/**
	 * Applies modifications to this object. This is needed for testing purposes
	 *
	 * @return A boolean which indicates whether modifications were made
	 */
	virtual bool modify_GUnitTests() OVERRIDE {
#ifdef GEM_TESTING
		bool result = false;

		// Call the parent classes' functions
		if(GParameterBase::modify_GUnitTests()) result = true;
		if(GStdPtrVectorInterfaceT<T>::modify_GUnitTests()) result = true;

		return result;

#else /* GEM_TESTING */  // If this function is called when GEM_TESTING isn't set, throw
		condnotset("GBrokerEA::modify_GUnitTests", "GEM_TESTING");
		return false;
#endif /* GEM_TESTING */
	}

	/***************************************************************************/
	/**
	 * Performs self tests that are expected to succeed. This is needed for testing purposes
	 */
	virtual void specificTestsNoFailureExpected_GUnitTests() OVERRIDE {
#ifdef GEM_TESTING
		// Call the parent classes' functions
		GParameterBase::specificTestsNoFailureExpected_GUnitTests();
		GStdPtrVectorInterfaceT<T>::specificTestsNoFailureExpected_GUnitTests();

		//---------------------------------------------------------------------

		{ // Check adding and resetting of random number generators
			// Create two local clones
			boost::shared_ptr<GParameterTCollectionT<T> > p_test1 = this->template clone<GParameterTCollectionT<T> >();

			// Assign a factory generator
			Gem::Hap::GRandomBase *gr_test = new Gem::Hap::GRandomT<Gem::Hap::RANDOMPROXY>();
			BOOST_CHECK_NO_THROW(p_test1->assignGRandomPointer(gr_test));

			// Has the generator been assigned ?
			BOOST_CHECK(p_test1->usesLocalRNG() == false);
			BOOST_CHECK(p_test1->assignedRNGUsed() == true);

			// Make sure we use the local generator again
			BOOST_CHECK_NO_THROW(p_test1->resetGRandomPointer());

			// Get rid of the test generator
			delete gr_test;

			// We should now be using a local random number generator again
			BOOST_CHECK(p_test1->usesLocalRNG() == true);
			BOOST_CHECK(p_test1->assignedRNGUsed() == false);
		}

		//------------------------------------------------------------------------------

#else /* GEM_TESTING */  // If this function is called when GEM_TESTING isn't set, throw
      condnotset("GBrokerEA::specificTestsNoFailureExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
	}

	/***************************************************************************/
	/**
	 * Performs self tests that are expected to fail. This is needed for testing purposes
	 */
	virtual void specificTestsFailuresExpected_GUnitTests() OVERRIDE {
#ifdef GEM_TESTING
		// Call the parent classes' functions
		GParameterBase::specificTestsFailuresExpected_GUnitTests();
		GStdPtrVectorInterfaceT<T>::specificTestsFailuresExpected_GUnitTests();

		//------------------------------------------------------------------------------

		{ // Check that assigning a NULL pointer for the random number generator throws
			boost::shared_ptr<GParameterTCollectionT<T> > p_test = this->template clone<GParameterTCollectionT<T> >();

			// Assigning a NULL pointer should throw
			BOOST_CHECK_THROW(
					p_test->assignGRandomPointer(NULL);
					, Gem::Common::gemfony_error_condition
			);
		}

		//------------------------------------------------------------------------------

#else /* GEM_TESTING */  // If this function is called when GEM_TESTING isn't set, throw
      condnotset("GBrokerEA::specificTestsFailuresExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
	}

};

/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */

/******************************************************************************/
/** @brief Mark this class as abstract. This is the content of
 * BOOST_SERIALIZATION_ASSUME_ABSTRACT(T) */

namespace boost {
	namespace serialization {
		template<typename T>
		struct is_abstract<Gem::Geneva::GParameterTCollectionT<T> > : public boost::true_type {};
		template<typename T>
		struct is_abstract< const Gem::Geneva::GParameterTCollectionT<T> > : public boost::true_type {};
	}
}

/******************************************************************************/

#endif /* GPARAMETERTCOLLECTIONT_HPP_ */

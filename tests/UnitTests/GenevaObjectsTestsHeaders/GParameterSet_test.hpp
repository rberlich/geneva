/**
 * @file GParameterSet_test.cpp
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

// Standard header files go here
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// Boost header files go here

#include <boost/test/unit_test.hpp>
#include <boost/shared_ptr.hpp>

#ifndef GPARAMETERSET_TEST_HPP_
#define GPARAMETERSET_TEST_HPP_

// Geneva header files go here
#include "common/GExceptions.hpp"
#include "hap/GRandomT.hpp"
#include "geneva/GParameterSet.hpp"
#include "geneva/GDoubleCollection.hpp"
#include "geneva/GInt32Collection.hpp"
#include "geneva/GDoubleGaussAdaptor.hpp"
#include "GTestIndividual1.hpp"
#include "GStdVectorInterface_test.hpp"
#include "GEqualityPrinter.hpp"

using namespace Gem::Tests;
using namespace Gem::Hap;
using namespace Gem::Geneva;

using boost::unit_test_framework::test_suite;
using boost::unit_test_framework::test_case;

/********************************************************************************************/
// The actual unit tests for this class
class GParameterSet_test {
public:
	/***********************************************************************************/
	// Test features that are expected to work
	void no_failure_expected() {
		// Prepare printing of error messages in object comparisons
		GEqualityPrinter gep("GParameterSet_test::no_failure_expected()",
							 pow(10,-10),
							 Gem::Common::CE_WITH_MESSAGES);

		// Default construction
		GTestIndividual1 gpi;

		// Make this an evolutionary algorithm individual
		gpi.setPersonalityType(EA);
		// Check that this is indeed the case
		BOOST_CHECK(gpi.getPersonalityType() == EA);

		// Test the vector interface of GMutableSetT
		boost::shared_ptr<GDoubleCollection> tempIItem_ptr(new GDoubleCollection(100, -10., 10.));
		boost::shared_ptr<GDoubleGaussAdaptor> gdga1(new GDoubleGaussAdaptor(1.,0.001,0.,1.));
		tempIItem_ptr->addAdaptor(gdga1);

		boost::shared_ptr<GDoubleCollection> findItem_ptr(new GDoubleCollection(100, -10., 10.));
		boost::shared_ptr<GDoubleGaussAdaptor> gdga2(new GDoubleGaussAdaptor(2.,0.001,0.,2.));
		findItem_ptr->addAdaptor(gdga2);

		stdvectorinterfacetestSP(gpi, tempIItem_ptr, findItem_ptr);
		// At this point gpi should have a number of items attached to it

		BOOST_CHECK(!gpi.empty());

		// Copy construction
		GTestIndividual1 gpi_cc(gpi);
		BOOST_CHECK(gep.isEqual(gpi_cc, gpi));

		// Assignment
		GTestIndividual1 gpi_as;
		gpi_as = gpi;
		BOOST_CHECK(gep.isEqual(gpi_as, gpi));

		// Test cloning and loading
		GTestIndividual1 gpi_load;
		{
			boost::shared_ptr<GObject> gpi_clone;
			BOOST_CHECK_NO_THROW(gpi_clone = gpi.GObject::clone());
			BOOST_CHECK_NO_THROW(gpi_load.GObject::load(gpi_clone));
		}
		BOOST_CHECK(gep.isEqual(gpi_load, gpi));

		// Test retrieval of the GDoubleCollection object. Can it be modified ?
		boost::shared_ptr<GDoubleCollection> gpi_load_gdc = gpi_load.pc_at<GDoubleCollection>(0);
		gpi_load_gdc->at(0) = gpi_load_gdc->at(0) + 1;
		boost::shared_ptr<GDoubleCollection> gpi_cc_gdc = gpi_cc.pc_at<GDoubleCollection>(0);
		gpi_cc_gdc->at(0) = gpi_cc_gdc->at(0) + 1;

		// Test that the copied, cloned, ... objects become in-equal to the
		// original when they are modified
		BOOST_CHECK(gep.isInEqual(gpi_load, gpi));
		BOOST_CHECK(gep.isInEqual(gpi_cc, gpi));
		BOOST_CHECK(gep.isEqual(gpi_cc, gpi_load));

		// Test adaption
		const int NADAPTIONS=100;
		double oldValue = -1., currentValue=0.;
		for(int i=0; i<NADAPTIONS; i++) {
			gpi.adapt();
			currentValue = gpi.fitness();
			BOOST_CHECK(currentValue != oldValue);
			oldValue=currentValue;
		}

		// Test serialization and loading in different serialization modes
		{ // plain text format
			// Copy construction of a new object
			GTestIndividual1 gpi_ser;
			boost::shared_ptr<GDoubleCollection> gdc_ser1(new GDoubleCollection(100, -10., 10.));
			boost::shared_ptr<GDoubleGaussAdaptor> gdga_ser1(new GDoubleGaussAdaptor(1.,0.001,0.,1.));
			gdc_ser1->addAdaptor(gdga_ser1);
			gpi_ser.push_back(gdc_ser1);
			GTestIndividual1 gpi_ser_cp(gpi_ser);

			// Check equalities and inequalities
			BOOST_CHECK(gpi_ser_cp == gpi_ser);
			// Add a new collection to gpi_ser_cp
			boost::shared_ptr<GDoubleCollection> gdc_ser2(new GDoubleCollection(100, -10., 10.));
			boost::shared_ptr<GDoubleGaussAdaptor> gdga_ser2(new GDoubleGaussAdaptor(1.,0.001,0.,1.));
			gdc_ser2->addAdaptor(gdga_ser2);
			gpi_ser_cp.push_back(gdc_ser2);
			BOOST_CHECK(gpi_ser_cp != gpi_ser);

			// Serialize gpi_ser and load into gpi_ser_cp, check equalities and similarities
			BOOST_REQUIRE_NO_THROW(gpi_ser_cp.fromString(gpi_ser.toString(Gem::Common::SERIALIZATIONMODE_TEXT), Gem::Common::SERIALIZATIONMODE_TEXT));
			BOOST_CHECK(gep.isSimilar(gpi_ser_cp, gpi_ser));
		}

		{ // XML format
			// Copy construction of a new object
			GTestIndividual1 gpi_ser;
			boost::shared_ptr<GDoubleCollection> gdc_ser1(new GDoubleCollection(100, -10., 10.));
			boost::shared_ptr<GDoubleGaussAdaptor> gdga_ser1(new GDoubleGaussAdaptor(1.,0.001,0.,1.));
			gdc_ser1->addAdaptor(gdga_ser1);
			gpi_ser.push_back(gdc_ser1);
			GTestIndividual1 gpi_ser_cp(gpi_ser);

			// Check equalities and inequalities
			BOOST_CHECK(gpi_ser_cp == gpi_ser);
			// Add a new collection to gpi_ser_cp
			boost::shared_ptr<GDoubleCollection> gdc_ser2(new GDoubleCollection(100, -10., 10.));
			boost::shared_ptr<GDoubleGaussAdaptor> gdga_ser2(new GDoubleGaussAdaptor(1.,0.001,0.,1.));
			gdc_ser2->addAdaptor(gdga_ser2);
			gpi_ser_cp.push_back(gdc_ser2);
			BOOST_CHECK(gpi_ser_cp != gpi_ser);

			// Serialize gpi_ser and load into gpi_ser_cp, check equalities and similarities
			BOOST_REQUIRE_NO_THROW(gpi_ser_cp.fromString(gpi_ser.toString(Gem::Common::SERIALIZATIONMODE_XML), Gem::Common::SERIALIZATIONMODE_XML));
			BOOST_CHECK(gep.isSimilar(gpi_ser_cp, gpi_ser));
		}

		{ // binary test format
			// Copy construction of a new object
			// Copy construction of a new object
			GTestIndividual1 gpi_ser;
			boost::shared_ptr<GDoubleCollection> gdc_ser1(new GDoubleCollection(100, -10., 10.));
			boost::shared_ptr<GDoubleGaussAdaptor> gdga_ser1(new GDoubleGaussAdaptor(1.,0.001,0.,1.));
			gdc_ser1->addAdaptor(gdga_ser1);
			gpi_ser.push_back(gdc_ser1);
			GTestIndividual1 gpi_ser_cp(gpi_ser);

			// Check equalities and inequalities
			BOOST_CHECK(gpi_ser_cp == gpi_ser);
			// Add a new collection to gpi_ser_cp
			boost::shared_ptr<GDoubleCollection> gdc_ser2(new GDoubleCollection(100, -10., 10.));
			boost::shared_ptr<GDoubleGaussAdaptor> gdga_ser2(new GDoubleGaussAdaptor(1.,0.001,0.,1.));
			gdc_ser2->addAdaptor(gdga_ser2);
			gpi_ser_cp.push_back(gdc_ser2);
			BOOST_CHECK(gpi_ser_cp != gpi_ser);

			// Serialize gpi_ser and load into gpi_ser_cp, check equalities and similarities
			BOOST_REQUIRE_NO_THROW(gpi_ser_cp.fromString(gpi_ser.toString(Gem::Common::SERIALIZATIONMODE_BINARY), Gem::Common::SERIALIZATIONMODE_BINARY));
			BOOST_CHECK(gep.isEqual(gpi_ser_cp, gpi_ser));
		}

		//----------------------------------------------------------------------------------------------
		// Tests of the GIndividual interface
		GTestIndividual1 gpi2;
		gpi2.setPersonalityType(EA);

		boost::shared_ptr<GDoubleCollection> gdc2_ptr(new GDoubleCollection(100, -10., 10.));
		gdc2_ptr->addAdaptor(boost::shared_ptr<GDoubleGaussAdaptor>(new GDoubleGaussAdaptor(1.,0.001,0.,1.)));
		gpi2.push_back(gdc2_ptr);

		// Check that a default-constructed GIndividual does not regard itself as a parent
		BOOST_CHECK(!(gpi2.getEAPersonalityTraits())->isParent());
		BOOST_CHECK((gpi2.getEAPersonalityTraits())->getParentCounter() == 0);

		// Mark the individual as parent a few times. Should update the parent counter
		for(boost::uint32_t i=0; i<10; i++) {
			bool previous = (gpi2.getEAPersonalityTraits())->setIsParent();

			if(i==0) BOOST_CHECK(previous==false);
			else BOOST_CHECK(previous==true);

			BOOST_CHECK((gpi2.getEAPersonalityTraits())->getParentCounter() == i+1);
			BOOST_CHECK((gpi2.getEAPersonalityTraits())->isParent());
		}

		// Mark the individual as a child
		bool previous = (gpi2.getEAPersonalityTraits())->setIsChild();
		BOOST_CHECK(previous=true);
		BOOST_CHECK(!(gpi2.getEAPersonalityTraits())->isParent());
		BOOST_CHECK((gpi2.getEAPersonalityTraits())->getParentCounter()==0);

		// Set an retrieve the position in the population a number of times
		for(std::size_t i=0; i <100; i++) {
			(gpi2.getEAPersonalityTraits())->setPopulationPosition(i);
			BOOST_CHECK(i == (gpi2.getEAPersonalityTraits())->getPopulationPosition());
		}

		// Do the same with the current generation
		for(boost::uint32_t i=0; i<10000; i++) {
			gpi2.setParentAlgIteration(i);
			BOOST_CHECK(i==gpi2.getParentAlgIteration());
		}

		// The dirty flag should have been set by default
		BOOST_CHECK(gpi2.isDirty());
		// Fitness should be 0. at this point
		bool dirtyFlag;
		BOOST_CHECK(gpi2.getCurrentFitness(dirtyFlag) == 0.);
		BOOST_CHECK(dirtyFlag==true);

		// Enforce calculation of the object's fitness. Should be != 0
		BOOST_CHECK(gpi2.doFitnessCalculation() != 0.); // note: just calling fitness() will throw if lazy evaluation is not allowed

		// Dirty Flag should have been reset now
		BOOST_CHECK(!gpi2.isDirty());

		// current fitness should be == externally visible fitness
		BOOST_CHECK(gpi2.getCurrentFitness(dirtyFlag) == gpi2.fitness());
		BOOST_CHECK(!gpi2.isDirty());
		BOOST_CHECK(dirtyFlag == false);

		// adapt the object and check if the dirty flag was set
		BOOST_CHECK(!gpi2.getAllowLazyEvaluation());
		gpi2.adapt();
		BOOST_CHECK(!gpi2.isDirty());
		BOOST_CHECK(gpi2.fitness() != 0); // We can safely call the fitness function in this situation

		// Allow lazy evaluation, adapt and check the fitness again
		gpi2.setAllowLazyEvaluation(true);
		BOOST_CHECK(gpi2.getAllowLazyEvaluation() == true);
		gpi2.adapt();
		BOOST_CHECK(gpi2.isDirty()); // No evaluation should have taken place at this pojnt
		BOOST_CHECK(gpi2.fitness() != 0); // Does the actual fitness calculation
		BOOST_CHECK(!gpi2.isDirty()); // Should have been reset by the fitness function
	}

	/***********************************************************************************/
	// Test features that are expected to fail
	void failures_expected() {
		{
			// Self assignment should throw in DEBUG mode
#ifdef DEBUG
			boost::shared_ptr<GTestIndividual1> gpi_ptr(new GTestIndividual1());
			BOOST_CHECK_THROW(gpi_ptr->load(gpi_ptr), Gem::Common::gemfony_error_condition);
#endif /* DEBUG */
		}

#ifdef DEBUG
		{
			// Default construction
			GTestIndividual1 gpi;

			// Needed for the following throw test
			boost::shared_ptr<GDoubleCollection > gdc_ptr(new GDoubleCollection(100, -10., 10.));
			boost::shared_ptr<GDoubleGaussAdaptor> gdga1(new GDoubleGaussAdaptor(1.,0.001,0.,1.));
			gdc_ptr->addAdaptor(gdga1);
			gpi.push_back(gdc_ptr);

			// TODO: The enable_if check in pc_at has crippled this test. FIXME !!!
			// Trying to retrieve an item of wrong type should throw in DEBUG mode
			BOOST_CHECK_THROW(gpi.pc_at<GInt32Collection>(0), Gem::Common::gemfony_error_condition);
		}
#endif

		{
			// Default construction
			GTestIndividual1 gpi;
			gpi.setPersonalityType(EA);
			BOOST_CHECK(gpi.getPersonalityType() == EA);

			// Needed for the following throw test
			boost::shared_ptr<GDoubleCollection > gdc_ptr(new GDoubleCollection(100, -10., 10.));
			boost::shared_ptr<GDoubleGaussAdaptor> gdga1(new GDoubleGaussAdaptor(1.,0.001,0.,1.));
			gdc_ptr->addAdaptor(gdga1);
			gpi.push_back(gdc_ptr);

#ifdef DEBUG
			// As the dirty flag is set, but lazy evaluation is not allowed, calculating
			// the object's fitness should throw in DEBUG mode in generations larger than 0
			// (see also the GIndividual::fitness() function)
			BOOST_CHECK(gpi.isDirty());
			gpi.setParentAlgIteration(1);
			BOOST_CHECK_THROW(gpi.fitness(), Gem::Common::gemfony_error_condition);
#endif /* DEBUG */
		}
	}

	/***********************************************************************************/
private:
	GRandomT<RANDOMLOCAL> gr;
};

/********************************************************************************************/
// This test suite checks as much as possible of the functionality provided
// by the GParameterSet class, using the GTestIndividual1 class. It also
// checks the functionality of the GMutableSetT and the GIndividual classes,
// as far as possible.
class GParameterSetSuite: public test_suite
{
public:
	GParameterSetSuite() :test_suite("GParameterSetSuite") {
	  // create an instance of the test cases class
	  boost::shared_ptr<GParameterSet_test> instance(new GParameterSet_test());

	  test_case* GParameterSet_no_failure_expected_test_case = BOOST_CLASS_TEST_CASE(&GParameterSet_test::no_failure_expected, instance);
	  test_case* GParameterSet_failures_expected_test_case = BOOST_CLASS_TEST_CASE(&GParameterSet_test::failures_expected, instance);

	  add(GParameterSet_no_failure_expected_test_case);
	  add(GParameterSet_failures_expected_test_case);
	}
};

#endif /* GPARAMETERSET_TEST_HPP_ */

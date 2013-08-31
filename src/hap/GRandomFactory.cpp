/**
 * @file GRandomFactory.cpp
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

#include "hap/GRandomFactory.hpp"

namespace Gem {
namespace Hap {

/******************************************************************************/
/**
 * Initialization of static data members
 */
boost::uint16_t Gem::Hap::GRandomFactory::multiple_call_trap_ = 0;
boost::mutex Gem::Hap::GRandomFactory::factory_creation_mutex_;

/******************************************************************************/
/**
 * The standard constructor, which seeds the random number generator and
 * creates a predefined number of threads.
 */
GRandomFactory::GRandomFactory()
	: finalized_(false)
	, threadsHaveBeenStarted_(false)
	, n01Threads_(boost::numeric_cast<boost::uint16_t>(Gem::Common::getNHardwareThreads(DEFAULT01PRODUCERTHREADS)))
	, g01_ (DEFAULTFACTORYBUFFERSIZE)
{
	boost::mutex::scoped_lock lk(factory_creation_mutex_);
	if(multiple_call_trap_ > 0) {
	   std::cerr
	   << "Error in GRandomFactory::GRandomFactory():" << std::endl
	   << "Class has been instantiated before." << std::endl
	   << "and may be instantiated only once" << std::endl;
	   std::terminate();
	}
	else {
		multiple_call_trap_++;
	}
}

/******************************************************************************/
/**
 * The destructor. All work is done in the finalize() function.
 */
GRandomFactory::~GRandomFactory() {
	// Make sure the finalization code is executed
	// (if this hasn't happened already). Calling
	// finalize() multiple times is safe.
	finalize();
}

/******************************************************************************/
/**
 * Initializes the factory. This function does nothing at this time. Its
 * only purpose is to control initialization of the factory in the singleton.
 */
void GRandomFactory::init() { /* nothing */ }

/******************************************************************************/
/**
 * Finalization code for the GRandomFactory. All threads are given the
 * interrupt signal. Then we wait for them to join us. This function will
 * only once perform useful work and will return immediately when called a
 * second time. It can thus be called as often as you wish.
 */
void GRandomFactory::finalize() {
	// Only allow one finalization action to be carried out
	if(finalized_) return;

	producer_threads_01_.interrupt_all(); // doesn't throw
	producer_threads_01_.join_all();
	finalized_ = true; // Let the audience know
}

/******************************************************************************/
/**
 * Allows to retrieve the size of random number arrays
 *
 * @return The current value of the arraySize_ variable
 */
std::size_t GRandomFactory::getCurrentArraySize() const {
   return DEFAULTARRAYSIZE;
}

/******************************************************************************/
/**
 * Retrieves the size of the random buffer, i.e. the array holding the random
 * number packages.
 *
 * @return The size of the random buffer
 */
std::size_t GRandomFactory::getBufferSize() const {
	return DEFAULTFACTORYBUFFERSIZE;
}

/******************************************************************************/
/**
 * Provides users with an interface to set the initial seed for the global seed
 * generator. Note that this function will have no effect once seeding has started.
 * A boolean will be returned that indicates whether the function has had
 * an effect, i.e. whether the seed could be set. The seed manager will then be
 * started by this function. If not set by the user, the seed manager will start
 * upon first retrieval of a seed and will then try to acquire a seed automatically.
 *
 * @param seed The desired initial value of the global seed
 * @return A boolean indicating whether the seed could be set
 */
bool GRandomFactory::setStartSeed(const initial_seed_type& initial_seed) {
	if(!seedManager_ptr_) { // double lock pattern
		boost::mutex::scoped_lock lk(seedingMutex_);
		if(!seedManager_ptr_) {
			seedManager_ptr_ = boost::shared_ptr<Gem::Hap::GSeedManager>(new GSeedManager(initial_seed));
			return true;
		}
		else {
			return false;
		}
	}

	return false;
}

/******************************************************************************/
/**
 * Retrieval of the value of the global startSeed_ variable
 *
 * @return The value of the global start seed
 */
boost::uint32_t GRandomFactory::getStartSeed() const {
	// Initialize the generator if necessary
	if(!seedManager_ptr_) { // double lock pattern
		boost::mutex::scoped_lock lk(seedingMutex_);
		if(!seedManager_ptr_) {
			seedManager_ptr_ = boost::shared_ptr<Gem::Hap::GSeedManager>(new GSeedManager());
		}
	}

	return seedManager_ptr_->getStartSeed();
}

/******************************************************************************/
/**
 * Checks whether the seeding process has already started
 *
 * @return A boolean indicating whether seeding has already been initialized
 */
bool GRandomFactory::checkSeedingIsInitialized() const {
	// Initialize the generator if necessary
	if(!seedManager_ptr_) { // double lock pattern
		boost::mutex::scoped_lock lk(seedingMutex_);
		if(!seedManager_ptr_) {
			seedManager_ptr_ = boost::shared_ptr<Gem::Hap::GSeedManager>(new GSeedManager());
		}
	}

	return seedManager_ptr_->checkSeedingIsInitialized();
}

/******************************************************************************/
/**
 * This function returns a random number from a pseudo random number generator
 * that has been seeded from a non-deterministic source (using the facilities
 * provided by boost.random). Note that there is no guaranty (albeit a good
 * likelihood) that neighboring numbers differ. Values obtained from this source
 * are intended to be used for the seeding of further generators. For the purpose
 * at hand, it is not mandatory that all start seeds of all generators being used
 * in an application differ. Hence there is no control whether there is a certain
 * number of unique seeds in a row. This might be implemented at a later time,
 * dependent on user requests. This function also checks whether seeding has already
 * started and if not, initiates seeding.
 *
 * @return A seed based on the current time
 */
boost::uint32_t GRandomFactory::getSeed(){
	// Initialize the generator if necessary
	if(!seedManager_ptr_) { // double lock pattern
		boost::mutex::scoped_lock lk(seedingMutex_);
		if(!seedManager_ptr_) {
			seedManager_ptr_ = boost::shared_ptr<Gem::Hap::GSeedManager>(new GSeedManager());
		}
	}

	return seedManager_ptr_->getSeed();
}

/******************************************************************************/
/**
 * Allows to retrieve the size of the seeding queue
 *
 * @return The size of the seeding queue
 */
std::size_t GRandomFactory::getSeedingQueueSize() const {
	return seedManager_ptr_->getQueueSize();
}

/******************************************************************************/
/**
 * Sets the number of producer threads for this factory.
 *
 * @param n01Threads
 */
void GRandomFactory::setNProducerThreads(const boost::uint16_t& n01Threads)
{
	boost::uint16_t n01Threads_local = 0;

	// Make a suggestion for the number of threads, if requested
	if (n01Threads == 0)	{
		n01Threads_local = boost::numeric_cast<boost::uint16_t>(Gem::Common::getNHardwareThreads(DEFAULT01PRODUCERTHREADS));
	}
	else {
		n01Threads_local = n01Threads;
	}

	// Threads might already be running, so we need to regulate access
	{
		boost::mutex::scoped_lock lk(thread_creation_mutex_);
		if(threadsHaveBeenStarted_) {
			if (n01Threads_local > n01Threads_) { // start new 01 threads
				for (boost::uint16_t i = n01Threads_; i < n01Threads_local; i++) {
					boost::uint32_t seed_ =  this->getSeed();
					producer_threads_01_.create_thread(boost::bind(&GRandomFactory::producer01, this, seed_));
				}
			} else if (n01Threads_local < n01Threads_) { // We need to remove threads
				producer_threads_01_.remove_last(n01Threads_ - n01Threads_local);
			}
		}

		n01Threads_ = n01Threads_local;
	}
}

/******************************************************************************/
/**
 * When objects need a new container [0,1[ of random numbers with the current
 * default size, they call this function. Note that calling threads are responsible
 * for catching the boost::thread_interrupted exception.
 *
 * @return A packet of new [0,1[ random numbers
 */
boost::shared_array<double> GRandomFactory::new01Container() {
	// Start the producer threads upon first access to this function
	if(!threadsHaveBeenStarted_) {
		boost::mutex::scoped_lock lk(thread_creation_mutex_);
		if(!threadsHaveBeenStarted_) {
			startProducerThreads();
			threadsHaveBeenStarted_=true;
		}
	}

	boost::shared_array<double> result; // empty
	try {
		g01_.pop_back(result, boost::posix_time::milliseconds(DEFAULTFACTORYGETWAIT));
	} catch (Gem::Common::condition_time_out&) {
		// nothing - our way of signaling a time out
		// is to return an empty boost::shared_ptr
	}

	return result;
}

/******************************************************************************/
/**
 * This function starts the threads needed for the production of random numbers
 */
void GRandomFactory::startProducerThreads()  {
	for (boost::uint16_t i = 0; i < n01Threads_; i++) {
		// thread() doesn't throw, and no exceptions are listed in the documentation
		// for the create_thread() function, so we assume it doesn't throw.
		boost::uint32_t seed = this->getSeed();
		producer_threads_01_.create_thread(boost::bind(&GRandomFactory::producer01, this, seed));
	}
}

/******************************************************************************/
/**
 * The production of [0,1[ random numbers takes place here. As this function
 * is called in a thread, it may not throw under any circumstance. Exceptions
 * could otherwise go unnoticed. Hence this function has a possibly confusing
 * setup.
 *
 * @param seed A seed for our local random number generator
 */
void GRandomFactory::producer01(boost::uint32_t seed)  {
	try {
		lagged_fibonacci lf(seed);

		while (true) {
			if(boost::this_thread::interruption_requested()) {
			   break;
			}

			// Will hold the newly created random numbers
			boost::shared_array<double> p(new double[DEFAULTARRAYSIZE]);

			// Faster access during the fill procedure - uses the "raw" pointer.
			// Note that we own the only instance of this pointer at this point
			double *p_raw = p.get();
			for (std::size_t i = 0; i < DEFAULTARRAYSIZE; i++)
			{
#ifdef DEBUG
				double value = lf();
				assert(value>=0. && value<1.);
				p_raw[i]=value;
#else
				p_raw[i] = lf();
#endif /* DEBUG */
			}

		   try {
	         g01_.push_front(p, boost::posix_time::milliseconds(DEFAULTFACTORYPUTWAIT));
		   } catch (Gem::Common::condition_time_out&) {
		      continue; // Try again, if we didn't succeed
		   }
		}
	} catch (boost::thread_interrupted&) { // Not an error
		return; // We're done
	} catch (std::bad_alloc& e) {
		std::cerr << "In GRandomFactory::producer01(): Error!" << std::endl
				  << "Caught std::bad_alloc exception with message"
				  << std::endl << e.what() << std::endl;

		std::terminate();
	} catch (std::invalid_argument& e) {
		std::cerr << "In GRandomFactory::producer01(): Error!" << std::endl
				  << "Caught std::invalid_argument exception with message"  << std::endl
				  << e.what() << std::endl;

		std::terminate();
	} catch (boost::thread_resource_error&) {
		std::cerr << "In GRandomFactory::producer01(): Error!" << std::endl
				  << "Caught boost::thread_resource_error exception which"  << std::endl
				  << "likely indicates that a mutex could not be locked."  << std::endl;

		// Terminate the process
		std::terminate();
	} catch (...) {
		std::cerr << "In GRandomFactory::producer01(): Error!" << std::endl
				  << "Caught unkown exception." << std::endl;

		// Terminate the process
		std::terminate();
	}
}

/******************************************************************************/

} /* namespace Hap */
} /* namespace Gem */

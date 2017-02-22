/**
 * @file GAsioAsyncTCPConsumerT.hpp
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
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <array>
#include <chrono>
#include <vector>
#include <algorithm>

// Boost headers go here
#include <boost/asio.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/utility.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_serialize.hpp>


#ifndef GASIOASYNCTCPCONSUMERT_HPP
#define GASIOASYNCTCPCONSUMERT_HPP

// Geneva headers go here
#include "common/GThreadGroup.hpp"
#include "common/GThreadPool.hpp"
#include "common/GSerializationHelperFunctionsT.hpp"
#include "common/GHelperFunctions.hpp"
#include "common/GHelperFunctionsT.hpp"
#include "courtier/GAsioHelperFunctions.hpp"
#include "courtier/GBrokerT.hpp"
#include "courtier/GCourtierEnums.hpp"
#include "courtier/GBaseConsumerT.hpp"

namespace Gem {
namespace Courtier {

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * Forward declaration
 */
template<typename processable_type>
class GAsioAsyncTCPConsumerT;

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * This class is responsible for the client side of network communication
 * with Boost::Asio. Note that this class is noncopyable, as it is derived
 * from a non-copyable base class. It assumes that a single data transfer
 * is done each time and that the connection is interrupted inbetween transfers.
 * This may be useful for long calculations, but may cause high load on the server
 * similar to a highly-frequented web server for short work loads or when all
 * workloads are returned at the same time.
 */
template<typename processable_type>
class GAsioAsyncTCPClientT
	: public Gem::Courtier::GBaseClientT<processable_type>
{
public:
	 /***************************************************************************/
	 /**
	  * Initialization by server name/ip and port
	  *
	  * @param server Identifies the server
	  * @param port Identifies the port on the server
	  */
	 GAsioAsyncTCPClientT(
		 const std::string &server
		 , const std::string &port
	 )
		 : GBaseClientT<processable_type>()
		 , m_query(server, port)
		 , m_endpoint_iterator0(m_resolver.resolve(m_query))
	 { /* nothing */ }

	 /***************************************************************************/
	 /**
	  * Initialization by server name/ip, port and a model for the item to
	  * be processed.
	  *
	  * @param server Identifies the server
	  * @param port Identifies the port on the server
	  */
	 GAsioAsyncTCPClientT(
		 const std::string &server
		 , const std::string &port
		 , std::shared_ptr<processable_type> additionalDataTemplate
	 )
		 : GBaseClientT<processable_type>(additionalDataTemplate)
		 , m_query(server, port)
		 , m_endpoint_iterator0(m_resolver.resolve(m_query))
	 { /* nothing */ }

	 /***************************************************************************/
	 /**
	  * The standard destructor.
	  */
	 virtual ~GAsioAsyncTCPClientT() {
		 // Make sure we don't leave any open sockets lying around.
		 disconnect(m_socket);

		 // Do some book-keeping
		 glogger
			 << "In GAsioTCPClinetT<>::GAsioAsyncTCPClientTlientT():" << std::endl
			 << "Recorded " << this->getTotalConnectionAttempts() << " failed connections" << std::endl
			 << "attempts during the runtime of this client" << std::endl
			 << GLOGGING;
	 }

	 /***************************************************************************/
	 /**
	  * Sets the maximum number of stalled connection attempts
	  *
	  * @param maxStalls The maximum number of stalled connection attempts
	  */
	 void setMaxStalls(const std::uint32_t &maxStalls) {
		 m_maxStalls = maxStalls;
	 }

	 /***************************************************************************/
	 /**
	  * Retrieves the maximum allowed number of stalled attempts
	  *
	  * @return The value of the m_maxStalls variable
	  */
	 std::uint32_t getMaxStalls() const {
		 return m_maxStalls;
	 }

	 /***************************************************************************/
	 /**
	  * Sets the maximum number of failed connection attempts before termination
	  *
	  * @param maxConnectionAttempts The maximum number of allowed failed connection attempts
	  */
	 void setMaxConnectionAttempts(const std::uint32_t &maxConnectionAttempts) {
		 m_maxConnectionAttempts = maxConnectionAttempts;
	 }

	 /***************************************************************************/
	 /**
	  * Retrieves the maximum allowed number of failed connection attempts
	  *
	  * @return The value of the m_maxConnectionAttempts variable
	  */
	 std::uint32_t getMaxConnectionAttempts() const {
		 return m_maxConnectionAttempts;
	 }

	 /***************************************************************************/
	 /**
	  * Retrieves the total number of failed connection attempts during program execution
	  * up the point of the call;
	  */
	 std::uint32_t getTotalConnectionAttempts() const {
		 return m_totalConnectionAttempts;
	 }

protected:
	 /***************************************************************************/
	 /**
	  * Performs initialization work
	  */
	 virtual bool init() override {
		 using namespace boost::asio;

		 // Start the io_service. It will be kept alive by m_work_ptr
		 m_work_ptr = std::shared_ptr<io_service::work>(new io_service::work(m_io_service));
		 m_gtp.async_schedule(
			 [&]() { this->m_io_service.run(); }
		 );

		 // Try to make a connection
		 if (!tryConnect()) {
			 glogger
				 << "In GAsioAsyncTCPClientT<processable_type>::init(): Warning" << std::endl
				 << "Could not connect to server. Shutting down now." << std::endl
				 << "NOTE: This might be simply caused by the server shutting down" << std::endl
				 << "at the end of an optimization run, so that usually this is no" << std::endl
				 << "cause for concern." << std::endl
				 << GLOGGING;

			 // Make sure we don't leave any open sockets lying around.
			 disconnect(m_socket);
			 return false;
		 }

		 // At this point we should have a valid connection to the server. Let it know we want work
		 m_write_strand.wrap(write(
			 m_socket
			 , buffer(assembleQueryString("ready", Gem::Courtier::COMMANDLENGTH))
		 ));

		 // Start the ping cycle
	    this->async_ping();

		 return true;
	 }

	 /***************************************************************************/
	 /**
	  * This is the main loop of the client, after initialization
	  */
	 virtual void run_() override {
		 using namespace boost::asio;

		 try {
			 // Read data until a halt-condition is met
			 while (!this->halt()) {
				 std::uint32_t idleTime = 0; // Holds information on the idle time in milliseconds, if "idle" command is received

				 // Read the next command
				 m_read_strand.wrap(read(
					 m_socket
					 , buffer(m_commandBuffer)
				 ));

				 // Remove all leading or trailing white spaces from the command
				 std::string command = boost::algorithm::trim_copy(
					 std::string(
						 m_commandBuffer.data()
						 , Gem::Courtier::COMMANDLENGTH
					 )
				 );

				 // Act on the command
				 if ("close" == command) { // set the termination flag
					 this->flagCloseRequested();
					 break; // Stop the loop
				 } else if ("pong" == command) { // decrease the m_openPing counter and catch values < 0
					 if (m_openPings-- < 0) {
						 glogger
							 << "In GAsioAsyncTCPClientT<processable_type>::run_(): Error" << std::endl
							 << "Got a negative number of open pings " << m_openPings << ", which should not happen"
							 << std::endl
							 << GWARNING;

						 // Terminate execution
						 this->flagTerminalError();
						 break; // Stop the loop
					 }
				 } else if (this->parseIdleCommand(
					 idleTime
					 , command
				 )) { // Received no work. We have been instructed to wait for a certain time
					 // We might want to allow a given number of timeouts / stalls
					 if ((m_maxStalls != 0) && (m_stalls++ > m_maxStalls)) {
						 glogger
							 << "In GAsioAsyncTCPClientT<processable_type>::run_(): Warning!" << std::endl
							 << "Maximum number of consecutive idle commands (" << m_maxStalls << ")" << std::endl
							 << "has been reached. Leaving now." << std::endl
							 << GWARNING;

						 // Terminate execution
						 this->flagTerminalError();
						 break; // Stop the loop
					 }

					 // We can continue. But let's wait for a time specified by the server in its idle-command
					 std::this_thread::sleep_for(std::chrono::milliseconds(idleTime));
				 } else if ("compute" == command) {
					 // We have received a work item. First find out its size
					 m_read_strand.wrap(read(
						 m_socket
						 , buffer(m_commandBuffer)
					 ));
					 std::size_t dataSize = boost::lexical_cast<std::size_t>(
						 boost::algorithm::trim_copy(std::string(
							 m_commandBuffer.data()
							 , Gem::Courtier::COMMANDLENGTH
						 ))
					 );

					 // Now retrieve the serialization mode that was used
					 m_read_strand.wrap(read(
						 m_socket
						 , buffer(m_commandBuffer)
					 ));
					 // Check the serialization mode we need to use
					 std::string serMode_str = boost::algorithm::trim_copy(
						 std::string(
							 m_commandBuffer.data()
							 , Gem::Courtier::COMMANDLENGTH
						 ));
					 if(serMode_str == "") {
						 glogger
							 << "In GAsioAsyncTCPClientT<processable_tyoe>::async_start_processing() : Warning!" << std::endl
							 << "Found empty serMode_str. Leaving ..." << std::endl
							 << GEXCEPTION;
					 }
					 m_serMode = boost::lexical_cast<Gem::Common::serializationMode>(serMode_str);

					 // Finally obtain the actual payload
					 char inboundData[dataSize];
					 m_read_strand.wrap(read(
						 m_socket
						 , buffer(
							 inboundData
							 , dataSize
						 )
					 ));
					 m_item = std::string(
						 inboundData
						 , dataSize
					 );

					 // We have successfully retrieved an item, so we may reset the stall-counter
					 m_stalls = 0;

					 // Asynchronously process the item and send the result back to the server
					 m_gtp.async_schedule(
						 [this]() -> void {
							 // Unpack the data and create a new object. Note that de-serialization must
							 // generally happen through the same type that was used for serialization.
							 std::shared_ptr<processable_type> target_ptr = Gem::Common::sharedPtrFromString<processable_type>(this->m_item, this->m_serMode);

							 // Check if we have received a valid item. Leave the function if this is not the case
							 if(!target_ptr) {
								 glogger
									 << "In GSerialSubmissionClientT<T>::run_() : Warning!" << std::endl
									 << "Received empty target." << std::endl
									 << GWARNING;

								 this->flagTerminalError();
								 return;
							 }

							 // If we have a model for the item to be parallelized, load its data into the target
							 this->loadDataTemplate(target_ptr);

							 // Perform the actual processing
							 target_ptr->process();
							 this->incrementProcessingCounter();

							 // Serialize the result
	                   this->m_item = Gem::Common::sharedPtrToString(target_ptr, m_serMode);

							 // Assemble an appropriate buffer
							 std::vector<const_buffer> buffers;

							 // First the command ("result")
							 std::string result = assembleQueryString("result", Gem::Courtier::COMMANDLENGTH);
							 buffers.push_back(buffer(result));

							 // Assemble the size header
							 std::string sizeHeader = assembleQueryString(
								 boost::lexical_cast<std::string>(this->m_item.size())
								 , Gem::Courtier::COMMANDLENGTH
							 );
							 buffers.push_back(buffer(sizeHeader));

							 // Take care of the data section.
							 buffers.push_back(buffer(this->m_item));

							 // Finally write the amalgamated data to the socket. We use "gather-write"
							 // to send different buffers in a single write operation. The operation needs
							 // to be wrapped into a strand so a parallel "ping" does not iterfer with the
							 // write operation.
							 this->m_write_strand.wrap(write(m_socket, buffers));
						 }
					 );
				 } else {
					 glogger
						 << "In GAsioAsyncTCPClientT<processable_type>::run_(): Warning!" << std::endl
						 << "Received unknown command " << command << std::endl
						 << "Leaving now." << std::endl
						 << GWARNING;

					 // Terminate execution
					 this->flagTerminalError();
					 break; // Stop the loop
				 }
			 }
		 } catch (boost::system::system_error &e) {
			 glogger
				 << "In GAsioAsyncTCPClientT<processable_type>::retrieve():" << std::endl
				 << "Caught boost::system::system_error exception" << std::endl
				 << "with message" << std::endl
				 << e.what() << std::endl
				 << "This is likely normal and due to a server shutdown." << std::endl
				 << "Leaving now." << std::endl
				 << GWARNING;
			 this->flagTerminalError();
		 } catch (std::exception& e) {
			 glogger
				 << "In GAsioAsyncTCPClientT<processable_type>::retrieve():" << std::endl
				 << "Caught std::exception with message" << std::endl
				 << e.what() << std::endl
				 << GWARNING;
			 this->flagTerminalError();
		 } catch (...) {
			 glogger
				 << "In sharedPtrFromString(): Error!" << std::endl
				 << "Caught unknown exception" << std::endl
				 << GWARNING;
			 this->flagTerminalError();
		 }

		 // The io_service will run out of work after this call
		 m_work_ptr.reset();
		 // Wait for all threads in the thread pool to complete their work
		 m_gtp.wait();
	 }

	 /***************************************************************************/
	 /**
	  * Perform necessary finalization activities
	  */
	 virtual bool finally() override {
		 // Make sure we don't leave any open sockets lying around.
		 disconnect(m_socket);

		 return true;
	 }

private:
	 /***************************************************************************/
	 /**
	  * Tries to make a connection to the remote site. If a maximum number of
	  * connection attempts has been set, the function will increase the waiting
	  * time by a factor of 2 each time a connection could not be established, starting with
	  * 10 milliseconds. Thus, with a maximum of 10 connection attempts, the maximum
	  * sleep time would be about 10 seconds. This is the recommended mode of operation.
	  * If no maximum amount of connection attempts has been set, the function will
	  * sleep for 10 milliseconds every time a connection could not be established.
	  *
	  * @return true if the connection could be established, false otherwise.
	  */
	 bool tryConnect() {
		 // Try to make a connection, at max m_maxConnectionAttempts times
		 long milliSecondsWait = 10;

		 std::uint32_t connectionAttempt = 0;

		 boost::system::error_code error;
		 boost::asio::ip::tcp::resolver::iterator endpoint_iterator;

		 while (m_maxConnectionAttempts ? (connectionAttempt++ < m_maxConnectionAttempts) : true) {
			 // Restore the start of the iteration
			 endpoint_iterator = m_endpoint_iterator0;
			 error = boost::asio::error::host_not_found;

			 while (error && endpoint_iterator != m_end) {
				 // Make sure we do not try to re-open an already open socket
				 disconnect(m_socket);
				 // Make the connection attempt
				 m_socket.connect(*endpoint_iterator++, error);

				 if (error) {
					 m_totalConnectionAttempts++;
				 }
			 }

			 // We were successful
			 if (!error) break;

			 // Unsuccessful. Sleep for 0.01 seconds, then try again
			 std::this_thread::sleep_for(std::chrono::milliseconds(milliSecondsWait));

			 if (m_maxConnectionAttempts > 0) {
				 milliSecondsWait *= 2;
			 }
		 }

		 // Still error ? Return, terminate
		 if (error) return false;

		 return true;
	 }

	 /***************************************************************************/
	 /**
	  * Starts an asynchronous ping cycle to keep the connection to the server alive
	  */
	 void async_ping() {
		 using namespace boost::asio;

		 m_timer.async_wait(
			 [&](const boost::system::error_code& ec) {
				 if(!ec && !this->halt()) {
					 // Send a ping to the server
					 m_write_strand.wrap(write(
						 m_socket
						 , buffer(assembleQueryString("ping", Gem::Courtier::COMMANDLENGTH))
					 ));

					 // Increase the ping-counter
					 if(m_openPings++ > m_maxOpenPings) {
						 // This is an error -- signal termination
						 glogger
							 << "In GAsioAsyncTCPClientT<processable_type>::async_ping(): Warning" << std::endl
							 << "Exceeded maximum number of open pings " << m_maxOpenPings << std::endl
							 << "Terminating -- possibly the server is down ..." << std::endl
							 << GWARNING;

						 this->flagTerminalError();
					 } else {
						 // Keep the ping cycle alive
						 m_timer.expires_from_now(m_pingInterval);

						 // Continue the ping cycle
						 async_ping();
					 }
				 } // No further action in case of an error or if the halt condition was set
			 }
		 );
	 }

	 /***************************************************************************/
	 // Data

	 std::uint32_t m_maxStalls = GASIOTCPCONSUMERMAXSTALLS; ///< The maximum allowed number of stalled connection attempts
	 std::uint32_t m_maxConnectionAttempts = GASIOTCPCONSUMERMAXCONNECTIONATTEMPTS; ///< The maximum allowed number of failed connection attempts
	 std::uint32_t m_totalConnectionAttempts = 0; ///< The total number of failed connection attempts during program execution

	 std::uint32_t m_stalls = 0; ///< counter for stalled connection attempts

	 boost::asio::io_service m_io_service; ///< Holds the Boost::ASIO::io_service object
	 std::shared_ptr<boost::asio::io_service::work> m_work_ptr; ///< Keeps the io_service alive when filled
	 boost::asio::ip::tcp::socket m_socket{m_io_service}; ///< The underlying socket
	 boost::asio::io_service::strand m_write_strand{m_io_service}; ///< Prevent multiple concurrent writes on the same socket
	 boost::asio::io_service::strand m_read_strand{m_io_service}; ///< Prevent multiple concurrent reads on the same socket

	 boost::asio::ip::tcp::resolver m_resolver{m_io_service}; ///< Responsible for name resolution
	 boost::asio::ip::tcp::resolver::query m_query; ///< A query

	 boost::asio::ip::tcp::resolver::iterator m_endpoint_iterator0; ///< start of iteration
	 boost::asio::ip::tcp::resolver::iterator m_end; ///< end for end point iterator

	 std::atomic<std::int_fast32_t> m_openPings{0}; ///< Counts the number of ping requests without matching pong from the server
	 std::int32_t m_maxOpenPings = GASIOMAXOPENPINGS; ///< The maximum number of pings without matching pong from the server

	 std::chrono::milliseconds m_pingInterval = GASIOPINGINTERVAL; ///< The time inbetween two pings
	 boost::asio::steady_timer m_timer{m_io_service, m_pingInterval};

	 std::array<char, COMMANDLENGTH> m_commandBuffer; ///< A buffer to be used for command transfers

	 Gem::Common::GThreadPool m_gtp{2}; ///< Holds workers processing work items as well as the io_service.run()

	 std::string m_item; ///< Holds a data item that was retrieved from the server
	 Gem::Common::serializationMode m_serMode; ///< The serialization mode as requested by the server

	 /***************************************************************************/
	 // Prevent default construction
	 GAsioAsyncTCPClientT() = delete;

	 // Prevent copy construction and assignment
	 GAsioAsyncTCPClientT(const GAsioAsyncTCPClientT<processable_type>&) = delete;
	 GAsioAsyncTCPClientT(const GAsioAsyncTCPClientT<processable_type>&&) = delete;
	 const GAsioAsyncTCPClientT<processable_type>& operator=(const GAsioAsyncTCPClientT<processable_type>&) = delete;
	 const GAsioAsyncTCPClientT<processable_type>& operator=(const GAsioAsyncTCPClientT<processable_type>&&) = delete;
};

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * An instance of this class is created for each new connection initiated
 * by the client. All the details of the data exchange between server
 * and client are implemented here. The class is declared in the same
 * file as the GAsioTCPConsumer in order to avoid cross referencing of
 * header files. It assumes that a single data transfer is done each time and
 * that the connection is interrupted inbetween transfers. This may be useful
 * for long calculations, but may cause high load on the server similar to
 * a highly-frequented web server for short work loads or when all workloads
 * are returned at the same time.
 */
template<typename processable_type>
class GAsioAsyncServerSessionT
	: public std::enable_shared_from_this<GAsioAsyncServerSessionT<processable_type>>
  	, private boost::noncopyable
{
public:
	 /***************************************************************************/
	 /**
	  * Standard constructor. Its main purpose it to initialize the underlying
	  * socket. We do not initialize the various character arrays and strings,
	  * as they are overwritten for each call to this class.
	  *
	  * @param io_service A reference to the server's io_service
	  */
	 GAsioAsyncServerSessionT(
		 boost::asio::io_service &io_service
		 , const Gem::Common::serializationMode &serMod
		 , GAsioAsyncTCPConsumerT<processable_type> *master
	 )
		 : m_write_strand(io_service)
		 , m_read_strand(io_service)
		 , m_socket(io_service)
		 , m_serializationMode(serMod)
		 , m_master(master)
		 , m_broker_ptr(master->m_broker_ptr)
	 {
		 // Update the connection counter in the consumer
		 m_master->m_connections++;
	 }

	 /***************************************************************************/
	 /**
	  * A standard destructor. Shuts down and closes the socket. Note: Non-virtual.
	  */
	 ~GAsioAsyncServerSessionT()
	 {
		 // Update the connection counter in the consumer
		 m_master->m_connections--;
	 }

	 /***************************************************************************/
	 /**
	  * This function processes requests from one client synchronously (i.e. the
	  * function will not return immediately. It will also check whether the stop
	  * condition was set in the server and will inform the clients accordingly.
	  */
	 void process() {
		 using namespace boost::asio;

		 try {
			 // Act on data until the termination flag is set
			 while (!m_master->stopped()) {
				 // Read the next command
				 m_read_strand.wrap(read(
					 m_socket
					 , buffer(m_commandBuffer)
				 ));
				 std::string command = boost::algorithm::trim_copy(
					 std::string(
						 m_commandBuffer.data()
						 , Gem::Courtier::COMMANDLENGTH
					 )
				 );


				 // Act on the received command
				 if("ping" == command) { // Send a "pong" back
					 this->sendSingleCommand("pong");
				 } else if("ready" == command) {
					 this->submitToRemote();
				 } else if("result" == command) {
					 this->retrieveFromRemote();
					 this->submitToRemote();
				 } else {
					 glogger
						 << "In GAsioAsyncServerSessionT<processable_type>::process(): Warning!" << std::endl
						 << "Received unknown command \"" << command << "\"" << std::endl
						 << GWARNING;

					 this->sendSingleCommand("unknown");
				 }
			 }

			 // The stopped flag was set -- let the client know by sending the close command
			 this->sendSingleCommand("close");
			 // Make sure we don't leave any open sockets lying around.
			 disconnect(m_socket);
		 } catch (const boost::system::system_error &e) {
			 glogger
				 << "In GAsioAsyncServerSessionT::process():" << std::endl
				 << "Caught boost::system::system_error exception with messages:" << std::endl
				 << e.what() << std::endl
				 << GEXCEPTION;
		 } catch (const boost::exception &e) {
			 glogger
				 << "In GAsioAsyncServerSessionT::process():" << std::endl
				 << "Caught boost::exception exception with messages:" << std::endl
				 << boost::diagnostic_information(e) << std::endl
				 << GEXCEPTION;
		 } catch (...) {
			 glogger
				 << "In GAsioAsyncServerSessionT::process():" << std::endl
				 << "Caught unknown exception" << std::endl
				 << GEXCEPTION;
		 }
	 }

	 /***************************************************************************/
	 /**
	  * Returns the socket used by this object
	  *
	  * @return The socket used by this object
	  */
	 boost::asio::ip::tcp::socket &getSocket() {
		 return m_socket;
	 }

private:
	 /***************************************************************************/
	 /**
	  * Retrieves an item from the remote site
	  */
	 void retrieveFromRemote() {
		 using namespace boost::asio;

		 // We have received a work item. First find out its size
		 m_read_strand.wrap(read(
			 m_socket
			 , buffer(m_commandBuffer)
		 ));
		 std::size_t dataSize = boost::lexical_cast<std::size_t>(
			 boost::algorithm::trim_copy(std::string(
				 m_commandBuffer.data()
				 , Gem::Courtier::COMMANDLENGTH
			 ))
		 );

		 // Finally obtain the actual payload
		 char inboundData[dataSize];
		 m_read_strand.wrap(read(
			 m_socket
			 , buffer(
				 inboundData
				 , dataSize
			 )
		 ));

		 // ... and schedule the work item for de-serialization and resubmission to the broker
		 m_master->async_scheduleDeSerialization(
			 std::shared_ptr<std::string>(new std::string(inboundData, dataSize))
		 );
	 }

	 /***************************************************************************/
	 /**
	  * Submit item to remote site
	  */
	 void submitToRemote() {
		 // Will hold a new work item
		 std::shared_ptr <processable_type> p;

		 // Retrieve an item. Terminate the submission if
		 // no item could be retrieved
		 std::size_t nRetries = 0;

		 while (!m_broker_ptr->get(p, m_timeout)) {
			 if (++nRetries > m_brokerRetrieveMaxRetries) {
				 std::string idleCommand
					 = std::string("idle(") + boost::lexical_cast<std::string>(m_noDataClientSleepMilliSeconds) +
						std::string(")");
				 this->sendSingleCommand(idleCommand);
				 return;
			 }
		 }

		 // Retrieve a string representation of the data item
		 // TODO: processable_type should already hold the serialization code
		 std::string item = Gem::Common::sharedPtrToString(p, m_serializationMode);

		 // Format the command
		 std::string outbound_command_header = assembleQueryString(
			 "compute", Gem::Courtier::COMMANDLENGTH
		 );

		 // Format the size header
		 std::string outbound_size_header
			 = assembleQueryString(boost::lexical_cast<std::string>(item.size()), Gem::Courtier::COMMANDLENGTH);

		 // Format a header for the serialization mode
		 std::string serialization_header = assembleQueryString(
			 boost::lexical_cast<std::string>(m_serializationMode), Gem::Courtier::COMMANDLENGTH
		 );

		 // Assemble the data buffers
		 std::vector<boost::asio::const_buffer> buffers;

		 buffers.push_back(boost::asio::buffer(outbound_command_header));
		 buffers.push_back(boost::asio::buffer(outbound_size_header));
		 buffers.push_back(boost::asio::buffer(serialization_header));
		 buffers.push_back(boost::asio::buffer(item));

		 // Finally write the amalgamated data to the socket. We use "gather-write"
		 // to send different buffers in a single write operation. The operation needs
		 // to be wrapped into a strand so a parallel "ping" does not iterfer with the
		 // write operation.
		 this->m_write_strand.wrap(write(m_socket, buffers));
	 }

	 /***************************************************************************/
	 /**
	  * Write a single command to a socket.
	  *
	  * @param command A command to be written to a socket
	  */
	 void sendSingleCommand(const std::string &command) {
		 using namespace boost::asio;

		 // Format the command ...
		 std::string outbound_command = assembleQueryString(command, Gem::Courtier::COMMANDLENGTH);
		 // ... and tell the client
		 try {
			 m_write_strand.wrap(write(
				 m_socket
				 , boost::asio::buffer(outbound_command)
			 ));
		 } catch (const boost::system::system_error &e) {
			 glogger
				 << "In GAsioSerialServerSessionT::sendSingleCommand():" << std::endl
				 << "Caught boost::system::system_error exception with messages:" << std::endl
				 << e.what() << std::endl
				 << GEXCEPTION;
		 } catch (const boost::exception &e) {
			 glogger
				 << "In GAsioSerialServerSessionT::sendSingleCommand():" << std::endl
				 << "Caught boost::exception exception with messages:" << std::endl
				 << boost::diagnostic_information(e) << std::endl
				 << GEXCEPTION;
		 } catch (...) {
			 glogger
				 << "In GAsioSerialServerSessionT::sendSingleCommand():" << std::endl
				 << "Caught unknown exception" << std::endl
				 << GEXCEPTION;
		 }
	 }

	 /***************************************************************************/

	 GAsioAsyncServerSessionT() = delete; ///< Intentionally left undefined
	 GAsioAsyncServerSessionT(const GAsioAsyncServerSessionT<processable_type>& ) = delete; ///< Intentionally left undefined
	 GAsioAsyncServerSessionT(const GAsioAsyncServerSessionT<processable_type>&&) = delete; ///< Intentionally left undefined
	 const GAsioAsyncServerSessionT<processable_type> &operator=(
		 const GAsioAsyncServerSessionT<processable_type> &
	 ) = delete; ///< Intentionally left undefined
	 const GAsioAsyncServerSessionT<processable_type> &operator=(
		 const GAsioAsyncServerSessionT<processable_type>&&
	 ) = delete; ///< Intentionally left undefined

	 /***************************************************************************/

	 boost::asio::io_service::strand m_write_strand; ///< Prevent concurrent writes on the same socket
	 boost::asio::io_service::strand m_read_strand; ///< Prevent concurrent reads on the same socket
	 boost::asio::ip::tcp::socket m_socket; ///< The underlying socket

	 std::array<char, COMMANDLENGTH> m_commandBuffer; ///< A buffer to be used for command transfers

	 Gem::Common::serializationMode m_serializationMode = Gem::Common::serializationMode::SERIALIZATIONMODE_BINARY; ///< Specifies the serialization mode
	 GAsioAsyncTCPConsumerT<processable_type> *m_master;
	 std::shared_ptr<GBrokerT<processable_type>> m_broker_ptr;

	 std::chrono::duration<double> m_timeout = std::chrono::milliseconds(200); ///< A timeout for put- and get-operations

	 std::size_t m_brokerRetrieveMaxRetries = 1; ///< The maximum amount
	 std::uint32_t m_noDataClientSleepMilliSeconds = 100; ///< The amount of milliseconds the client should sleep when no data could be retrieved from the broker
};

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * It is the main responsibility of this class to start new server session
 * for each client request. It assumes that a single data transfer is done each time and
 * that the connection is interrupted inbetween transfers. This may be useful
 * for long calculations, but may cause high load on the server similar to
 * a highly-frequented web server for short work loads or when all workloads
 * are returned at the same time
 */
template<typename processable_type>
class GAsioAsyncTCPConsumerT
	: public Gem::Courtier::GBaseConsumerT<processable_type> // note: GBaseConsumerT<> is non-copyable
{
	 friend class GAsioAsyncServerSessionT<processable_type>;

public:
	 /***************************************************************************/
	 /**
	  * The default constructor
	  */
	 GAsioAsyncTCPConsumerT()
	 { /* noting */ }

	 /***************************************************************************/
	 /**
	  * A constructor that accepts a number of vital parameters
	  *
	  * @param port The port where the server should wait for new connections
	  * @param listenerThreads The number of threads used to wait for incoming connections
	  * @param sm The desired serialization mode
	  */
	 GAsioAsyncTCPConsumerT(
		 const unsigned short &port
		 , const std::size_t &listenerThreads = 0
		 , const Gem::Common::serializationMode &sm = Gem::Common::serializationMode::SERIALIZATIONMODE_BINARY
	 )
		 : m_listenerThreads(listenerThreads > 0 ? listenerThreads : Gem::Common::getNHardwareThreads(GASIOTCPCONSUMERTHREADS))
		 , m_serializationMode(sm)
		 , m_port(port)
	 { /* nothing */ }

	 /***************************************************************************/
	 /**
	  * A standard destructor
	  */
	 virtual ~GAsioAsyncTCPConsumerT()
	 { /* nothing */ }

	 /***************************************************************************/
	 /**
	  * Returns an indication whether full return can be expected from this
	  * consumer.
	  */
	 virtual bool capableOfFullReturn() const override {
		 return false;
	 }
	 /***************************************************************************/
	 /**
	  * Returns the (possibly estimated) number of concurrent processing units.
	  * Note that this function does not make any assumptions whether processing
	  * units are dedicated solely to a given task.
	  */
	 virtual std::size_t getNProcessingUnitsEstimate(bool& exact) const override {
		 exact=false; // mark the answer as approximate
		 return boost::numeric_cast<std::size_t>(m_connections.load());
	 }

	 /***************************************************************************/
	 /**
	  * Allows to set the server name or ip
	  */
	 void setServer(std::string server) {
		 m_server = server;
	 }

	 /***************************************************************************/
	 /**
	  * Allows to retrieve the server name or ip
	  */
	 std::string getServer() const {
		 return m_server;
	 }

	 /***************************************************************************/
	 /**
	  * Allows to set the port the server listens on
	  */
	 void setPort(unsigned short port) {
		 m_port = port;
	 }

	 /***************************************************************************/
	 /**
	  * Allows to retrieve the port the server listens on
	  */
	 unsigned short getPort() const {
		 return m_port;
	 }

	 /***************************************************************************/
	 /**
	  * Allows to set the number of listener threads
	  */
	 void setNListenerThreads(std::size_t listenerThreads) {
		 m_listenerThreads = listenerThreads;
	 }

	 /***************************************************************************/
	 /**
	  * Allows to retrieve the number of listener threads
	  */
	 std::size_t getNListenerThreads() const {
		 return m_listenerThreads;
	 }

	 /***************************************************************************/
	 /**
	  * Allows to set the serialization mode
	  */
	 void setSerializationMode(Gem::Common::serializationMode sm) {
		 m_serializationMode = sm;
	 }

	 /***************************************************************************/
	 /**
	  * Retrieves the serialization mode.
	  *
	  * @return The current serialization mode
	  */
	 Gem::Common::serializationMode getSerializationMode() const {
		 return m_serializationMode;
	 }

	 /***************************************************************************/
	 /**
	  * Sets the maximum number of stalled connection attempts
	  *
	  * @param maxStalls The maximum number of stalled connection attempts
	  */
	 void setMaxStalls(const std::uint32_t &maxStalls) {
		 m_maxStalls = maxStalls;
	 }

	 /***************************************************************************/
	 /**
	  * Retrieves the maximum allowed number of stalled attempts
	  *
	  * @return The value of the m_maxStalls variable
	  */
	 std::uint32_t getMaxStalls() const {
		 return m_maxStalls;
	 }

	 /***************************************************************************/
	 /**
	  * Sets the maximum number of failed connection attempts before termination
	  *
	  * @param maxConnectionAttempts The maximum number of allowed failed connection attempts
	  */
	 void setMaxConnectionAttempts(const std::uint32_t &maxConnectionAttempts) {
		 m_maxConnectionAttempts = maxConnectionAttempts;
	 }

	 /***************************************************************************/
	 /**
	  * Retrieves the maximum allowed number of failed connection attempts
	  *
	  * @return The value of the m_maxConnectionAttempts variable
	  */
	 std::uint32_t getMaxConnectionAttempts() const {
		 return m_maxConnectionAttempts;
	 }

	 /***************************************************************************/
	 /**
	  * Allows to check whether this consumer needs a client to operate.
	  *
	  * @return A boolean indicating whether this consumer needs a client to operate
	  */
	 virtual bool needsClient() const override {
		 return true;
	 }

	 /***************************************************************************/
	 /**
	  * Emits a client suitable for processing the data emitted by this consumer
	  */
	 virtual std::shared_ptr<GBaseClientT<processable_type>> getClient() const override {
		 std::shared_ptr <GAsioAsyncTCPClientT<processable_type>> p(
			 new GAsioAsyncTCPClientT<processable_type>(m_server, boost::lexical_cast<std::string>(m_port))
		 );

		 p->setMaxStalls(m_maxStalls); // Set to 0 to allow an infinite number of stalls
		 p->setMaxConnectionAttempts(m_maxConnectionAttempts); // Set to 0 to allow an infinite number of failed connection attempts

		 return p;
	 }

	 /***************************************************************************/
	 /**
	  * Starts the actual processing loops
	  */
	 virtual void async_startProcessing() override {
		 // Open the acceptor
		 boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), m_port);
		 m_acceptor.open(endpoint.protocol());
		 m_acceptor.bind(endpoint);
		 // Set the SOL_SOCKET/SO_REUSEADDR options
		 // compare http://www.boost.org/doc/libs/1_57_0/doc/html/boost_asio/reference/basic_socket_acceptor/reuse_address.html
		 boost::asio::socket_base::reuse_address option(true);
		 m_acceptor.set_option(option);
		 m_acceptor.listen();

		 // Retrieve a pointer to the global broker for later usage
		 m_broker_ptr = GBROKER(processable_type);

		 if (!m_broker_ptr) {
			 glogger
				 << "In GAsioAsyncTCPConsumerT<processable_type>::async_startProcessing(): Error!" << std::endl
				 << "Got empty broker pointer" << std::endl
				 << GEXCEPTION;
		 }

		 // Set the number of threads in the pool
		 m_gtp.setNThreads(boost::numeric_cast<unsigned int>(m_listenerThreads));

		 try {
			 // Prevent a race condition
			 m_work.reset(new boost::asio::io_service::work(m_io_service));

			 // Start the first session
			 this->async_newAccept();

			 // Create a number of threads responsible for the m_io_service objects
			 // This absolutely needs to happen after the first session has started,
			 // so the io_service doesn't run out of work
			 m_gtg.create_threads(
				 [&]() { this->m_io_service.run(); } // this-> deals with a problem of g++ 4.7.2
				 , m_listenerThreads
			 );
		 } catch (const boost::system::system_error &e) {
			 glogger
				 << "In GAsioAsyncTCPConsumerT::async_startProcessing():" << std::endl
				 << "Caught boost::system::system_error exception with messages:" << std::endl
				 << boost::diagnostic_information(e) << std::endl
				 << GEXCEPTION;
		 } catch (const boost::exception &e) {
			 glogger
				 << "In GAsioAsyncTCPConsumerT::async_startProcessing():" << std::endl
				 << "Caught boost::exception with messages:" << std::endl
				 << boost::diagnostic_information(e) << std::endl
				 << GEXCEPTION;
		 } catch (const std::exception &e) {
			 glogger
				 << "In GAsioAsyncTCPConsumerT::async_startProcessing():" << std::endl
				 << "Caught std::exception with messages:" << std::endl
				 << boost::diagnostic_information(e) << std::endl
				 << GEXCEPTION;
		 } catch (...) {
			 glogger
				 << "In GAsioAsyncTCPConsumerT::async_startProcessing():" << std::endl
				 << "Caught unknown exception" << std::endl
				 << GEXCEPTION;
		 }
	 }

	 /***************************************************************************/
	 /**
	  * Make sure the consumer and the server sessions shut down gracefully
	  */
	 virtual void shutdown() override {
		 // Set the stop criterion
		 GBaseConsumerT<processable_type>::shutdown();

		 // Terminate the worker and clear the thread group
		 m_work.reset(); // This will initiate termination of all threads

		 // Terminate the io service
		 m_io_service.stop();
		 // Wait for the threads in the group to exit
		 m_gtg.join_all();
	 }

	 /***************************************************************************/
	 /**
	  * A unique identifier for a given consumer
	  *
	  * @return A unique identifier for a given consumer
	  */
	 virtual std::string getConsumerName() const override {
		 return std::string("GAsioAsyncTCPConsumerT");
	 }

	 /***************************************************************************/
	 /**
	  * Returns a short identifier for this consumer
	  */
	 virtual std::string getMnemonic() const override {
		 return std::string("ws");
	 }

	 /***************************************************************************/
	 /**
	  * Adds local command line options to a boost::program_options::options_description object.
	  *
	  * @param visible Command line options that should always be visible
	  * @param hidden Command line options that should only be visible upon request
	  */
	 virtual void addCLOptions(
		 boost::program_options::options_description &visible, boost::program_options::options_description &hidden
	 ) override {
		 namespace po = boost::program_options;

		 visible.add_options()
			 ("ws_ip", po::value<std::string>(&m_server)->default_value(GASIOTCPCONSUMERDEFAULTSERVER),
				 "\t[ws] The name or ip of the server")
			 ("ws_port", po::value<unsigned short>(&m_port)->default_value(GASIOTCPCONSUMERDEFAULTPORT),
			 "\t[ws] The port of the server");

		 hidden.add_options()
			 ("ws_serializationMode", po::value<Gem::Common::serializationMode>(&m_serializationMode)->default_value(
				 GASIOTCPCONSUMERSERIALIZATIONMODE),
				 "\t[ws] Specifies whether serialization shall be done in TEXTMODE (0), XMLMODE (1) or BINARYMODE (2)")
			 ("ws_maxStalls", po::value<std::uint32_t>(&m_maxStalls)->default_value(GASIOTCPCONSUMERMAXSTALLS),
				 "\t[ws] The maximum allowed number of stalled connection attempts of a client. 0 means \"forever\".")
			 ("ws_maxConnectionAttempts",
				 po::value<std::uint32_t>(&m_maxConnectionAttempts)->default_value(GASIOTCPCONSUMERMAXCONNECTIONATTEMPTS),
				 "\t[ws] The maximum allowed number of failed connection attempts of a client")
			 ("ws_nListenerThreads", po::value<std::size_t>(&m_listenerThreads)->default_value(m_listenerThreads),
				 "\t[ws] The number of threads used to listen for incoming connections");
	 }

	 /***************************************************************************/
	 /**
	  * Takes a boost::program_options::variables_map object and checks for supplied options.
	  */
	 virtual void actOnCLOptions(const boost::program_options::variables_map &vm) override
	 { /* nothing */ }

private:
	 /***************************************************************************/
	 /**
	  * Schedules the de-serialization of a completed work item, so the server
	  * session does not need to perform this work
	  */
	 void async_scheduleDeSerialization(
		 std::shared_ptr<std::string> dataBody_ptr
	 ) {
		 m_gtp.async_schedule(
			 std::function<void()>(
				 std::bind(
					 &GAsioAsyncTCPConsumerT<processable_type>::handle_workItemComplete // Does its own error checks
					 , this
					 , dataBody_ptr
					 , m_serializationMode
					 , m_timeout
				 )
			 )
		 );
	 }

	 /***************************************************************************/
	 /**
	  * Handles the de-serialization of a completed work item. This function
	  * will usually be called in its own thread.
	  */
	 void handle_workItemComplete(
		 std::shared_ptr<std::string> dataBody_ptr
		 , Gem::Common::serializationMode sM
		 , std::chrono::duration<double> timeout
	 ) {
		 // De-Serialize the data
		 std::shared_ptr <processable_type> p
			 = Gem::Common::sharedPtrFromString<processable_type>(*dataBody_ptr, sM);

		 // Complain if this is an empty item
		 if (!p) {
			 glogger
				 << "In GAsioAsyncTCPConsumerT<>::handle_workItemComplete(): Error!" << std::endl
				 << "Received empty item when filled item was expected!" << std::endl
				 << GEXCEPTION;
		 }

		 // Return the item to the broker. The item will be discarded
		 // if the requested target queue cannot be found.
		 try {
			 while (!m_broker_ptr->put(p, timeout)) {
				 if (this->stopped()) { // This may lead to a loss of items
					 glogger
						 << "GAsioAsyncTCPConsumerT<>::In handle_workItemComplete(): Warning!" << std::endl
						 << "Discarding item as the consumer object stopped operation" << std::endl
						 << GWARNING;

					 return;
				 }

				 continue;
			 }
		 } catch (const Gem::Courtier::buffer_not_present &) { // discard the item
			 glogger
				 << "GAsioAsyncTCPConsumerT<>::In handle_workItemComplete(): Warning!" << std::endl
				 << "Discarding item as buffer port is not present" << std::endl
				 << GWARNING;

			 return;
		 } catch (...) {
			 glogger
				 << "GAsioAsyncTCPConsumerT<>::In handle_workItemComplete():" << std::endl
				 << "Caught unknown exception" << std::endl
				 << GEXCEPTION;
		 }
	 }

	 /***************************************************************************/
	 /**
	  * Starts a new session
	  */
	 void async_newAccept() {
		 try {
			 // First we make sure a new session is started asynchronously so the next request can be served
			 std::shared_ptr<GAsioAsyncServerSessionT<processable_type>> newSession(
				 new GAsioAsyncServerSessionT<processable_type>(
					 m_io_service
					 , m_serializationMode
					 , this
				 )
			 );

			 // Start the new session
			 m_acceptor.async_accept(
				 newSession->getSocket()
				 , std::bind(
					 &GAsioAsyncTCPConsumerT<processable_type>::async_handleAccept
					 , this
					 , newSession
					 , std::placeholders::_1 // Replaces boost::asio::placeholders::error
				 )
			 );
		 } catch (const boost::system::system_error &e) {
			 glogger
				 << "In GAsioAsyncTCPConsumerT::async_newAccept():" << std::endl
				 << "Caught boost::system::system_error exception with messages:" << std::endl
				 << e.what() << std::endl
				 << GEXCEPTION;
		 } catch (const boost::exception &e) {
			 glogger
				 << "In GAsioAsyncTCPConsumerT::async_newAccept():" << std::endl
				 << "Caught boost::exception exception with messages:" << std::endl
				 << boost::diagnostic_information(e) << std::endl
				 << GEXCEPTION;
		 } catch (...) {
			 glogger
				 << "In GAsioAsyncTCPConsumerT::async_newAccept():" << std::endl
				 << "Caught unknown exception" << std::endl
				 << GEXCEPTION;
		 }
	 }


	 /***************************************************************************/
	 /**
	  * Handles a new connection request from a client.
	  *
	  * @param currentSession A pointer to the current session
	  * @param error Possible error conditions
	  */
	 void async_handleAccept(
		 std::shared_ptr<GAsioAsyncServerSessionT<processable_type>> currentSession
		 , const boost::system::error_code &error
	 ) {
		 if (error) {
			 glogger
				 << "In GAsioAsyncTCPConsumerT<>::async_handleAccept():"
				 << "Terminating on error " << error << std::endl
				 << GEXCEPTION;
			 return;
		 }

		 try {
			 // Initiate waiting for new connections
			 this->async_newAccept();

			 // Initiate the processing sequence
			 if(currentSession) {
				 currentSession->process();
			 } else {
				 glogger
				 << "In AsioAsyncTCPConsumerT::async_handleAccept():" << std::endl
				 << "currentSession pointer seems to be empty" << std::endl
			    << GEXCEPTION;
			 }
		 } catch (const boost::system::system_error &e) {
			 glogger
				 << "In GAsioAsyncTCPConsumerT::async_handleAccept():" << std::endl
				 << "Caught boost::system::system_error exception with messages:" << std::endl
				 << e.what() << std::endl
				 << GEXCEPTION;
		 } catch (const boost::exception &e) {
			 glogger
				 << "In GAsioAsyncTCPConsumerT::async_handleAccept():" << std::endl
				 << "Caught boost::exception exception with messages:" << std::endl
				 << boost::diagnostic_information(e) << std::endl
				 << GEXCEPTION;
		 } catch (...) {
			 glogger
				 << "In GAsioAsyncTCPConsumerT::async_handleAccept():" << std::endl
				 << "Caught unknown exception" << std::endl
				 << GEXCEPTION;
		 }
	 }

	 /***************************************************************************/
	 // Data

	 boost::asio::io_service m_io_service;   ///< ASIO's io service, responsible for event processing, absolutely needs to be _before_ acceptor so it gets initialized first.
	 std::shared_ptr<boost::asio::io_service::work> m_work; ///< A place holder ensuring that the io_service doesn't stop prematurely
	 std::size_t m_listenerThreads = Gem::Common::getNHardwareThreads(GASIOTCPCONSUMERTHREADS);  ///< The number of threads used to listen for incoming connections through io_servce::run()
	 boost::asio::ip::tcp::acceptor m_acceptor{m_io_service}; ///< takes care of external connection requests
	 Gem::Common::serializationMode m_serializationMode = Gem::Common::serializationMode::SERIALIZATIONMODE_BINARY; ///< Specifies the serialization mode
	 std::uint32_t m_maxStalls = GASIOTCPCONSUMERMAXSTALLS; ///< The maximum allowed number of stalled connection attempts of a client
	 std::uint32_t m_maxConnectionAttempts = GASIOTCPCONSUMERMAXCONNECTIONATTEMPTS; ///< The maximum allowed number of failed connection attempts of a client
	 unsigned short m_port = GASIOTCPCONSUMERDEFAULTPORT; ///< The port on which the server is supposed to listen
	 std::string m_server = GASIOTCPCONSUMERDEFAULTSERVER;  ///< The name or ip if the server
	 std::chrono::duration<double> m_timeout = std::chrono::milliseconds(200); ///< A timeout for put- and get-operations
	 Gem::Common::GThreadGroup m_gtg; ///< Holds listener threads
	 Gem::Common::GThreadPool m_gtp; ///< Holds workers sorting processed items back into the broker
	 std::shared_ptr<Gem::Courtier::GBrokerT<processable_type>> m_broker_ptr; ///< A pointer to the global broker
	 std::atomic<std::int_fast32_t> m_connections{0}; ///< Holds the current number of open connections
};

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/

} /* namespace Courtier */
} /* namespace Gem */

#endif /* GASIOASYNCTCPCONSUMERT_HPP */
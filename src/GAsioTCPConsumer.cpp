/**
 * \file
 */

/**
 * Copyright (C) 2004-2008 Dr. Ruediger Berlich
 * Copyright (C) 2008 Forschungszentrum Karlsruhe GmbH
 *
 * This file is part of the Geneva library.
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

#include "GAsioTCPConsumer.hpp"

namespace GenEvA
{

/*********************************************************************/
/**
 * Standard constructor. Its main purpose it to initialize the underlying
 * socket. We do not initialize the various character arrays and strings,
 * as they are overwritten for each call to this class.
 *
 * \param io_service A reference to the server's io_service
 */
GAsioServerSession::GAsioServerSession(boost::asio::io_service& io_service)
 	:socket_(io_service)
{ /* nothing */ }

/*********************************************************************/
/**
 * A standard destructor. Closes the socket.
 */
GAsioServerSession::~GAsioServerSession(){
	socket_.close();
}

/*********************************************************************/
/**
 * Returns the socket used by this object
 *
 * \return The socket used by this object
 */
boost::asio::ip::tcp::socket& GAsioServerSession::socket(void){
    return socket_;
}

/*********************************************************************/
/**
 * Retrieve a single command from the stream. It will afterwards have
 * been removed from the stream.
 *
 * \return A single command that has been retrieved from a network socket
 */
string GAsioServerSession::getSingleCommand(void){
	char inbound_command[command_length];

    // Read a command from the socket. This will remove it from the stream.
    boost::asio::read(socket_, boost::asio::buffer(inbound_command));

    // Remove all leading or trailing white spaces from the command.
    return boost::algorithm::trim_copy(std::string(inbound_command, command_length));
}

/*********************************************************************/
/**
 * Write a single command to a socket.
 *
 * \param command A command to be written to a socket
 */
void GAsioServerSession::sendSingleCommand(const string& command){
	// Format the command ...
	string outbound_command = assembleQueryString(command, command_length);
	// ... and tell the client
	boost::asio::write(socket_, boost::asio::buffer(outbound_command));
}

/*********************************************************************/
/**
 * Retrieves an item from the client (i.e. the socket).
 *
 * \param item An item to be retrieved from the socket
 * \return true if successful, otherwise false
 */
bool GAsioServerSession::retrieve(string& item){
	char inbound_header[command_length];

	// First read the header from socket_
	boost::asio::read(socket_, boost::asio::buffer(inbound_header));

	// And transform to an integer
	uint16_t dataSize;
	try{
		dataSize=lexical_cast<int16_t>(boost::algorithm::trim_copy(std::string(inbound_header, command_length)));
	}
	catch(boost::bad_lexical_cast& e){
		// tried to put an unknown key back - needs to be logged ...
		GLogStreamer gls;
		gls << "GAsioServerSession::retrieve(string&):" << endl
			<< "Conversion of dataSize failed with message" << endl
			<< e.what() << endl
			<< logLevel(UNCRITICAL);

		return false;
	}

	// Read inbound data
	std::vector<char> inboundData(dataSize);
	boost::asio::read(socket_, boost::asio::buffer(inboundData));

	// Transfer data into a string
	ostringstream oss;
	for(vector<char>::iterator it=inboundData.begin(); it != inboundData.end(); ++it) oss << *it;
	item = oss.str();

	return true;
}

/*********************************************************************/
/**
 * Submit an item to the client (i.e. the socket).
 *
 * \param item An item to be written to the socket
 * \return true if successful, otherwise false
 */
bool GAsioServerSession::submit(const string& item){
	// Format the command
	string outbound_command = assembleQueryString("compute", command_length);

	// Format the size header
	string outbound_header = assembleQueryString(lexical_cast<string>(item.size()), command_length);

	// Write the serialized data to the socket. We use "gather-write" to send
	// command, header and data in a single write operation.
	std::vector<boost::asio::const_buffer> buffers;

	buffers.push_back(boost::asio::buffer(outbound_command));
	buffers.push_back(boost::asio::buffer(outbound_header));
	buffers.push_back(boost::asio::buffer(item));

	boost::asio::write(socket_, buffers);

	return true;
}

/*********************************************************************/
///////////////////////////////////////////////////////////////////////
/*********************************************************************/
/**
 * Standard constructor. Initializes the acceptor with the ioservice
 * and the port.
 *
 * \param port The port through which clients can access the server
 */
GAsioTCPConsumer::GAsioTCPConsumer(uint8_t port)
	:GConsumer(),
	 _acceptor(_io_service, tcp::endpoint(tcp::v4(), port)),
	 tp(GASIOTCPCONSUMERTHREADS)
{
	GLogStreamer gls;
	gls << "GAsioTCPConsumer::GAsioTCPConsumer() : Running on port " << port << endl
		<< logLevel(INFORMATIONAL);

	// We can start the actual processing. All real work is done in the GAsioServerSession class .
	// Note that a pointer to our class will be handed down to all GAsioServerSessions to come, so they
	// can inform us when a stop condition was reached.
	shared_ptr<GAsioServerSession> newSession(new GAsioServerSession(_io_service));
	_acceptor.async_accept(newSession->socket(),
			boost::bind(&GAsioTCPConsumer::handleAccept, this, newSession, boost::asio::placeholders::error));
}

/*********************************************************************/
/**
 * A standard destructor. We have no local, dynamically allocated data,
 * hence it is empty.
 */
GAsioTCPConsumer::~GAsioTCPConsumer()
{ /* nothing */ }

/*********************************************************************/
/**
 * Handles a new connection request from a client.
 *
 * \param currentSession A pointer to the current session
 * \param gmb A pointer to the GMemberBroker, so GAsioServerSession can access the data
 * \param error Possible error conditions
 */
void GAsioTCPConsumer::handleAccept(shared_ptr<GAsioServerSession> currentSession,
											const boost::system::error_code& error)
{
	// First check whether the stop condition was reached or an error occurred in the
	// previous call. If so, we return immediately and thus interrupt the restart of the
	// listeners. _io_service then runs out of work, and the main loop terminates
	if (!error && !stopConditionReached()){
	    // First we make sure a new session is started asynchronously so the next request can be served
	    shared_ptr<GAsioServerSession> newSession(new GAsioServerSession(_io_service));
	    _acceptor.async_accept(newSession->socket(),
	    	                   boost::bind(&GAsioTCPConsumer::handleAccept, this, newSession,
                		   	   boost::asio::placeholders::error));

	    // Now we can dispatch the actual session code to our thread pool
	    tp.schedule(boost::bind(&GAsioServerSession::processRequest, currentSession));
	}
	else{
		// tp.shutdown(); // terminate the threadpool  Wie ???
		return;
	}
}

/*********************************************************************/
/**
 * Initialization code. Not needed here, hence it is empty.
 */
void GAsioTCPConsumer::init(void)
{ /* nothing */ }

/*********************************************************************/
/**
 * The function used as the basis of the GMemberBroker's consumer thread.
 *
 * \param gmb A pointer to the GMemberBroker object that initiated the thread
 */
void GAsioTCPConsumer::customProcess(void){
	// The io_service's event loop
	_io_service.run();
}

/*********************************************************************/
/**
 * Finalization code. Not needed here, hence empty.
 */
void GAsioTCPConsumer::finally(void)
{ /* nothing */ }

/*********************************************************************/

} /* namespace GenEvA */

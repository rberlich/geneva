/**
 * @file GExceptions.cpp
 */

/*
 * Copyright (C) Gemfony scientific UG (haftungsbeschraenkt)
 *
 * This file is part of the Geneva library collection.
 *
 * The following license applies to the code in this file:
 *
 * ***************************************************************************
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * ***************************************************************************
 *
 * NOTE THAT THE BOOST-LICENSE DOES NOT APPLY TO ANY OTHER FILES OF THE
 * GENEVA LIBRARY, UNLESS THIS IS EXPLICITLY STATED IN THE CORRESPONDING FILE!
 *
 * See the AUTHORS file in the top-level directory for a list of authors.
 *
 * Contact: contact [at] gemfony (dot) eu
 *
 * Geneva was developed with kind support from Karlsruhe Institute of
 * Technology (KIT) and Steinbuch Centre for Computing (SCC). Further
 * information about KIT and SCC can be found at http://www.kit.edu/english
 * and http://scc.kit.edu .
 *
 * Geneva is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Boost Software License for more details.
 *
 * For further information on Gemfony scientific and Geneva, visit
 * http://www.gemfony.eu .
 */

#include "common/GExceptions.hpp"

namespace Gem {
namespace Common {

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * The standard constructor
 */
gemfony_error_condition::gemfony_error_condition(const std::string &description) throw()
	: description_(description) { /* nothing */ }

/******************************************************************************/
/**
 * The destructor
 */
gemfony_error_condition::~gemfony_error_condition()  throw() { /* nothing */ }

/******************************************************************************/
/**
 * Allows to add further information to the exception
 */
void gemfony_error_condition::add(const std::string &newMessage) throw() {
	description_ += newMessage;
}

/******************************************************************************/
/**
 * Allows to add further informtion, automatically terminated by a '\n'.
 * The intended use for this feature is catching and throwing exceptions in
 * an easy way, e.g. in order to create a stack trace:
 *
 * try {
 *   // some condition
 * } catch(gemfony_error_condition& g) {
 *    throw g()
 * }
 */
gemfony_error_condition &gemfony_error_condition::operator()(
	const std::string &s
) throw() {
	this->add(s + "\n");
	return *this;
}

/******************************************************************************/
/**
 * Emits information when thrown
 */
const char *gemfony_error_condition::what() const throw() {
	return description_.c_str();
}

/******************************************************************************/
/**
 * Allows to check whether any information is stored in this class
 */
bool gemfony_error_condition::empty() const throw() {
	return description_.empty();
}

/******************************************************************************/
/**
 * This function allows to output a gemfony_error_condition to a stream
 */
std::ostream &operator<<(std::ostream &o, const Gem::Common::gemfony_error_condition &g) {
	o << g.what();
	return o;
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * The standard constructor
 */
g_expectation_violation::g_expectation_violation(const std::string &description) throw()
	: gemfony_error_condition(description) { /* nothing */ }

/******************************************************************************/
/**
 * The destructor
 */
g_expectation_violation::~g_expectation_violation() throw() { /* nothing */ }

/******************************************************************************/
/**
 * Allows to add further informtion, automatically terminated by a '\n'.
 * The intended use for this feature is catching and throwing exceptions in
 * an easy way, e.g. in order to create a stack trace:
 *
 * try {
 *   // some condition
 * } catch(g_expectation_violation& g) {
 *    throw g()
 * }
 */
g_expectation_violation &g_expectation_violation::operator()(
	const std::string &s
) throw() {
	this->add(s + "\n");
	return *this;
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/

} /* namespace Common */
} /* namespace Gem */

/******************************************************************************/
/**
 * Raise an exception if a given define wasn't set. "F" stands for "function",
 * "D" for "define".
 */
void condnotset(const std::string &F, const std::string &D) {
	std::ostringstream error;
	error
	<< std::endl
	<< "================================================" << std::endl
	<< "In function " << F << " Error!" << std::endl
	<< "Function was called even though " << D << " hasn't been set." << std::endl
	<< "================================================" << std::endl;                               \
        throw(Gem::Common::gemfony_error_condition(error.str()));
}

/******************************************************************************/

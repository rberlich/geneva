/**
 * @file GFormulaParserT.cpp
 */

/*
 * Copyright (C) Gemfony scientific UG (haftungsbeschraenkt)
 *
 * This file is part of the Geneva library collection.
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

/*
 * This code is based on a number of examples shipped with the Boost.Spirit
 * library, particularly "calc6.cpp" from Boost 1.54. Consequently, the following
 * license applies to the code in this file:
 *
 *******************************************************************************
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
 *******************************************************************************
 *
 * NOTE THAT THE BOOST-LICENSE DOES NOT APPLY TO ANY OTHER FILES OF THE
 * GENEVA LIBRARY, UNLESS THIS IS EXPLICITLY STATED IN THE CORRESPONDING FILE.
 */

#include "common/GFormulaParserT.hpp"

namespace Gem {
namespace Common {

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * The standard constructor
 */
math_logic_error::math_logic_error(const std::string &errorText) throw()
	: gemfony_exception(errorText) { /* nothing */ }

/******************************************************************************/
/**
 * The destructor
 */
math_logic_error::~math_logic_error()  throw() { /* nothing */ }

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * The standard constructor
 */
division_by_0::division_by_0() throw()
	: math_logic_error("Division by 0 in Geneva formula parser") { /* nothing */ }

/******************************************************************************/
/**
 * The destructor
 */
division_by_0::~division_by_0() throw() { /* nothing */ }

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
// Some swap functions needed to deal with MSVC peculiarities

// Swaps two nil structs
void nil::swap(nil &n) { }

// Swaps two signed_ structs
void signed_::swap(signed_ &s) {
	std::swap(sign, s.sign);
	std::swap(operand_, s.operand_);
}

// Swaps two operation structs
void operation::swap(operation &o) {
	std::swap(operator_, o.operator_);
	std::swap(operand_, o.operand_);
}

// Swaps two unary_function_ structs
void unary_function_::swap(unary_function_ &u) {
	std::swap(fname_, u.fname_);
	std::swap(operand_, u.operand_);
}

// Swaps two binary_function_ structs
void binary_function_::swap(binary_function_ &b) {
	std::swap(fname_, b.fname_);
	std::swap(operand1_, b.operand1_);
	std::swap(operand2_, b.operand2_);
}

// Swaps two ast_expression structs
void ast_expression::swap(ast_expression &a) {
	std::swap(first, a.first);
	std::swap(rest, a.rest);
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/

} /* namespace Common */
} /* namespace Gem */

// Needed for rules to work. Follows http://boost.2283326.n4.nabble.com/hold-multi-pass-backtracking-swap-compliant-ast-td4664679.html
namespace boost {
namespace spirit {

void swap(Gem::Common::nil &a, Gem::Common::nil &b) {
	a.swap(b);
}

void swap(Gem::Common::signed_ &a, Gem::Common::signed_ &b) {
	a.swap(b);
}

void swap(Gem::Common::operation &a, Gem::Common::operation &b) {
	a.swap(b);
}

void swap(Gem::Common::unary_function_ &a, Gem::Common::unary_function_ &b) {
	a.swap(b);
}

void swap(Gem::Common::binary_function_ &a, Gem::Common::binary_function_ &b) {
	a.swap(b);
}

void swap(Gem::Common::ast_expression &a, Gem::Common::ast_expression &b) {
	a.swap(b);
}

} /* namespace spirit */
} /* namespace boost */


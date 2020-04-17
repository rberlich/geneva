/*
 * This file is part of the Geneva library collection.
 *
 * This code is based on a number of examples shipped with the
 * Boost.Spirit library, particularly "calc6.cpp" from Boost 1.54.
 * It is consequently covered by the Boost license v.1.0, as quoted below.
 *
 * See the NOTICE file in the top-level directory of the Geneva library
 * collection for a list of contributors and copyright information.
 *
 * The following license applies to the code IN THIS FILE:
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
 */

#pragma once

// Global checks, defines and includes needed for all of Geneva
#include "common/GGlobalDefines.hpp"

// Standard headers go here
#include <iostream>
#include <string>
#include <stack>
#include <map>
#include <cmath>
#include <type_traits>

// Boost headers go here

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi_operator.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_string.hpp>
#include <boost/spirit/include/qi_numeric.hpp>
#include <boost/spirit/include/qi_auxiliary.hpp>
#include <boost/spirit/include/qi_nonterminal.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <boost/variant.hpp>
#include <boost/utility.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/adapted/std_tuple.hpp> // Compare http://stackoverflow.com/questions/18158376/getting-boostspiritqi-to-use-stl-containers

// Geneva headers go here
#include "common/GExceptions.hpp"
#include "common/GLogger.hpp"
#include "common/GErrorStreamer.hpp"
#include "common/GCommonHelperFunctionsT.hpp"
#include "common/GCommonMathHelperFunctions.hpp"

namespace Gem {
namespace Common {

/******************************************************************************/
// Exceptions for some error conditions

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * An exception to be thrown in case of mathematical errors,
 * such as division by 0
 */
class math_logic_error : public gemfony_exception {
public:
	/** @brief The default constructor: Intentionally deleted */
	G_API_COMMON math_logic_error() = delete;
	/** @brief The standard constructor */
	explicit G_API_COMMON math_logic_error(std::string const&) noexcept;

	/**************************************************************************/
	// Defaulted functions, constructors and destructor; rule of five

	G_API_COMMON math_logic_error(math_logic_error const&) = default;
	G_API_COMMON math_logic_error(math_logic_error &&) noexcept = default;
	G_API_COMMON ~math_logic_error() noexcept override = default;

	G_API_COMMON math_logic_error& operator=(math_logic_error const&) = default;
	G_API_COMMON math_logic_error& operator=(math_logic_error &&) noexcept = default;
};

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * An exception indicating a division by 0
 */
class division_by_0 : public math_logic_error {
public:
	/** @brief The default constructor */
	G_API_COMMON division_by_0() noexcept;

	/**************************************************************************/
	// Defaulted functions, constructors and destructor; rule of five

	G_API_COMMON division_by_0(division_by_0 const&) = default;
	G_API_COMMON division_by_0(division_by_0 &&) noexcept = default;
	G_API_COMMON ~division_by_0() noexcept override = default;

	G_API_COMMON division_by_0& operator=(division_by_0 const&) = default;
	G_API_COMMON division_by_0& operator=(division_by_0 &&) noexcept = default;
};

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * An exception indicating a range outside [-1:1] in acos
 */
template<typename fp_type>
class acos_invalid_range : public math_logic_error {
public:
	/** @brief The default constructor: Intentionally deleted */
	acos_invalid_range() = delete;

	/** @brief The standard constructor */
	explicit acos_invalid_range(const fp_type &val) noexcept
		: math_logic_error(std::string("acos: Value ") + Gem::Common::to_string(val) +
								 std::string(" out of valid range [-1:1] in GFormulaParserT"))
	{ /* nothing */ }

	/**************************************************************************/
	// Defaulted functions, constructors and destructor; rule of five

	G_API_COMMON acos_invalid_range(acos_invalid_range const&) = default;
	G_API_COMMON acos_invalid_range(acos_invalid_range &&) noexcept = default;
	G_API_COMMON ~acos_invalid_range() noexcept override = default;

	G_API_COMMON acos_invalid_range& operator=(acos_invalid_range const&) = default;
	G_API_COMMON acos_invalid_range& operator=(acos_invalid_range &&) noexcept = default;
};

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * An exception indicating a range outside [-1:1] in acos
 */
template<typename fp_type>
class asin_invalid_range : public math_logic_error {
public:
	/** @brief The default constructor: Intentionally deleted */
	asin_invalid_range() = delete;
	/** @brief The standard constructor */
	explicit asin_invalid_range(const fp_type &val) noexcept
		: math_logic_error(std::string("asin: Value ") + Gem::Common::to_string(val) +
								 std::string(" out of valid range [-1:1] in GFormulaParserT")) { /* nothing */ }

	/**************************************************************************/
	// Defaulted functions, constructors and destructor; rule of five

	G_API_COMMON asin_invalid_range(asin_invalid_range const&) = default;
	G_API_COMMON asin_invalid_range(asin_invalid_range &&) noexcept = default;
	G_API_COMMON ~asin_invalid_range() noexcept override = default;

	G_API_COMMON asin_invalid_range& operator=(asin_invalid_range const&) = default;
	G_API_COMMON asin_invalid_range& operator=(asin_invalid_range &&) noexcept = default;
};

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * An exception indicating a value <= 0
 */

template<typename fp_type>
class log_negative_value : public math_logic_error {
public:
	/** @brief The default constructor: Intentionally deleted */
	log_negative_value() = delete;
	/** @brief The standard constructor */
	explicit log_negative_value(const fp_type &val) noexcept
		: math_logic_error(std::string("log: Value ") + Gem::Common::to_string(val) +
								 std::string(" <= 0 in GFormulaParserT"))
	{ /* nothing */ }

	/**************************************************************************/
	// Defaulted functions, constructors and destructor; rule of five

	G_API_COMMON log_negative_value(log_negative_value const&) = default;
	G_API_COMMON log_negative_value(log_negative_value &&) noexcept = default;
	G_API_COMMON ~log_negative_value() noexcept override = default;

	G_API_COMMON log_negative_value& operator=(log_negative_value const&) = default;
	G_API_COMMON log_negative_value& operator=(log_negative_value &&) noexcept = default;
};

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * An exception indicating a value <= 0
 */
template<typename fp_type>
class log10_negative_value : public math_logic_error {
public:
	/** @brief The default constructor: Intentionally deleted */
	log10_negative_value() = delete;
	/** @brief The standard constructor */
	explicit log10_negative_value(const fp_type &val) noexcept
		: math_logic_error(std::string("log10: Value ") + Gem::Common::to_string(val) +
								 std::string(" <= 0  in GFormulaParserT"))
	{ /* nothing */ }

	/**************************************************************************/
	// Defaulted functions, constructors and destructor; rule of five

	G_API_COMMON log10_negative_value(log10_negative_value const&) = default;
	G_API_COMMON log10_negative_value(log10_negative_value &&) noexcept = default;
	G_API_COMMON ~log10_negative_value() noexcept override = default;

	G_API_COMMON log10_negative_value& operator=(log10_negative_value const&) = default;
	G_API_COMMON log10_negative_value& operator=(log10_negative_value &&) noexcept = default;
};

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * An exception indicating a value <= 0
 */
template<typename fp_type>
class sqrt_negative_value : public math_logic_error {
public:
	/** @brief The default constructor: Intentionally deleted */
	sqrt_negative_value() = delete;
	/** @brief The standard constructor */
	explicit sqrt_negative_value(const fp_type &val) noexcept
		: math_logic_error(std::string("sqrt: Value ") + Gem::Common::to_string(val) +
								 std::string(" < 0  in GFormulaParserT")) { /* nothing */ }

	/**************************************************************************/
	// Defaulted functions, constructors and destructor; rule of five

	G_API_COMMON sqrt_negative_value(sqrt_negative_value const&) = default;
	G_API_COMMON sqrt_negative_value(sqrt_negative_value &&) noexcept = default;
	G_API_COMMON ~sqrt_negative_value() noexcept override = default;

	G_API_COMMON sqrt_negative_value& operator=(sqrt_negative_value const&) = default;
	G_API_COMMON sqrt_negative_value& operator=(sqrt_negative_value &&) noexcept = default;
};

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
// The Abstract Syntax Tree + access functions

// Forward declarations
struct nil;
struct signed_;
struct unary_function_;
struct binary_function_;
struct ast_expression;

using operand =
boost::variant<
	nil
	, float
	, double
	, boost::recursive_wrapper<signed_>
	, boost::recursive_wrapper<unary_function_>
	, boost::recursive_wrapper<binary_function_>
	, boost::recursive_wrapper<ast_expression>
>;

struct nil {
	void swap(nil &);
};

struct signed_ {
	char sign;
	operand operand_;

	void swap(signed_ &);
};

struct operation {
	char operator_;
	operand operand_;

	void swap(operation &);
};

struct unary_function_ {
	std::string fname_;
	operand operand_;

	void swap(unary_function_ &);
};

struct binary_function_ {
	std::string fname_;
	operand operand1_;
	operand operand2_;

	void swap(binary_function_ &);
};

struct ast_expression {
	operand first;
	std::list<operation> rest;

	void swap(ast_expression &);
};

/** @brief print function for debugging */
inline std::ostream &operator<<(std::ostream &out, nil) {
	out << "nil";
	return out;
}

} /* namespace Common */
} /* namespace Gem */

BOOST_FUSION_ADAPT_STRUCT(
	Gem::Common::signed_,
	(char, sign)
		(Gem::Common::operand, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
	Gem::Common::operation,
	(char, operator_)
		(Gem::Common::operand, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
	Gem::Common::unary_function_,
	(std::string, fname_)
		(Gem::Common::operand, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
	Gem::Common::binary_function_,
	(std::string, fname_)
		(Gem::Common::operand, operand1_)
		(Gem::Common::operand, operand2_)
)

BOOST_FUSION_ADAPT_STRUCT(
	Gem::Common::ast_expression,
	(Gem::Common::operand, first)
		(std::list<Gem::Common::operation>, rest)
)

namespace Gem {
namespace Common {

/******************************************************************************/
/**
 * This class allows to parse and evaluate simple mathematical formulas of the
 * type "(sin(3.)*sqrt(5.) - (2*pi))^2". Formulas may optionally contain place
 * holders for variables, e.g. "(sin({{var1}})*sqrt({{var2}}) - ({{var3}}*pi))^2".
 * Formulas are provided in string form to the constructor. The evaluate()
 * function will then replace the place-holders with the corresponding entries of
 * a std::map<std::string, std::vector<fp_type>>. For simple variable names such
 * as "var2" only the first value of the std::vector is used -- a notation such as
 * "var3{2]" is also possible -- in this case the third value of the vector will be
 * used. An exception will be thrown, if the vector doesn't have enough entries.
 * An object of this class may deal with a single formula only, which is
 * given to it through the constructor. When a formula cannot be parsed,
 * an exception will be thrown. Likewise, exceptions derived from "Gem::Common::math_logic_error"
 * will be thrown for common mathematical errors, such as division by 0 or sqrt(-1).
 * Note that the class currently only handles floating point values (float and double).
 * The class is based on a number of examples taken from the Boost.Spirit code base,
 * particularly calc6.cpp from Boost version 1.54.
 */
/*****************************************************************************/
/**
 * The actual formula parser
 */
template<typename fp_type>
class GFormulaParserT
	: public boost::spirit::qi::grammar<std::string::const_iterator, ast_expression(), boost::spirit::ascii::space_type> {
	// Make sure, fp_type is a floating point value
	static_assert(std::is_floating_point<fp_type>::value, "fp_type should ne a floating point type");

public:
	/*****************************************************************************/
	/**
	 * Specifies the operations the parser must know about
	 */
	enum class byte_code : Gem::Common::ENUMBASETYPE {
		op_trap = 0,      // triggers an exception --> boost::variant<int,fp_type>() == 0
		op_neg = 1,      // negate the top stack entry
		op_add = 2,      // add top two stack entries
		op_sub = 3,      // subtract top two stack entries
		op_mul = 4,      // multiply top two stack entries
		op_div = 5,      // divide top two stack entries
		op_acos = 7,      // Calculates the acos value of the top-most stack-entry
		op_asin = 8,      // Calculates the asin value of the top-most stack-entry
		op_atan = 9,      // Calculates the atan value of the top-most stack-entry
		op_ceil = 10,     // Calculates the ceil value of the top-most stack-entry
		op_cos = 11,     // Calculates the cos value of the top-most stack-entry
		op_cosh = 12,     // Calculates the cosh value of the top-most stack-entry
		op_exp = 13,     // Calculates the exp value of the top-most stack-entry
		op_fabs = 14,     // Calculates the fabs value of the top-most stack-entry
		op_floor = 15,     // Calculates the floor value of the top-most stack-entry
		op_log = 16,     // Calculates the log value of the top-most stack-entry
		op_log10 = 17,     // Calculates the log10 value of the top-most stack-entry
		op_sin = 18,     // Calculates the sin value of the top-most stack-entry
		op_sinh = 19,     // Calculates the sinh value of the top-most stack-entry
		op_sqrt = 20,     // Calculates the sqrt value of the top-most stack-entry
		op_tan = 21,     // Calculates the tan value of the top-most stack-entry
		op_tanh = 22,     // Calculates the tanh value of the top-most stack-entry
		op_pow = 23,     // Calculates the pow value of the two top-most stack-entries
		op_hypot = 24,     // Calculates the hypot value of the two top-most stack-entries
		op_min = 25,     // Calculates the min value of the two top-most stack-entries
		op_max = 26,     // Calculates the max value of the two top-most stack-entries
		op_fp = 27,     // Pushes a fp_type onto the stack
	};

	using result_type = void; // Needed for the operator() and apply_visitor
	using codeEntry = boost::variant<byte_code, fp_type>;
	using parameter_map = std::map<std::string, std::vector<fp_type>>;
	using constants_map = std::map<std::string, fp_type>;

	/***************************************************************************/
	/** @brief The default constructor -- intentionally deleted */
	GFormulaParserT() = delete;

	/***************************************************************************/
	/**
	 * The standard constructor
	 */
	explicit GFormulaParserT(
		const std::string &formula
		, const constants_map &user_constants = constants_map()
	)
		: GFormulaParserT::base_type(expression_rule_)
		, raw_formula_(formula)
		, stack_(4096)
		, stack_ptr_(stack_.begin())
		, printCode_(false)
	{
		boost::spirit::qi::char_type char_;
		boost::spirit::qi::string_type string_;

		using boost::spirit::qi::on_error;
		using boost::spirit::qi::fail;

		//---------------------------------------------------------------------------
		// Define a number of mathematical constants
		constants_.add
			("e", boost::math::constants::e<fp_type>())
			("pi", boost::math::constants::pi<fp_type>());

		// Add user-defined constants
		typename constants_map::const_iterator cit;
		for (cit = user_constants.begin(); cit != user_constants.end(); ++cit) {
			constants_.add(cit->first, cit->second);
		}

		//---------------------------------------------------------------------------
		// Define the actual grammar
		expression_rule_ =
			term_rule_ >> *((char_('+') > term_rule_) | (char_('-') > term_rule_));

		term_rule_ =
			factor_rule_ >> *((char_('*') > factor_rule_) | (char_('/') > factor_rule_));

		unary_function_rule_ =
			(string_("acos") > '(' > expression_rule_ > ')')
			| (string_("asin") > '(' > expression_rule_ > ')')
			| (string_("atan") > '(' > expression_rule_ > ')')
			| (string_("ceil") > '(' > expression_rule_ > ')')
			| (string_("cosh") > '(' > expression_rule_ > ')')
			| (string_("cos") > '(' > expression_rule_ > ')')
			| (string_("exp") > '(' > expression_rule_ > ')')
			| (string_("fabs") > '(' > expression_rule_ > ')')
			| (string_("floor") > '(' > expression_rule_ > ')')
			| (string_("log10") > '(' > expression_rule_ > ')')
			| (string_("log") > '(' > expression_rule_ > ')')
			| (string_("sinh") > '(' > expression_rule_ > ')')
			| (string_("sin") > '(' > expression_rule_ > ')')
			| (string_("sqrt") > '(' > expression_rule_ > ')')
			| (string_("tanh") > '(' > expression_rule_ > ')')
			| (string_("tan") > '(' > expression_rule_ > ')');

		binary_function_rule_ =
			(string_("min") > '(' > expression_rule_ > ',' > expression_rule_ > ')')
			| (string_("max") > '(' > expression_rule_ > ',' > expression_rule_ > ')')
			| (string_("pow") > '(' > expression_rule_ > ',' > expression_rule_ > ')')
			| (string_("hypot") > '(' > expression_rule_ > ',' > expression_rule_ > ')');

		factor_rule_ =
			real
			| ('(' > expression_rule_ > ')')
			| (char_('-') > factor_rule_)
			| (char_('+') > factor_rule_)
			| unary_function_rule_
			| binary_function_rule_
			| constants_;

		//---------------------------------------------------------------------------
		// Debugging and error handling and reporting support.
		BOOST_SPIRIT_DEBUG_NODES(
			(expression_rule_)
				(term_rule_)
				(unary_function_rule_)
				(binary_function_rule_)
				(factor_rule_)
		);

		// Error handling
		{
			namespace qi = boost::spirit::qi;
			namespace ascii = boost::spirit::ascii;
			namespace phoenix = boost::phoenix;

			using qi::eps;
			using qi::lit;
			using qi::_val;
			using qi::_2;
			using qi::_3;
			using qi::_4;
			using ascii::char_;
			using qi::on_error;
			using qi::fail;
			using phoenix::construct;
			using phoenix::val;

			on_error<fail>
				(
					// start
					expression_rule_, phoenix::ref(std::cout)
											<< "Error! Was expecting " << qi::_4
											<< " here: '" << phoenix::construct<std::string>(qi::_3, qi::_2) << "'\n"
				);
		}
	}

	/***************************************************************************/
	// Deleted copy and assignment / rule of five

	GFormulaParserT(GFormulaParserT<fp_type> const&) = delete;
	GFormulaParserT(GFormulaParserT<fp_type> &&) = delete;

	GFormulaParserT<fp_type>& operator=(GFormulaParserT<fp_type> const&) = delete;
	GFormulaParserT<fp_type>& operator=(GFormulaParserT<fp_type> &&) = delete;

	/***************************************************************************/
	/**
	 * When set to true, the code-vector will be printed prior to the evaluation
	 */
	void setPrintCode(bool printCode) {
		printCode_ = printCode;
	}

	/***************************************************************************/
	/**
	 * Retrieves the processed formula (after replacement of place-holders)
	 *
	 * @param placeHolders A list of place-holders for variable values
	 * @return A string containing the processed formula
	 */
	std::string getFormula(const parameter_map &vm) const {
		return this->replacePlaceHolders(vm);
	}

	/***************************************************************************/
	/**
	 * Evaluates a formula after replacing place holders with values
	 */
	fp_type evaluate(const parameter_map &vm = parameter_map()) const {
		// Clear local data structures
		code_.clear();
		stack_ptr_ = stack_.begin();

		ast_expression ast; ///< The abstract syntax tree

		// Replace place holders with values taken from the map
		std::string formula = this->replacePlaceHolders(vm);

		// Do the actual parsing of the formula
		std::string::const_iterator iter = formula.begin();
		std::string::const_iterator end = formula.end();
		boost::spirit::ascii::space_type space;
		bool r = boost::spirit::qi::phrase_parse(iter, end, *this, space, ast);

		if (r && iter == end) {
			this->compile(ast);
			this->execute();
		} else {
			std::string rest(iter, end);

			throw gemfony_exception(
				g_error_streamer(DO_LOG, time_and_place)
					<< "In GFormulaParserT<>::evaluate(): Error!" << std::endl
					<< "Parsing of formula " << formula << " failed at " << rest << std::endl
			);
		}

		return stack_.at(0);
	}

	/*****************************************************************************/
	/**
	 * Ease of access to the evaluate function
	 */
	fp_type operator()(const parameter_map &vm = parameter_map()) const {
		return this->evaluate(vm);
	}

	/*****************************************************************************/
	// Code for the compilation of the AST
	void operator()(nil) const { BOOST_ASSERT(0); }

	void operator()(const fp_type &fp_val) const {
		code_.push_back(codeEntry(byte_code::op_fp));
		code_.push_back(codeEntry(fp_val));
	}

	void operator()(const operation &x) const {
		boost::apply_visitor(*this, x.operand_);

		if (x.operator_ == '+') code_.push_back(codeEntry(byte_code::op_add));
		else if (x.operator_ == '-') code_.push_back(codeEntry(byte_code::op_sub));
		else if (x.operator_ == '*') code_.push_back(codeEntry(byte_code::op_mul));
		else if (x.operator_ == '/')
			code_.push_back(codeEntry(byte_code::op_div));  // division by 0 throws Gem::Common::division_by_0 exception
		else
			BOOST_ASSERT(0);
	}

	void operator()(const unary_function_ &f) const {
		boost::apply_visitor(*this, f.operand_);

		if (f.fname_ == "acos")
			code_.push_back(codeEntry(byte_code::op_acos)); // Value out of valid range [-1,1] throws Gem::Common::acos_invalid_range
		else if (f.fname_ == "asin")
			code_.push_back(codeEntry(byte_code::op_asin)); // Value out of valid range [-1,1] throws Gem::Common::asin_invalid_range
		else if (f.fname_ == "atan") code_.push_back(codeEntry(byte_code::op_atan));
		else if (f.fname_ == "ceil") code_.push_back(codeEntry(byte_code::op_ceil));
		else if (f.fname_ == "cos") code_.push_back(codeEntry(byte_code::op_cos));
		else if (f.fname_ == "cosh") code_.push_back(codeEntry(byte_code::op_cosh));
		else if (f.fname_ == "exp") code_.push_back(codeEntry(byte_code::op_exp));
		else if (f.fname_ == "fabs") code_.push_back(codeEntry(byte_code::op_fabs));
		else if (f.fname_ == "floor") code_.push_back(codeEntry(byte_code::op_floor));
		else if (f.fname_ == "log")
			code_.push_back(codeEntry(byte_code::op_log)); // Value <= 0 throws Gem::Common::log_negative_value
		else if (f.fname_ == "log10")
			code_.push_back(codeEntry(byte_code::op_log10)); // Value <= 0 throws Gem::Common::log10_negative_value
		else if (f.fname_ == "sin") code_.push_back(codeEntry(byte_code::op_sin));
		else if (f.fname_ == "sinh") code_.push_back(codeEntry(byte_code::op_sinh));
		else if (f.fname_ == "sqrt")
			code_.push_back(codeEntry(byte_code::op_sqrt)); // Value < 0 throws Gem::Common::sqrt_negative_value
		else if (f.fname_ == "tan") code_.push_back(codeEntry(byte_code::op_tan));
		else if (f.fname_ == "tanh") code_.push_back(codeEntry(byte_code::op_tanh));
		else
			BOOST_ASSERT(0);
	}

	void operator()(const binary_function_ &f) const {
		boost::apply_visitor(*this, f.operand1_);
		boost::apply_visitor(*this, f.operand2_);

		if (f.fname_ == "min") code_.push_back(codeEntry(byte_code::op_min));
		else if (f.fname_ == "max") code_.push_back(codeEntry(byte_code::op_max));
		else if (f.fname_ == "pow") code_.push_back(codeEntry(byte_code::op_pow));
		else if (f.fname_ == "hypot") code_.push_back(codeEntry(byte_code::op_hypot));
		else
			BOOST_ASSERT(0);
	}

	void operator()(const signed_ &x) const {
		boost::apply_visitor(*this, x.operand_);
		if (x.sign == '-') code_.push_back(codeEntry(byte_code::op_neg));
		else if (x.sign == '+') { /* nothing */ }
		else
			BOOST_ASSERT(0);
	}

	void operator()(const ast_expression &x) const {
		boost::apply_visitor(*this, x.first);
		for(const auto& oper: x.rest) {
			(*this)(oper);
		}
	}

private:
	/***************************************************************************/
	/**
	 * Replaces place holders with corresponding parameter values
	 *
	 * @param vm A std::map of name-value pairs, holding place-holders to be replaced with values
	 */
	std::string replacePlaceHolders(const parameter_map &vm) const {
		std::string formula = raw_formula_;
		std::string key, value;
		boost::xpressive::sregex re;

		typename parameter_map::const_iterator cit;
		for (cit = vm.begin(); cit != vm.end(); ++cit) {
			key = cit->first;

			if (1 == (cit->second).size()) { // Try just the key
				value = Gem::Common::to_string((cit->second).at(0));
				re = boost::xpressive::as_xpr("{{" + key + "}}");
				formula = boost::xpressive::regex_replace(formula, re, value);

			} else if ((cit->second).size() > 1) { // Try key[0], key[1] --> you may use formulas with place holders sin({{x[2]}})
				std::size_t cnt = 0;
				typename std::vector<fp_type>::const_iterator v_cit;
				for (v_cit = (cit->second).begin(); v_cit != (cit->second).end(); ++v_cit) {
					value = Gem::Common::to_string(*v_cit);
					re = boost::xpressive::as_xpr("{{" + key + "[" + Gem::Common::to_string(cnt++) + "]" + "}}");
					formula = boost::xpressive::regex_replace(formula, re, value);
				}
			} else { // The vector is empty
				throw gemfony_exception(
					g_error_streamer(DO_LOG, time_and_place)
						<< "In GFormulaParserT::replacePlaceHolders(): Error!" << std::endl
						<< "Vector is empty!" << std::endl
				);
			}
		}

		return formula;
	}

	/***************************************************************************/
	/**
	 * Compiles the AST into byte code
	 */
	void compile(const ast_expression &x) const {
		(*this)(x);
	}

	/***************************************************************************/
	/**
	 * The actual calculations
	 */
	void execute() const {
		// Position pointers for stack and code
		typename std::vector<codeEntry>::const_iterator code_ptr = code_.begin();
		stack_ptr_ = stack_.begin();

		// When requested by the user, print a copy of the code-vector
		if (printCode_) printCode();

		while (code_ptr != code_.end()) {
			// Note: *code_ptr is a boost::variabt, boost::get has nothing to do with a boost::tuple here
			switch (boost::get<byte_code>(*code_ptr++)) { // Read out code_ptr, then switch it to the next position
				case byte_code::op_trap: {
					throw gemfony_exception(
						g_error_streamer(DO_LOG, time_and_place)
							<< "In GFormulaParserT<fp_type>::execute(): Error!" << std::endl
							<< "byte_code::op_trap encountered" << std::endl
					);
				}
					break;

				case byte_code::op_neg:
					stack_ptr_[-1] = -stack_ptr_[-1];
					break;

				case byte_code::op_add:
					--stack_ptr_;
					stack_ptr_[-1] += stack_ptr_[0];
					break;

				case byte_code::op_sub:
					--stack_ptr_;
					stack_ptr_[-1] -= stack_ptr_[0];
					break;

				case byte_code::op_mul:
					--stack_ptr_;
					stack_ptr_[-1] *= stack_ptr_[0];
					break;

				case byte_code::op_div: {
					--stack_ptr_;
					if (0 == stack_ptr_[0]) {
						throw Gem::Common::division_by_0();
					} else {
						stack_ptr_[-1] /= stack_ptr_[0];
					}
				}
					break;

				case byte_code::op_min:
					--stack_ptr_;
					stack_ptr_[-1] = Gem::Common::gmin(stack_ptr_[-1], stack_ptr_[0]);
					break;

				case byte_code::op_max:
					--stack_ptr_;
					stack_ptr_[-1] = Gem::Common::gmax(stack_ptr_[-1], stack_ptr_[0]);
					break;

				case byte_code::op_pow:
					--stack_ptr_;
					stack_ptr_[-1] = std::pow(stack_ptr_[-1], stack_ptr_[0]);
					break;

				case byte_code::op_hypot:
					--stack_ptr_;
					stack_ptr_[-1] = hypot(stack_ptr_[-1], stack_ptr_[0]);
					break;

				case byte_code::op_acos: {
					if (stack_ptr_[-1] < -1. || stack_ptr_[-1] > 1.) {
						throw Gem::Common::acos_invalid_range<fp_type>(stack_ptr_[-1]);
					} else {
						stack_ptr_[-1] = std::acos(stack_ptr_[-1]);
					}
				}
					break;

				case byte_code::op_asin: {
					if (stack_ptr_[-1] < -1. || stack_ptr_[-1] > 1.) {
						throw Gem::Common::asin_invalid_range<fp_type>(stack_ptr_[-1]);
					} else {
						stack_ptr_[-1] = std::asin(stack_ptr_[-1]);
					}
				}
					break;

				case byte_code::op_atan:
					stack_ptr_[-1] = std::atan(stack_ptr_[-1]);
					break;

				case byte_code::op_ceil:
					stack_ptr_[-1] = std::ceil(stack_ptr_[-1]);
					break;

				case byte_code::op_cos:
					stack_ptr_[-1] = std::cos(stack_ptr_[-1]);
					break;

				case byte_code::op_cosh:
					stack_ptr_[-1] = std::cosh(stack_ptr_[-1]);
					break;

				case byte_code::op_exp:
					stack_ptr_[-1] = std::exp(stack_ptr_[-1]);
					break;

				case byte_code::op_fabs:
					stack_ptr_[-1] = std::fabs(stack_ptr_[-1]);
					break;

				case byte_code::op_floor:
					stack_ptr_[-1] = std::floor(stack_ptr_[-1]);
					break;

				case byte_code::op_log: {
					if (stack_ptr_[-1] <= 0.) {
						throw Gem::Common::log_negative_value<fp_type>(stack_ptr_[-1]);
					} else {
						stack_ptr_[-1] = std::log(stack_ptr_[-1]);
					}
				}
					break;

				case byte_code::op_log10: {
					if (stack_ptr_[-1] <= 0.) {
						throw Gem::Common::log10_negative_value<fp_type>(stack_ptr_[-1]);
					} else {
						stack_ptr_[-1] = std::log10(stack_ptr_[-1]);
					}
				}
					break;

				case byte_code::op_sin:
					stack_ptr_[-1] = std::sin(stack_ptr_[-1]);
					break;

				case byte_code::op_sinh:
					stack_ptr_[-1] = std::sinh(stack_ptr_[-1]);
					break;

				case byte_code::op_sqrt: {
					if (stack_ptr_[-1] < 0.) {
						throw Gem::Common::sqrt_negative_value<fp_type>(stack_ptr_[-1]);
					} else {
						stack_ptr_[-1] = std::sqrt(stack_ptr_[-1]);
					}
				}
					break;

				case byte_code::op_tan:
					stack_ptr_[-1] = std::tan(stack_ptr_[-1]);
					break;

				case byte_code::op_tanh:
					stack_ptr_[-1] = std::tanh(stack_ptr_[-1]);
					break;

				case byte_code::op_fp:
					*stack_ptr_++ = boost::get<fp_type>(*code_ptr++);
					break;

				default: {
					throw gemfony_exception(
						g_error_streamer(DO_LOG, time_and_place)
							<< "In GFormulaParserT<fp_type>::execute(): Error!" << std::endl
							<< "Invalid instruction " << static_cast<std::size_t>(boost::get<byte_code>(*code_ptr--)) << std::endl
					);
					// Note that the static cast is required here as strongly-typed enums cannot be
					// cast implicitly to integers types.
				}
					break;
			}
		}
	}

	/***************************************************************************/
	/**
	 * Prints the stack until it encounters a 0 entry or the end of the list
	 */
	void printStack() const {
		if (stack_.empty()) {
			std::cout << "Stack is empty!" << std::endl;
			return;
		}

		typename std::vector<fp_type>::const_iterator it = stack_.begin();
		std::cout << "Stack: ";
		while (it != stack_.end() && *it != 0.) {
			std::cout << *it << " " << std::flush;
			++it;
		}
		std::cout << std::endl;
	}

	/***************************************************************************/
	/**
	 * Prints the code
	 */
	void printCode() const {
		if (code_.empty()) {
			std::cout << "Code is empty!" << std::endl;
			return;
		}

		std::cout << "Code: ";
		for (auto it: code_) {
			std::cout << static_cast<std::size_t>(boost::get<byte_code>(it)) << " " << std::flush;
		}
		std::cout << std::endl;
	}

	/***************************************************************************/
	// Local data and empty functions

	std::string raw_formula_; ///< Holds the formula with place holders

	boost::spirit::qi::rule<std::string::const_iterator, ast_expression(), boost::spirit::ascii::space_type> expression_rule_;
	boost::spirit::qi::rule<std::string::const_iterator, ast_expression(), boost::spirit::ascii::space_type> term_rule_;
	boost::spirit::qi::rule<std::string::const_iterator, unary_function_(), boost::spirit::ascii::space_type> unary_function_rule_;
	boost::spirit::qi::rule<std::string::const_iterator, binary_function_(), boost::spirit::ascii::space_type> binary_function_rule_;
	boost::spirit::qi::rule<std::string::const_iterator, operand(), boost::spirit::ascii::space_type> factor_rule_;

	boost::spirit::qi::real_parser<fp_type, boost::spirit::qi::real_policies<fp_type>> real;

	boost::spirit::qi::symbols<std::iterator_traits<std::string::const_iterator>::value_type, fp_type> constants_; ///< Holds mathematical- and user-defined constants

	mutable std::vector<fp_type> stack_; ///< Holds the data needed as input for each operation
	mutable std::vector<codeEntry> code_; ///< Holds the "compiled" code

	mutable typename std::vector<fp_type>::iterator stack_ptr_;

	bool printCode_; ///< When set, the code will be printed prior to the evaluation
};

/******************************************************************************/

} /* namespace Common */
} /* namespace Gem */

// Needed for rules to work. Follows http://boost.2283326.n4.nabble.com/hold-multi-pass-backtracking-swap-compliant-ast-td4664679.html
namespace boost {
namespace spirit {

G_API_COMMON void swap(Gem::Common::nil &, Gem::Common::nil &);
G_API_COMMON void swap(Gem::Common::signed_ &, Gem::Common::signed_ &);
G_API_COMMON void swap(Gem::Common::operation &, Gem::Common::operation &);
G_API_COMMON void swap(Gem::Common::unary_function_ &, Gem::Common::unary_function_ &);
G_API_COMMON void swap(Gem::Common::binary_function_ &, Gem::Common::binary_function_ &);
G_API_COMMON void swap(Gem::Common::ast_expression &, Gem::Common::ast_expression &);

} /* namespace spirit */
} /* namespace boost */

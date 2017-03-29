/**
 * @file GParserBuilder.hpp
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

#include "common/GParserBuilder.hpp"

namespace Gem {
namespace Common {

/******************************************************************************/
/**
 * Initialization of static data members
 */
std::mutex Gem::Common::GParserBuilder::configFileParser_mutex_;

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * The standard constructor of the comment level
 */
commentLevel::commentLevel(const std::size_t cl)
	: commentLevel_(cl) { /* nothing */ }

/******************************************************************************/
/**
 * Retrieves the current comment level
 */
std::size_t commentLevel::getCommentLevel() const {
	return commentLevel_;
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * A constructor for individual items
 */
GParsableI::GParsableI(
	const std::string &optionNameVar, const std::string &commentVar
)
	: optionName_(GParsableI::makeVector(optionNameVar)), comment_(GParsableI::makeVector(commentVar)),
	  cl_(0) { /* nothing */ }

/******************************************************************************/
/**
 * A constructor for vectors
 */
GParsableI::GParsableI(
	const std::vector<std::string> &optionNameVec, const std::vector<std::string> &commentVec
)
	: optionName_(optionNameVec), comment_(commentVec), cl_(0) { /* nothing */ }

/******************************************************************************/
/**
 * The destructor
 */
GParsableI::~GParsableI() { /* nothing */ }

/******************************************************************************/
/**
 * Retrieves the option name
 */
std::string GParsableI::optionName(std::size_t pos) const {
	if (optionName_.size() <= pos) {
		glogger
		<< "In GParsableI::optionName(std::size_t): Error!" << std::endl
		<< "Tried to access item at position " << pos << std::endl
		<< "where the size of the vector is " << optionName_.size() << std::endl
		<< GEXCEPTION;
	}

	return optionName_.at(pos);
}

/******************************************************************************/
/**
 * Retrieves the comment that was assigned to this variable
 */
std::string GParsableI::comment(std::size_t pos) const {
	if (comment_.size() <= pos) {
		glogger
		<< "In GParsableI::comment_(std::size_t): Error!" << std::endl
		<< "Tried to access item at position " << pos << std::endl
		<< "where the size of the vector is " << comment_.size() << std::endl
		<< GEXCEPTION;
	}

	return comment_.at(pos);
}

/******************************************************************************/
/**
 * Checks whether comments have indeed been registered
 */
bool GParsableI::hasComments() const {
	return !comment_.empty();
}

/******************************************************************************/
/**
 * Retrieves the number of comments available
 */
std::size_t GParsableI::numberOfComments() const {
	return comment_.size();
}

/******************************************************************************/
/**
 * Needed for ostringstream
 */
GParsableI &GParsableI::operator<<(std::ostream &( *val )(std::ostream &)) {
	std::ostringstream oss;
	oss << val;
	comment_.at(cl_) += oss.str();
	return *this;
}

/******************************************************************************/
/**
 * Needed for ostringstream
 */
GParsableI &GParsableI::operator<<(std::ios &( *val )(std::ios &)) {
	std::ostringstream oss;
	oss << val;
	comment_.at(cl_) += oss.str();
	return *this;
}

/******************************************************************************/
/**
 *  Needed for ostringstream
 */
GParsableI &GParsableI::operator<<(std::ios_base &( *val )(std::ios_base &)) {
	std::ostringstream oss;
	oss << val;
	comment_.at(cl_) += oss.str();
	return *this;
}

/******************************************************************************/
/**
 * Allows to indicate the current comment level
 */
GParsableI &GParsableI::operator<<(const commentLevel &cl) {
#ifdef DEBUG
   if(comment_.empty()) {
      glogger
       << "In GParsableI::operator<< (const commentLevel& cl): Error!" << std::endl
       << "No comments in vector" << std::endl
       << GEXCEPTION;
   }

   if(comment_.size() <= cl.getCommentLevel()) {
      glogger
      << "In GParsableI::operator<< (const commentLevel& cl): Error!" << std::endl
      << "Invalid comment level " << cl.getCommentLevel() << " requested, where the maximum is " << comment_.size() - 1 << std::endl
      << GEXCEPTION;
   }
#endif /* DEBUG */

	cl_ = cl.getCommentLevel();
	return *this;
}

/******************************************************************************/
/**
 * Allows to switch to the next comment level
 */
GParsableI &GParsableI::operator<<(const nextComment &nC) {
#ifdef DEBUG
   if(comment_.empty()) {
      glogger
       << "In GParsableI::operator<< (const nextComment& nC): Error!" << std::endl
       << "No comments in vector" << std::endl
       << GEXCEPTION;
   }

   if(comment_.size() <= (cl_+1)) {
      glogger
      << "In GParsableI::operator<< (const nextComment& nC): Error!" << std::endl
      << "Invalid comment level " << cl_+1 << " requested, where the maximum is " << comment_.size() - 1 << std::endl
      << GEXCEPTION;
   }
#endif /* DEBUG */

	// Increment the comment level
	cl_++;
	return *this;
}

/******************************************************************************/
/**
 * Splits a comment into sub-tokens. The comment will be split in case of newlines
 * and semicolons.
 */
std::vector<std::string> GParsableI::splitComment(const std::string &comment) const {
	std::vector<std::string> results;

	// Needed for the separation of comment strings
	using tokenizer = boost::tokenizer<boost::char_separator<char>>;
	boost::char_separator<char> semicolon_sep(";");

	if (comment != "" && comment != "empty") {
		// First split the comment according to newlines
		std::vector<std::string> nlComments;
		std::istringstream buffer(comment);
		std::string line;

		while (std::getline(buffer, line)) {
			nlComments.push_back(line);
		}

		// Break the sub-comments into individual lines after each semicolon
		std::vector<std::string>::iterator it;
		for (it = nlComments.begin(); it != nlComments.end(); ++it) {
			tokenizer commentTokenizer(*it, semicolon_sep);
			for (tokenizer::iterator c = commentTokenizer.begin(); c != commentTokenizer.end(); ++c) {
				results.push_back(*c);
			}
		}
	}

	return results;
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * A constructor for individual items
 */
GFileParsableI::GFileParsableI(
	const std::string &optionNameVar, const std::string &commentVar, const bool &isEssentialVar
)
	: GParsableI(optionNameVar, commentVar), isEssential_(isEssentialVar) { /* nothing */ }

/******************************************************************************/
/**
 * A constructor for vectors
 */
GFileParsableI::GFileParsableI(
	const std::vector<std::string> &optionNameVec, const std::vector<std::string> &commentVec, const bool &isEssentialVar
)
	: GParsableI(optionNameVec, commentVec), isEssential_(isEssentialVar) { /* nothing */ }

/******************************************************************************/
/**
 * The destructor
 */
GFileParsableI::~GFileParsableI() { /* nothing */ }

/******************************************************************************/
/**
 * Checks whether this is an essential variable
 */
bool GFileParsableI::isEssential() const {
	return isEssential_;
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * A constructor for individual items
 */
GCLParsableI::GCLParsableI(
	const std::string &optionNameVar, const std::string &commentVar
)
	: GParsableI(optionNameVar, commentVar) { /* nothing */ }

/******************************************************************************/
/**
 * A constructor for vectors
 */
GCLParsableI::GCLParsableI(
	const std::vector<std::string> &optionNameVec, const std::vector<std::string> &commentVec
)
	: GParsableI(optionNameVec, commentVec) { /* nothing */ }

/******************************************************************************/
/**
 * The destructor
 */
GCLParsableI::~GCLParsableI() { /* nothing */ }

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * The default constructor
 *
 * @param configurationFile The name of the configuration file
 */
GParserBuilder::GParserBuilder()
	: configFileBaseName_("empty") {
#if defined(_MSC_VER)  && (_MSC_VER >= 1020)
   char* jsonBaseName_ch = 0;
   size_t sz = 0;
   if (0 == _dupenv_s(&jsonBaseName_ch, &sz, "GENEVA_CONFIG_BASENAME") && nullptr != jsonBaseName_ch) {
      // Only convert to a string if the environment variable exists
      configFileBaseName_ = std::string(jsonBaseName_ch);
      // Convert to a std::string and remove any white space characters
      boost::trim(configFileBaseName_);
      // Clean up the environment
      free(jsonBaseName_ch);
   }
#else /* _MSC_VER */
	const char *jsonBaseName_ch = std::getenv("GENEVA_CONFIG_BASENAME");

	if (jsonBaseName_ch) {
		// Only convert to a string if the environment variable exists
		configFileBaseName_ = std::string(jsonBaseName_ch);

		// Convert to a std::string and remove any white space characters
		boost::trim(configFileBaseName_);
	}
#endif /* _MSC_VER */
}

/******************************************************************************/
/**
 * The destructor
 */
GParserBuilder::~GParserBuilder() { /* nothing */ }

/******************************************************************************/
/**
 * Tries to parse a given configuration file for a set of options. Note that parsing
 * is a one-time effort.
 *
 * @param configFile The name of the configuration file to be parsed
 * @return A boolean indicating whether parsing was successful
 */
bool GParserBuilder::parseConfigFile(const std::string &configFile) {
	// Make sure only one entity is parsed at once. This allows us to
	// concurrently create e.g. optimization algorithms, letting them
	// parse the same config file.
	std::unique_lock<std::mutex> lk(GParserBuilder::configFileParser_mutex_);

	namespace pt = boost::property_tree;
	namespace bf = boost::filesystem;

	bool result = false;
	pt::ptree ptr; // A property tree object;

	// Add a base name, if possible and required
	std::string configFile_withBase = configFile;
	boost::trim(configFile_withBase);

	// Check that configFile_withBase doesn't start with a / (--> absolute path)
	if (!configFileBaseName_.empty() && "empty" != configFileBaseName_ && configFile.at(0) != '/') {
		configFile_withBase = configFileBaseName_ + configFile_withBase;
	}

	try {
		// Do some error checking. Also check that the configuration file exists.
		// If not, create a default version
		if (!bf::exists(configFile_withBase)) {
			glogger
			<< "Note: In GParserBuilder::parseConfigFile():" << std::endl
			<< "Configuration file " << configFile_withBase << " does not exist." << std::endl
			<< "We will try to create a file with default values for you." << std::endl
			<< GLOGGING;

			std::string header = "This configuration file was automatically created by GParserBuilder;";
			this->writeConfigFile(
				configFile, header, true // writeAll == true
			);
		} else { // configFile exists
			// Is it a regular file ?
			if (!bf::is_regular_file(configFile_withBase)) {
				glogger
				<< "In GParserBuilder::parseConfigFile(): Error!" << std::endl
				<< configFile_withBase << " exists but is no regular file." << std::endl
				<< GEXCEPTION;
			}

			// We require the file to have the json extension
			if (!bf::path(configFile_withBase).has_extension() || bf::path(configFile_withBase).extension() != ".json") {
				glogger
				<< "In GParserBuilder::parseConfigFile(): Error!" << std::endl
				<< configFile_withBase << " does not have the required extension \".json\"" << std::endl
				<< GEXCEPTION;
			}
		}

		// Do the actual parsing
		pt::read_json(configFile_withBase, ptr);

		// Load the data into our objects and execute the relevant call-back functions
		std::vector<std::shared_ptr < GFileParsableI>> ::iterator
		it;
		for (it = file_parameter_proxies_.begin(); it != file_parameter_proxies_.end(); ++it) {
			(*it)->load(ptr);
			(*it)->executeCallBackFunction();
		}

		result = true;
	} catch (const gemfony_error_condition &e) {
		glogger
		<< "Caught gemfony_error_condition when parsing configuration file " << configFile_withBase << ":" << std::endl
		<< e.what() << std::endl
		<< GLOGGING;
		result = false;
	} catch (const std::exception &e) {
		glogger
		<< "Caught std::exception when parsing configuration file " << configFile_withBase << ":" << std::endl
		<< e.what() << std::endl
		<< GLOGGING;
		result = false;
	} catch (...) {
		glogger
		<< "Unknown error while parsing the configuration file " << configFile_withBase << std::endl
		<< GLOGGING;
		result = false;
	}

	return result;
}

/******************************************************************************/
/**
 * Writes out a configuration file. If no argument is given, the configuration file
 * will be written to the stored file name.
 *
 * @param fileName The name of the configuration file to be written
 * @param header A descriptive comment to be output at the top of the configuration file
 * @param writeAll A boolean parameter that indicates whether all or only essential parameters should be written
 */
void GParserBuilder::writeConfigFile(
	const std::string &configFile, const std::string &header, bool writeAll
) const {
	using namespace boost::filesystem;

	// Add a base name, if possible and required
	std::string configFile_withBase = configFile;
	boost::trim(configFile_withBase);

	// Check that configFile_withBase doesn't start with a / (--> absolute path)
	if (!configFileBaseName_.empty() && "empty" != configFileBaseName_ && configFile.at(0) != '/') {
		configFile_withBase = configFileBaseName_ + configFile_withBase;
	}

	// Needed for the separation of comment strings
	using tokenizer = boost::tokenizer<boost::char_separator<char>>;
	boost::char_separator<char> semicolon_sep(";");

	// Do some error checking
	{
		// Is configFile a directory ?
		if (is_directory(configFile_withBase)) {
			glogger
			<< "In GParserBuilder::writeConfigFile(): Error!" << std::endl
			<< configFile_withBase << " is a directory." << std::endl
			<< GEXCEPTION;
		}

		// We do not allow to overwrite existing files
		if (exists(configFile_withBase) && is_regular_file(configFile_withBase)) {
			glogger
			<< "In GParserBuilder::writeConfigFile(): Error!" << std::endl
			<< "You have specified an existing file (" << configFile_withBase << ")." << std::endl
			<< GEXCEPTION;
		}

		// Check that the target path exists and is a directory
		if (!exists(path(configFile_withBase).remove_filename()) ||
			 !is_directory(path(configFile_withBase).remove_filename())) {
			glogger
			<< "In GParserBuilder::writeConfigFile(): Error!" << std::endl
			<< "The target path " << path(configFile_withBase).remove_filename() <<
			" does not exist or is no directory." << std::endl
			<< GEXCEPTION;
		}

		// Check that the configuration file has the required extension
		if (!path(configFile_withBase).has_extension() || path(configFile_withBase).extension() != ".json") {
			glogger
			<< "In GParserBuilder::writeConfigFile(): Error!" << std::endl
			<< configFile_withBase << " does not have the required extension \".json\"" << std::endl
			<< GEXCEPTION;
		}
	}

	// Open the required configuration file
	boost::filesystem::ofstream ofs(configFile_withBase);
	if (!ofs) {
		glogger
		<< "In GParserBuilder::writeConfigFile(): Error writing the configuration file " << configFile_withBase <<
		std::endl
		<< GEXCEPTION;
	}

	// Do some error checking
	if (file_parameter_proxies_.size() == 0) {
		glogger
		<< "In GParserBuilder::writeConfigFile(): No variables found!" << std::endl
		<< GEXCEPTION;
	}

	// Create a property tree object;
	boost::property_tree::ptree ptr;

	// Output a header
	if (header != "") {
		// Break the header into individual tokens
		tokenizer headerTokenizer(header, semicolon_sep);
		for (tokenizer::iterator h = headerTokenizer.begin(); h != headerTokenizer.end(); ++h) {
			ptr.add("header.comment", std::string(*h).c_str());
		}
	}
	ptr.add("header.comment", Gem::Common::currentTimeAsString());

	// Output variables and values
	std::vector<std::shared_ptr <GFileParsableI>> ::const_iterator cit;
	for (cit = file_parameter_proxies_.begin(); cit != file_parameter_proxies_.end(); ++cit) {
		// Only write out the parameter(s) if they are either essential or it
		// has been requested to write out all parameters regardless
		if (!writeAll && !(*cit)->isEssential()) continue;

		// Output the actual data of this parameter object to the property tree
		(*cit)->save(ptr);
	}

	// Write the configuration data to disk
	boost::property_tree::write_json(ofs, ptr);

	// Close the file handle
	ofs.close();
}

/******************************************************************************/
/**
 * Provides information on the number of configuration options stored in this class
 *
 * @return The number of configuration options stored in this class
 */
std::size_t GParserBuilder::numberOfFileOptions() const {
	return file_parameter_proxies_.size();
}

/******************************************************************************/
/**
 * Parses the command line for options
 *
 * @param argc The argument count
 * @param argv The argument vector
 * @param verbose If set to true, the function will emit information about the parsed parameters
 * @return A boolean indicating whether help was requested (true) or not (false)
 */
bool GParserBuilder::parseCommandLine(int argc, char **argv, const bool &verbose) {
	namespace po = boost::program_options;

	bool result = GCL_NO_HELP_REQUESTED;

	try {
		std::string usageString = std::string("Usage: ") + argv[0] + " [options]";
		po::options_description desc(usageString.c_str());

		// We always want --help and -h to be available
		desc.add_options()("help,h", "Emit help message");

		// Add further options from the parameter objects
		std::vector<std::shared_ptr < GCLParsableI>> ::iterator
		it;
		for (it = cl_parameter_proxies_.begin(); it != cl_parameter_proxies_.end(); ++it) {
			(*it)->save(desc);
		}

		// Do the actual parsing
		po::variables_map vm;
		po::store(po::parse_command_line(argc, (const char *const *) arg, desc), vm);
		po::notify(vm);

		// Emit a help message, if necessary and let the caller of this function know
		if (vm.count("help")) {
			std::cout << desc << std::endl;
			result = GCL_HELP_REQUESTED;
		} else {
			if (verbose) {
				std::cout
				<< "GParserBuilder::parseCommandLine():" << std::endl
				<< "Working with the following options:" << std::endl;
				for (it = cl_parameter_proxies_.begin(); it != cl_parameter_proxies_.end(); ++it) {
					std::cout << (*it)->content() << std::endl;
				}
				std::cout << std::endl;
			}
		}
	} catch (const po::error &e) {
		glogger
		<< "In GParserBuilder::parseCommandLine(int argc, char **argv):" << std::endl
		<< "Error parsing the command line:" << std::endl
		<< e.what() << std::endl
		<< GTERMINATION;
	} catch (...) {
		glogger
		<< "In GParserBuilder::parseCommandLine(int argc, char **argv):" << std::endl
		<< "Unknown error while parsing the command line" << std::endl
		<< GTERMINATION;
	}

	return result;
}

/******************************************************************************/
/**
 * Provides information on the number of command line configuration options
 * stored in this class
 */
std::size_t GParserBuilder::numberOfCLOptions() const {
	return cl_parameter_proxies_.size();
}

/******************************************************************************/

} /* namespace Common */
} /* namespace Gem */

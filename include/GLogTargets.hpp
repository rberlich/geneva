/**
 * \file
 */

/**
 * Copyright (C) 2004-2008 Dr. Ruediger Berlich
 * Copyright (C) 2007-2008 Forschungszentrum Karlsruhe GmbH
 * 
 * This file is part of Geneva, Gemfony scientific's optimization library.
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

#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include <boost/version.hpp>

#if BOOST_VERSION < 103600
#error "Error: Boost should at least have version 1.36 !"
#endif /* BOOST_VERSION */

#include <boost/cstdint.hpp>

#ifndef GLOGTARGETS_H_
#define GLOGTARGETS_H_

using namespace std;

namespace GenEvA
{

  /***********************************************************************************/
  /**
   * This class defines the interface of log targets, i.e. targets for the logging of
   * messages through the GLogger and GLogStreamer classes. Essentially all that is 
   * needed is the log function. Pointers to this class are stored in the GLogger, they
   * point to objects of the GConsoleLogger or GDiskLogger classes, or other log targets
   * defined by the user.
   */
  class GBaseLogTarget{
  public:
    /** \brief The default constructor */
    GBaseLogTarget(void);
    /** \brief The standard destructor */
    virtual ~GBaseLogTarget();
    /** \brief The logging interface */
    virtual void log(const string& msg) = 0;
  };

  /***********************************************************************************/
  /**
   * The console logger writes log messages to the console.
   */
  class GConsoleLogger
    :public GBaseLogTarget
  {
  public:
    /** \brief A standard constructor */
    GConsoleLogger(void);
    /** \brief The standard destructor */
    virtual ~GConsoleLogger();
    /** \brief Implements the logging to the console */
    virtual void log(const string& msg);
  };

  /***********************************************************************************/
  /**
   * The disk logger writes log messages to a file.
   */
  class GDiskLogger
    :public GBaseLogTarget
  {
  public:
    /** \brief A standard constructor */
    GDiskLogger(void);
    /** \brief This constructor accepts the name of the log file as argument */
    explicit GDiskLogger(string fn);
    /** \brief The standard destructor */
    virtual ~GDiskLogger();
    /** \brief Implements logging to a file on disk */
    virtual void log(const string& msg);

  private:
    string fname; ///< The name of the log file
  };

  /***********************************************************************************/

} /* namespace GenEvA */

#endif /*GLOGTARGETS_H_*/

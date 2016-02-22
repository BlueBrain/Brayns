/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef ABSTRACTPARAMETERS_H
#define ABSTRACTPARAMETERS_H

#include <brayns/common/log.h>
#include <brayns/common/types.h>

#include <boost/program_options.hpp>

namespace brayns
{

/**
   Base class defining command line parameters
 */
class AbstractParameters
{
public:
    /**
       Constructor
       @param name Display name for the set of parameters
     */
    AbstractParameters( const std::string& name ) : _name(name) {};

    virtual ~AbstractParameters( ) {}

    /**
       Parses parameters managed by the class
       @param argc number of command line parameters
       @param argv actual command line parameters
       @return false if the parsing requires application termination. This can
               help stopping the application if a mandatory parameter is missing
     */
    virtual bool parse( int argc, const char **argv );

    /**
       Displays the usage of registered parameters
     */
    void usage( );

    /**
       Displays values of registered parameters
     */
    virtual void print( );

    const strings& arguments() const;

protected:
    std::string _name;
    boost::program_options::options_description _parameters;
    boost::program_options::variables_map _vm;
    strings _arguments;
};

}
#endif // ABSTRACTPARAMETERS_H

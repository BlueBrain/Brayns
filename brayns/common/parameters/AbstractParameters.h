/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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

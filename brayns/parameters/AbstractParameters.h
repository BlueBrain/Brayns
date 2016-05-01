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

namespace po = boost::program_options;

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
       @return a map of the variables identified by the parsing
     */
    bool parse( int argc, const char **argv );

    /**
       Displays the usage of registered parameters
     */
    void usage( );

    /**
       Displays values of registered parameters
     */
    virtual void print( );

    /**
       Sets a parameter. If the parameter is not registered, Action is ignored
       and a warning message is traced
       @param key Name of the parameter
       @param value Value of the parameter
     */
    void set( const std::string& key, const std::string& value );

    const strings& arguments() const;

protected:

    virtual bool _parse( const po::variables_map& ) = 0;

    std::string _name;
    po::options_description _parameters;
    strings _arguments;
};

}
#endif // ABSTRACTPARAMETERS_H

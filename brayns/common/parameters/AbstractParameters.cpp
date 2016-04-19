/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#include "AbstractParameters.h"
#include <brayns/common/types.h>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <ostream>

namespace brayns
{

po::variables_map AbstractParameters::parse( int argc, const char **argv )
{
    for( int i = 1; i < argc; ++i )
        _arguments.push_back(argv[i]);

    boost::program_options::variables_map vm;
    po::parsed_options parsedOptions =
        po::command_line_parser( argc, argv ).options( _parameters ).
        allow_unregistered( ).run( );
    po::store( parsedOptions, vm );
    po::notify(vm);
    return vm;
}

void AbstractParameters::usage( )
{
    BRAYNS_INFO << _name << " parameters:" <<
        std::endl << _parameters << std::endl;
}

void AbstractParameters::print( )
{
    BRAYNS_INFO << "-= " << _name << " parameters =-" << std::endl;
}

const strings& AbstractParameters::arguments() const
{
    return _arguments;
}

void AbstractParameters::set( const std::string& key, const std::string& value )
{
    const std::string p = "--" + key;
    std::vector< std::string > strs;
    boost::split(strs, value, boost::is_any_of(" "));

    const size_t argc = 2 + strs.size();
    const char** argv = new const char*[argc];
    argv[0] = "";
    argv[1] = p.c_str();
    for( size_t i = 0; i < strs.size(); ++i )
        argv[2+i] = strs[i].c_str();

    boost::program_options::variables_map vm;
    po::parsed_options parsedOptions =
        po::command_line_parser( argc, argv ).options( _parameters ).
        allow_unregistered( ).run( );
    po::store( parsedOptions, vm );
    po::notify(vm);
    delete [] argv;

    _parse( vm );
}

}

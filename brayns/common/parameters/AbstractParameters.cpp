/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#include "AbstractParameters.h"
#include <brayns/common/types.h>
#include <boost/program_options.hpp>
#include <ostream>

namespace brayns
{

namespace po = boost::program_options;

bool AbstractParameters::parse( int argc, const char **argv )
{
    for( int i = 1; i < argc; ++i )
        _arguments.push_back(argv[i]);

    po::parsed_options parsedOptions =
        po::command_line_parser( argc, argv ).options( _parameters ).
        allow_unregistered( ).run( );
    po::store( parsedOptions, _vm );
    po::notify(_vm);
    return true;
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

}


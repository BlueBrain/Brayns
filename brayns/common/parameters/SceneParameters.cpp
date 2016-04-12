/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#include "SceneParameters.h"


namespace
{
const std::string PARAM_TIMESTAMP = "timestamp";
}

namespace brayns
{

namespace po = boost::program_options;

SceneParameters::SceneParameters()
    : AbstractParameters( "Scene" )
    , _timestamp( std::numeric_limits<float>::max( ))
{
    _parameters.add_options()
        (PARAM_TIMESTAMP.c_str(), po::value< float >( ), "Timestamp");
}

po::variables_map SceneParameters::parse( int argc, const char **argv )
{
    po::variables_map vm = AbstractParameters::parse( argc, argv );
    _parse( vm );
    return vm;
}

bool SceneParameters::_parse( const boost::program_options::variables_map& vm )
{
    if( vm.count( PARAM_TIMESTAMP ))
        _timestamp = vm[PARAM_TIMESTAMP].as< float >( );

    return true;
}

void SceneParameters::print( )
{
    AbstractParameters::print( );
    BRAYNS_INFO << "Timestamp :" <<
        _timestamp << std::endl;
}

}

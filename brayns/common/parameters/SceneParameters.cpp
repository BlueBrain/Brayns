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

bool SceneParameters::parse( int argc, const char **argv )
{
    AbstractParameters::parse( argc, argv );

    if( _vm.count( PARAM_TIMESTAMP ))
        _timestamp = _vm[PARAM_TIMESTAMP].as< float >( );

    return true;
}

void SceneParameters::print( )
{
    AbstractParameters::print( );
    BRAYNS_INFO << "Timestamp :" <<
        _timestamp << std::endl;
}

}

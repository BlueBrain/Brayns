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

#include "ApplicationParameters.h"
#include <brayns/common/log.h>
#include <brayns/common/parameters/ParametersManager.h>

#include <boost/lexical_cast.hpp>

namespace
{

const std::string PARAM_HELP = "help";
const std::string PARAM_WINDOW_SIZE = "window-size";
const std::string PARAM_CAMERA = "camera";
const std::string PARAM_BENCHMARKING = "enable-benchmark";
const std::string PARAM_DEFLECT_HOST_NAME = "deflect-hostname";
const std::string PARAM_DEFLECT_STREAM_NAME = "deflect-streamname";

const size_t DEFAULT_WINDOW_WIDTH = 800;
const size_t DEFAULT_WINDOW_HEIGHT = 600;
const std::string DEFAULT_DEFLECT_STREAM_NAME = "brayns";
const std::string DEFAULT_CAMERA = "perspective";

}

namespace brayns
{

namespace po = boost::program_options;

ApplicationParameters::ApplicationParameters( )
    : AbstractParameters( "Application" )
    , _camera( DEFAULT_CAMERA )
    , _windowSize( DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT )
    , _deflectStreamname( DEFAULT_DEFLECT_STREAM_NAME )
    , _benchmarking( false )
{
    _parameters.add_options( )
        ( PARAM_HELP.c_str( ), "Help screen" )
        ( PARAM_WINDOW_SIZE.c_str( ), po::value< uints >( )->multitoken( ),
            "Window size" )
        ( PARAM_CAMERA.c_str( ), po::value< std::string >( ),
            "Camera name" )
        ( PARAM_DEFLECT_HOST_NAME.c_str( ), po::value< std::string >( ),
            "Name of host running DisplayCluster" )
        ( PARAM_DEFLECT_STREAM_NAME.c_str( ), po::value< std::string >( ),
            "Name of DisplayCluster stream" )
        ( PARAM_BENCHMARKING.c_str( ), po::value< std::string >( ),
            "Activates application benchmarking" );
}

bool ApplicationParameters::parse( int argc, const char **argv )
{
    AbstractParameters::parse( argc, argv );

    if( _vm.count( PARAM_HELP ))
        return false;
    if( _vm.count( PARAM_WINDOW_SIZE ))
    {
        uints values = _vm[PARAM_WINDOW_SIZE].as< uints >( );
        if( values.size() == 2 )
        {
            _windowSize.x( ) = values[0];
            _windowSize.y( ) = values[1];
        }
    }
    if( _vm.count( PARAM_CAMERA ))
        _camera = _vm[PARAM_CAMERA].as< std::string >( );
    if( _vm.count( PARAM_DEFLECT_HOST_NAME ))
        _deflectHostname = _vm[PARAM_DEFLECT_HOST_NAME].as< std::string >( );
    if( _vm.count( PARAM_DEFLECT_STREAM_NAME ))
        _deflectStreamname = _vm[PARAM_DEFLECT_STREAM_NAME].as< std::string >( );
    if( _vm.count( PARAM_BENCHMARKING ))
        _benchmarking = _vm[PARAM_BENCHMARKING].as< bool >( );

    return true;
}

void ApplicationParameters::print( )
{
    AbstractParameters::print( );
    BRAYNS_INFO << "Window size             : " << _windowSize << std::endl;
    BRAYNS_INFO << "Camera                  : " << _camera << std::endl;
    BRAYNS_INFO << "Deflect host name       : " <<
        _deflectHostname << std::endl;
    BRAYNS_INFO << "Deflect stream name     : " <<
        _deflectStreamname << std::endl;
    BRAYNS_INFO << "Benchmarking            : " <<
        ( _benchmarking ? "on" : "off" ) << std::endl;
}

}

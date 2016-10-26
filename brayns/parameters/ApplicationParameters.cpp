/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
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
#include <brayns/parameters/ParametersManager.h>

#include <boost/lexical_cast.hpp>

namespace
{

const std::string PARAM_HELP = "help";
const std::string PARAM_WINDOW_SIZE = "window-size";
const std::string PARAM_CAMERA = "camera";
const std::string PARAM_BENCHMARKING = "enable-benchmark";
const std::string PARAM_JPEG_COMPRESSION = "jpeg-compression";
const std::string PARAM_JPEG_SIZE = "jpeg-size";
const std::string PARAM_FILTERS = "filters";

const size_t DEFAULT_WINDOW_WIDTH = 800;
const size_t DEFAULT_WINDOW_HEIGHT = 600;
const size_t DEFAULT_JPEG_WIDTH = DEFAULT_WINDOW_WIDTH;
const size_t DEFAULT_JPEG_HEIGHT = DEFAULT_WINDOW_HEIGHT;
const size_t DEFAULT_JPEG_COMPRESSION = 100;
const std::string DEFAULT_CAMERA = "perspective";

}

namespace brayns
{

ApplicationParameters::ApplicationParameters( )
    : AbstractParameters( "Application" )
    , _camera( DEFAULT_CAMERA )
    , _windowSize( DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT )
    , _benchmarking( false )
    , _jpegCompression( DEFAULT_JPEG_COMPRESSION )
    , _jpegSize( DEFAULT_JPEG_WIDTH, DEFAULT_JPEG_HEIGHT )
{
    _parameters.add_options( )
        ( PARAM_HELP.c_str( ), "Help screen" )
        ( PARAM_WINDOW_SIZE.c_str( ), po::value< uints >( )->multitoken( ),
            "Window size" )
        ( PARAM_CAMERA.c_str( ), po::value< std::string >( ),
            "Camera name" )
        ( PARAM_BENCHMARKING.c_str( ), po::value< bool >( ),
            "Activates application benchmarking" )
        ( PARAM_JPEG_COMPRESSION.c_str( ), po::value< size_t >( ),
            "JPEG compression rate (100 = full quality)" )
        ( PARAM_JPEG_SIZE.c_str( ), po::value< uints >( )->multitoken( ),
            "JPEG size" )
        ( PARAM_FILTERS.c_str( ), po::value< strings >( )->multitoken( ),
            "Screen space filters" );
}

bool ApplicationParameters::_parse( const po::variables_map& vm )
{
    if( vm.count( PARAM_HELP ))
        return false;
    if( vm.count( PARAM_WINDOW_SIZE ))
    {
        uints values = vm[PARAM_WINDOW_SIZE].as< uints >( );
        if( values.size() == 2 )
        {
            _windowSize.x( ) = values[0];
            _windowSize.y( ) = values[1];
        }
    }
    if( vm.count( PARAM_CAMERA ))
        _camera = vm[PARAM_CAMERA].as< std::string >( );
    if( vm.count( PARAM_BENCHMARKING ))
        _benchmarking = vm[PARAM_BENCHMARKING].as< bool >( );
    if( vm.count( PARAM_JPEG_COMPRESSION ))
        _jpegCompression = vm[PARAM_JPEG_COMPRESSION].as< size_t >( );
    if( vm.count( PARAM_JPEG_SIZE ))
    {
        uints values = vm[PARAM_JPEG_SIZE].as< uints >( );
        if( values.size() == 2 )
        {
            _jpegSize.x( ) = values[0];
            _jpegSize.y( ) = values[1];
        }
    }
    if( vm.count( PARAM_FILTERS ))
    {
        _filters = vm[PARAM_FILTERS].as< strings >( );
    }

    return true;
}

void ApplicationParameters::print( )
{
    AbstractParameters::print( );
    BRAYNS_INFO << "Window size             : " << _windowSize << std::endl;
    BRAYNS_INFO << "Camera                  : " << _camera << std::endl;
    BRAYNS_INFO << "JPEG size               : " << _jpegSize << std::endl;
}

}

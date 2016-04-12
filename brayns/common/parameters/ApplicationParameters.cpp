/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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
const std::string PARAM_JPEG_COMPRESSION = "jpeg-compression";

const size_t DEFAULT_WINDOW_WIDTH = 800;
const size_t DEFAULT_WINDOW_HEIGHT = 600;
const std::string DEFAULT_DEFLECT_STREAM_NAME = "brayns";
const size_t DEFAULT_JPEG_COMPRESSION = 100;
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
    , _jpegCompression( DEFAULT_JPEG_COMPRESSION )
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
            "Activates application benchmarking" )
        ( PARAM_JPEG_COMPRESSION.c_str( ), po::value< size_t >( ),
            "JPeg compression rate (100 = full quality)" );
}

po::variables_map ApplicationParameters::parse( int argc, const char **argv )
{
    po::variables_map vm = AbstractParameters::parse( argc, argv );
    _parse( vm );
    return vm;
}

bool ApplicationParameters::_parse( const boost::program_options::variables_map& vm )
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
    if( vm.count( PARAM_DEFLECT_HOST_NAME ))
        _deflectHostname = vm[PARAM_DEFLECT_HOST_NAME].as< std::string >( );
    if( vm.count( PARAM_DEFLECT_STREAM_NAME ))
        _deflectStreamname = vm[PARAM_DEFLECT_STREAM_NAME].as< std::string >( );
    if( vm.count( PARAM_BENCHMARKING ))
        _benchmarking = vm[PARAM_BENCHMARKING].as< bool >( );
    if( vm.count( PARAM_JPEG_COMPRESSION ))
        _jpegCompression = vm[PARAM_JPEG_COMPRESSION].as< size_t >( );

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
    BRAYNS_INFO << "JPEG Compression        : " <<
        _jpegCompression << std::endl;
}

}

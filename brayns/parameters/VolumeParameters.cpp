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

#include "VolumeParameters.h"

namespace
{
const std::string PARAM_VOLUME_FOLDER = "volume-folder";
const std::string PARAM_VOLUME_FILENAME = "volume-file";
const std::string PARAM_VOLUME_DIMENSIONS = "volume-dimensions";
const std::string PARAM_VOLUME_ELEMENT_SPACING = "volume-element-spacing";
const std::string PARAM_VOLUME_OFFSET = "volume-offset";
const std::string PARAM_VOLUME_SPR = "volume-samples-per-ray";
const size_t DEFAULT_SAMPLES_PER_RAY = 128;
}

namespace brayns
{

VolumeParameters::VolumeParameters()
    : AbstractParameters( "Volume" )
    , _dimensions( 0, 0, 0 )
    , _elementSpacing( 1.f, 1.f, 1.f )
    , _offset( 0.f, 0.f, 0.f )
    , _spr( DEFAULT_SAMPLES_PER_RAY )
{
    _parameters.add_options()
        ( PARAM_VOLUME_FOLDER.c_str(), po::value< std::string >(),
            "Folder containing volume files" )
        ( PARAM_VOLUME_FILENAME.c_str(), po::value< std::string >(),
            "Cache file volume data" )
        ( PARAM_VOLUME_DIMENSIONS.c_str(), po::value< size_ts >()->multitoken(),
            "Volume dimensions" )
        ( PARAM_VOLUME_ELEMENT_SPACING.c_str(), po::value< floats >()->multitoken(),
            "Element spacing in the volume" )
        ( PARAM_VOLUME_OFFSET.c_str(), po::value< floats >()->multitoken(),
            "Volume offset" )
        ( PARAM_VOLUME_SPR.c_str(), po::value< size_t >(),
            "Volume samples per ray" );
}

bool VolumeParameters::_parse( const po::variables_map& vm )
{
    if( vm.count( PARAM_VOLUME_FOLDER ))
        _folder = vm[PARAM_VOLUME_FOLDER].as< std::string >( );

    if( vm.count( PARAM_VOLUME_FILENAME ))
        _filename = vm[PARAM_VOLUME_FILENAME].as< std::string >( );

    if( vm.count( PARAM_VOLUME_DIMENSIONS ))
    {
        size_ts values = vm[PARAM_VOLUME_DIMENSIONS].as< size_ts >( );
        if( values.size( ) == 3 )
            _dimensions = Vector3ui( values[0], values[1], values[2] );
    }
    if( vm.count( PARAM_VOLUME_ELEMENT_SPACING ))
    {
        floats values = vm[PARAM_VOLUME_ELEMENT_SPACING].as< floats >( );
        if( values.size( ) == 3 )
            _elementSpacing = Vector3f( values[0], values[1], values[2] );
    }
    if( vm.count( PARAM_VOLUME_OFFSET ))
    {
        floats values = vm[PARAM_VOLUME_OFFSET].as< floats >( );
        if( values.size() == 3 )
            _offset = Vector3f( values[0], values[1], values[2] );
    }
    if( vm.count( PARAM_VOLUME_SPR ))
        _spr = vm[PARAM_VOLUME_SPR].as< size_t >();
    return true;
}

void VolumeParameters::print( )
{
    AbstractParameters::print( );
    BRAYNS_INFO << "Folder          : " << _folder << std::endl;
    BRAYNS_INFO << "Filename        : " << _filename << std::endl;
    BRAYNS_INFO << "Dimensions      : " << _dimensions << std::endl;
    BRAYNS_INFO << "Element spacing : " << _elementSpacing << std::endl;
    BRAYNS_INFO << "Offset          : " << _offset << std::endl;
    BRAYNS_INFO << "Samples per ray : " << _spr << std::endl;
}

}

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

#include "VolumeHandler.h"

#include <brayns/common/log.h>
#include <brayns/parameters/VolumeParameters.h>

#include <fstream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace
{
const int NO_DESCRIPTOR = -1;
}

namespace brayns
{

VolumeHandler::VolumeHandler(
    VolumeParameters& volumeParameters,
    const TimestampMode timestampMode )
    : _volumeParameters( &volumeParameters )
    , _currentTimestamp( std::numeric_limits<float>::max( ))
    , _timestampRange( std::numeric_limits<float>::max(), std::numeric_limits<float>::min( ))
    , _timestampMode( timestampMode )
{
}

VolumeHandler::~VolumeHandler()
{
    _volumeDescriptors.clear();
}

void VolumeHandler::attachVolumeToFile( const float timestamp, const std::string& volumeFile )
{
    _volumeDescriptors[ timestamp ].reset( new VolumeDescriptor(
        volumeFile,
        _volumeParameters->getDimensions(),
        _volumeParameters->getElementSpacing(),
        _volumeParameters->getOffset()));
    for( const auto& volumeDescriptor: _volumeDescriptors )
    {
        _timestampRange.x() = std::min( _timestampRange.x(), volumeDescriptor.first );
        _timestampRange.y() = std::max( _timestampRange.y(), volumeDescriptor.first );
    }
    BRAYNS_INFO << "Attached " << volumeFile << " to timestamp "
                << timestamp << " " << _timestampRange << std::endl;
}

void* VolumeHandler::getData( const float timestamp )
{
    const float ts = _getBoundedTimestamp( timestamp );
    if( ts != _currentTimestamp &&
        _volumeDescriptors.find( ts ) != _volumeDescriptors.end( ))
    {
        if( _volumeDescriptors.find( _currentTimestamp ) != _volumeDescriptors.end( ))
            _volumeDescriptors[ _currentTimestamp ]->unmap();
        _currentTimestamp = ts;
        _volumeDescriptors[ _currentTimestamp ]->map();
        return _volumeDescriptors[ _currentTimestamp ]->getMemoryMapPtr();
    }
    return 0;
}

float VolumeHandler::getEpsilon(
    const float timestamp ,
    const Vector3f& elementSpacing,
    const uint16_t samplesPerRay )
{
    const float ts = _getBoundedTimestamp( timestamp );
    if( _volumeDescriptors.find( ts ) != _volumeDescriptors.end( ))
    {
        const Vector3f diag =
            Vector3f( _volumeDescriptors[ ts ]->getDimensions( )) * elementSpacing;
        return diag.find_max() / float( samplesPerRay );
    }

    BRAYNS_ERROR << "No volume is attached to the specified timestamp: "
                 << timestamp << std::endl;
    return 0;
}

const Vector3ui VolumeHandler::getDimensions( const float timestamp )
{
    float ts = _getBoundedTimestamp( timestamp );
    if( _volumeDescriptors.find( ts ) != _volumeDescriptors.end( ))
        return _volumeDescriptors[ ts ]->getDimensions();

    BRAYNS_ERROR << "No volume is attached to the specified timestamp: "
                 << timestamp << std::endl;
    return Vector3ui();
}

const Vector3f VolumeHandler::getElementSpacing( const float timestamp )
{
    float ts = _getBoundedTimestamp( timestamp );
    if( _volumeDescriptors.find( ts ) != _volumeDescriptors.end( ))
        return _volumeDescriptors[ ts ]->getElementSpacing();

    BRAYNS_ERROR << "No volume is attached to the specified timestamp: "
                 << timestamp << std::endl;
    return Vector3f();
}

const Vector3f VolumeHandler::getOffset( const float timestamp )
{
    float ts = _getBoundedTimestamp( timestamp );
    if( _volumeDescriptors.find( ts ) != _volumeDescriptors.end( ))
        return _volumeDescriptors[ ts ]->getOffset();

    BRAYNS_ERROR << "No volume is attached to the specified timestamp: "
                 << timestamp << std::endl;
    return Vector3f();
}

uint64_t VolumeHandler::getSize( const float timestamp )
{
    const float ts = _getBoundedTimestamp( timestamp );
    if( _volumeDescriptors.find( ts ) != _volumeDescriptors.end( ))
        return _volumeDescriptors[ ts ]->getSize();

    BRAYNS_ERROR << "No volume is attached to the specified timestamp: "
                 << timestamp << std::endl;
    return 0;
}

float VolumeHandler::_getBoundedTimestamp( const float timestamp ) const
{
    float result;
    switch( _timestampMode )
    {
    case TM_MODULO:
        result = size_t( timestamp + _timestampRange.x( )) % _volumeDescriptors.size();
        break;
    case TM_BOUNDED:
        result = std::max( std::min( timestamp, _timestampRange.y( )), _timestampRange.x( ));
    case TM_DEFAULT:
    default:
        result = timestamp;
    }
    return result;
}

VolumeHandler::VolumeDescriptor::VolumeDescriptor(
    const std::string& filename,
    const Vector3ui& dimensions,
    const Vector3f& elementSpacing,
    const Vector3f& offset )
    : _filename( filename )
    , _memoryMapPtr( 0 )
    , _cacheFileDescriptor( NO_DESCRIPTOR )
    , _dimensions( dimensions )
    , _elementSpacing( elementSpacing )
    , _offset( offset )
{
}

VolumeHandler::VolumeDescriptor::~VolumeDescriptor()
{
    unmap();
}

void VolumeHandler::VolumeDescriptor::map()
{
    _cacheFileDescriptor = open( _filename.c_str(), O_RDONLY );
    if( _cacheFileDescriptor == NO_DESCRIPTOR )
    {
        BRAYNS_ERROR << "Failed to attach " << _filename << std::endl;
        return;
    }

    struct stat sb;
    if( ::fstat( _cacheFileDescriptor, &sb ) == NO_DESCRIPTOR )
    {
        BRAYNS_ERROR << "Failed to attach " << _filename << std::endl;
        return;
    }

    _size = sb.st_size;
    _memoryMapPtr = ::mmap( 0, _size, PROT_READ, MAP_PRIVATE, _cacheFileDescriptor, 0 );
    if( _memoryMapPtr == MAP_FAILED )
    {
        _memoryMapPtr = 0;
        ::close( _cacheFileDescriptor );
        _cacheFileDescriptor = NO_DESCRIPTOR;
        BRAYNS_ERROR << "Failed to attach " << _filename << std::endl;
        return;
    }
}

void VolumeHandler::VolumeDescriptor::unmap()
{
    if( _memoryMapPtr )
    {
        ::munmap( (void*)_memoryMapPtr, _size );
        _memoryMapPtr = 0;
    }
    if( _cacheFileDescriptor != NO_DESCRIPTOR )
    {
        ::close( _cacheFileDescriptor );
        _cacheFileDescriptor = NO_DESCRIPTOR;
    }
}

}

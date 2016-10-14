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

namespace brayns
{

VolumeHandler::VolumeHandler(
    const VolumeParameters& volumeParameters )
    : _dimensions( volumeParameters.getDimensions() )
{
}

VolumeHandler::~VolumeHandler()
{
    for( auto memoryMapPtr: _memoryMapPtrs )
        if( memoryMapPtr.second )
            ::munmap( (void *)memoryMapPtr.second, _size );

    for( auto cacheFileDescriptor: _cacheFileDescriptors )
        if( cacheFileDescriptor.second != -1 )
            ::close( cacheFileDescriptor.second );
}

void VolumeHandler::attachVolumeToFile( const float timestamp, const std::string& volumeFile )
{
    BRAYNS_INFO << "Attaching " << volumeFile << " to timestamp " << timestamp << std::endl;
    _cacheFileDescriptors[ timestamp ] = open( volumeFile.c_str(), O_RDONLY );
    if( _cacheFileDescriptors[ timestamp ] == -1 )
        BRAYNS_THROW( std::runtime_error( "Failed to attach " + volumeFile ));

    struct stat sb;
    if( ::fstat( _cacheFileDescriptors[ timestamp ], &sb ) == -1 )
        BRAYNS_THROW( std::runtime_error( "Failed to attach " + volumeFile ));

    _memoryMapPtrs[ timestamp ] = ::mmap(
        0, sb.st_size, PROT_READ, MAP_PRIVATE, _cacheFileDescriptors[ timestamp ], 0 );
    if( _memoryMapPtrs[ timestamp ] == MAP_FAILED )
    {
        _memoryMapPtrs[ timestamp ] = 0;
        ::close( _cacheFileDescriptors[ timestamp ] );
        BRAYNS_THROW( std::runtime_error( "Failed to attach " + volumeFile ));
    }

    _size = sb.st_size;

    BRAYNS_INFO << "Volume size      : " << _size << " bytes ["
                <<  _size / 1048576 << " MB]" << std::endl;
    BRAYNS_INFO << "Volume dimensions: " << _dimensions << std::endl;
    BRAYNS_INFO << "Successfully attached to " << volumeFile << std::endl;
}

void* VolumeHandler::getData( const float timestamp )
{
    size_t ts = timestamp;
    ts = ts % _memoryMapPtrs.size();
    BRAYNS_INFO << "Assigning volume for timestamp " << ts << std::endl;
    return _memoryMapPtrs[ ts ];
}

float VolumeHandler::getEpsilon( const Vector3f& scale, const uint16_t samplesPerRay )
{
    Vector3f diag = Vector3f(_dimensions) * scale;
    return std::max(diag.x(), std::max( diag.y(), diag.z())) / float( samplesPerRay );
}

}

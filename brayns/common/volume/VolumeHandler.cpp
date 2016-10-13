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
    const VolumeParameters& volumeParameters,
    const std::string& volumeFile )
    : _memoryMapPtr( 0 )
    , _cacheFileDescriptor( -1 )
    , _dimensions( volumeParameters.getDimensions() )
{
    BRAYNS_INFO << "Attaching " << volumeFile << " to current scene" << std::endl;
    _cacheFileDescriptor = open( volumeFile.c_str(), O_RDONLY );
    if( _cacheFileDescriptor == -1 )
        BRAYNS_THROW( std::runtime_error( "Failed to attach " + volumeFile ));

    struct stat sb;
    if( ::fstat( _cacheFileDescriptor, &sb ) == -1 )
        BRAYNS_THROW( std::runtime_error( "Failed to attach " + volumeFile ));

    _memoryMapPtr = ::mmap(
        0, sb.st_size, PROT_READ, MAP_PRIVATE, _cacheFileDescriptor, 0 );
    if( _memoryMapPtr == MAP_FAILED )
    {
        _memoryMapPtr = 0;
        ::close( _cacheFileDescriptor );
        BRAYNS_THROW( std::runtime_error( "Failed to attach " + volumeFile ));
    }

    _size = sb.st_size;

    BRAYNS_INFO << "Volume size      : " << _size << " bytes ["
                <<  _size / 1048576 << " MB]" << std::endl;
    BRAYNS_INFO << "Volume dimensions: " << _dimensions << std::endl;
    BRAYNS_INFO << "Successfully attached to " << volumeFile << std::endl;
}

VolumeHandler::~VolumeHandler()
{
    if( _memoryMapPtr )
        ::munmap( (void *)_memoryMapPtr, _size );

    if( _cacheFileDescriptor != -1 )
        ::close( _cacheFileDescriptor );
}

void* VolumeHandler::getData()
{
    return _memoryMapPtr;
}

float VolumeHandler::getEpsilon( const Vector3f& scale, const uint16_t samplesPerRay )
{
    Vector3f diag = Vector3f(_dimensions) * scale;
    return std::max(diag.x(), std::max( diag.y(), diag.z())) / float( samplesPerRay );
}

}

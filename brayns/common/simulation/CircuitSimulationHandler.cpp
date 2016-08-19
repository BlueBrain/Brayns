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

#include "CircuitSimulationHandler.h"

#include <brayns/common/log.h>

#include <fstream>
#include <sys/mman.h>

namespace brayns
{

void* CircuitSimulationHandler::getFrameData( const float timestamp )
{
    if( _nbFrames ==  0 )
        return 0;

    const uint64_t frame = timestamp;
    const uint64_t moduloFrame = frame % _nbFrames;
    const uint64_t index = std::min( _frameSize, std::max( uint64_t(0), moduloFrame ));
    return (unsigned char*)_memoryMapPtr + _headerSize + index * _frameSize * sizeof(float);
}

}

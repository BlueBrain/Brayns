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

#ifndef SIMULATIONDESCRIPTOR_H
#define SIMULATIONDESCRIPTOR_H

#include <brayns/api.h>
#include <brayns/common/types.h>

namespace brayns
{

class SimulationDescriptor
{

public:

    SimulationDescriptor();
    ~SimulationDescriptor();

    /**
    * @brief Attaches a memory mapped file to the scene so that renderers can access the data
    *        as if it was in memory. The OS is in charge of dealing with the map file in system
    *        memory.
    * @param cacheFile File containing the simulation values
    * @return True if the file was successfully attached, false otherwise
    */
    BRAYNS_API bool attachSimulationToCacheFile( const std::string& cacheFile );

    /**
    * @brief Writes the header to a stream. The header contains the number of frames and the frame
    *        size.
    * @param stream Stream where the header should be written
    * @param nbFrames Number of frames
    * @param frameSize Frame size
    */
    BRAYNS_API static void writeHeader(
        std::ofstream& stream,
        uint64_t nbFrames,
        uint64_t frameSize );

    /**
    * @brief Writes a frame to a stream. A frame is a set of float values.
    * @param stream Stream where the header should be written
    * @param value Frame values
    */
    BRAYNS_API static void writeFrame(
        std::ofstream& stream,
        const floats& values );

    /**
     * @brief Returns the size of a given frame
     * @param frame Frame number
     * @return Size of given frame
     */
    uint64_t getFrameSize( const uint64_t ) { return _frameSize; }

    /**
     * @brief Returns a pointer to a given frame in the memory mapped file.
     * @param frame Frame number
     * @return Pointer to given frame
     */
    void* getFramePointer( const uint64_t frame );

private:

    uint64_t _headerSize;
    uint64_t _nbFrames;
    uint64_t _frameSize;
    void* _memoryMapPtr;
    int _cacheFileDescriptor;

};

}

#endif // SIMULATIONDESCRIPTOR_H

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

#ifndef VOLUMEHANDLER_H
#define VOLUMEHANDLER_H

#include <brayns/common/types.h>

namespace brayns
{

/**

   VolumeHandler object

   This object contains the attributes of a volume, as well as a pointer to the memory mapped file
   containing the 8bit data.

 */
class VolumeHandler
{

public:

    /**
    * @brief Attaches a memory mapped file to the scene so that renderers can access the data
    *        as if it was in memory. The OS is in charge of dealing with the map file in system
    *        memory.
    * @param volumeFile File containing the 8bit volume
    * @return True if the file was successfully attached, false otherwise
    */
    VolumeHandler(
        const VolumeParameters& volumeParameters,
        const std::string& volumeFile );
    ~VolumeHandler();

    /**
     * @brief Returns the dimension of the 8bit volume
     * @return Dimensions of the volume
     */
    const Vector3ui& getDimensions() const { return _dimensions; }

    /**
     * @brief Returns the size of the 8bit volume in bytes
     * @return Size of the volume
     */
    uint64_t getSize() const { return _size; }

    /**
     * @brief Returns a pointer to a given frame in the memory mapped file.
     * @return Pointer to volume
     */
    void* getData();

    /**
     * @brief Returns the epsilon that defines the step used to walk along the ray when traversing
     *        the volume. The value is defined according to the dimensions and scaling of the
     *        volume, and the number of samples per ray
     * @return Dimensions of the volume
     */
    float getEpsilon ( const Vector3f& scale, const uint16_t samplesPerRay );

private:

    void* _memoryMapPtr;
    int _cacheFileDescriptor;
    uint64_t _size;
    Vector3ui _dimensions;

};

}

#endif // VOLUMEHANDLER_H

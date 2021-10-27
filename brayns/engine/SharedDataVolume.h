/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel Nachbaur <daniel.nachbaur@epfl.ch>
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

#pragma once

#include <brayns/engine/Volume.h>

namespace brayns
{
/**
 * A volume type where the voxels are set once and only referenced from the
 * source location.
 */
class SharedDataVolume : public virtual Volume
{
public:
    /** @name API for engine-specific code */
    //@{
    virtual void setVoxels(const void* voxels) = 0;
    //@}

    /**
     * Convenience functions to use voxels from given file and pass them to
     * setVoxels().
     */
    void mapData(const std::string& filename);
    void mapData(const uint8_ts& buffer);
    void mapData(uint8_ts&& buffer);

protected:
    SharedDataVolume(const Vector3ui& dimensions, const Vector3f& spacing,
                     const DataType type)
        : Volume(dimensions, spacing, type)
    {
    }

    ~SharedDataVolume();

private:
    uint8_ts _memoryBuffer;
    void* _memoryMapPtr{nullptr};
    int _cacheFileDescriptor{-1};
    size_t _size{0};
};
} // namespace brayns

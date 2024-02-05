/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <brayns/engine/BrickedVolume.h>
#include <brayns/engine/SharedDataVolume.h>
#include <brayns/parameters/VolumeParameters.h>

#include <ospray/SDK/volume/Volume.h>

namespace brayns
{
class OSPRayVolume : public virtual Volume
{
public:
    OSPRayVolume(const Vector3ui& dimensions, const Vector3f& spacing,
                 const VolumeDataType type, const VolumeParameters& params,
                 OSPTransferFunction transferFunction,
                 const std::string& volumeType);

    ~OSPRayVolume();

    void setDataRange(const Vector2f& range) final;

    void commit() final;

    OSPVolume impl() const { return _volume; }

protected:
    size_t _dataSize{0};
    const VolumeParameters& _parameters;
    OSPVolume _volume;
    OSPDataType _ospType;
};

class OSPRayBrickedVolume : public BrickedVolume, public OSPRayVolume
{
public:
    OSPRayBrickedVolume(const Vector3ui& dimensions, const Vector3f& spacing,
                        const VolumeDataType type,
                        const VolumeParameters& params,
                        OSPTransferFunction transferFunction);

    void setBrick(const void* data, const Vector3ui& position,
                  const Vector3ui& size) final;
};

class OSPRaySharedDataVolume : public SharedDataVolume, public OSPRayVolume
{
public:
    OSPRaySharedDataVolume(const Vector3ui& dimensions, const Vector3f& spacing,
                           const VolumeDataType type,
                           const VolumeParameters& params,
                           OSPTransferFunction transferFunction);

    void setVoxels(const void* voxels) final;
};
} // namespace brayns

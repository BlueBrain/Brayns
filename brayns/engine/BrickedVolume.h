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

#include <brayns/engine/Volume.h>

#include <memory>

namespace brayns
{
/** A volume type where the voxels are copied for each added brick. */
class BrickedVolume : public virtual Volume
{
public:
    /** @name API for engine-specific code */
    //@{
    virtual void setBrick(const void* data, const Vector3ui& position,
                          const Vector3ui& size) = 0;
    //@}

protected:
    BrickedVolume(const Vector3ui& dimensions, const Vector3f& spacing,
                  const VolumeDataType type)
        : Volume(dimensions, spacing, type)
    {
    }
};

using BrickedVolumePtr = std::shared_ptr<BrickedVolume>;
} // namespace brayns

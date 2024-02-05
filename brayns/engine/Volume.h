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

#include <brayns/common/BaseObject.h>
#include <brayns/common/MathTypes.h>
#include <brayns/common/VolumeDataType.h>

#include <memory>

namespace brayns
{
/** A base class for volumes to share common properties. */
class Volume : public BaseObject
{
public:
    /** @name API for engine-specific code */
    //@{
    virtual void setDataRange(const Vector2f& range) = 0;

    virtual void commit() = 0;
    //@}

    Volume(const Vector3ui& dimensions, const Vector3f& spacing,
           const VolumeDataType type);

    size_t getSizeInBytes() const { return _sizeInBytes; }
    Boxd getBounds() const
    {
        return {{0, 0, 0},
                {_dimensions.x * _spacing.x, _dimensions.y * _spacing.y,
                 _dimensions.z * _spacing.z}};
    }

protected:
    std::atomic_size_t _sizeInBytes{0};
    const Vector3ui _dimensions;
    const Vector3f _spacing;
    const VolumeDataType _dataType;
};

using VolumePtr = std::shared_ptr<Volume>;
} // namespace brayns

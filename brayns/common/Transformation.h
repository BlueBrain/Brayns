/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
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

#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include <brayns/common/BaseObject.h>
#include <brayns/common/types.h>

SERIALIZATION_ACCESS(Transformation)

namespace brayns
{
/**
* @brief Defines the translation, rotation and scaling parameters to be applied
* to a scene asset.
*/
class Transformation : public BaseObject
{
public:
    const Vector3f& getTranslation() const { return _translation; }
    void setTranslation(const Vector3f& value)
    {
        _updateValue(_translation, value);
    }
    const Vector3f& getScale() const { return _scale; }
    void setScale(const Vector3f& value) { _updateValue(_scale, value); }
    const Quaternionf& getRotation() const { return _rotation; }
    void setRotation(const Quaternionf& value)
    {
        _updateValue(_rotation, value);
    }

    bool operator==(const Transformation& rhs) const
    {
        return _translation == rhs._translation && _rotation == rhs._rotation &&
               _scale == rhs._scale;
    }
    bool operator!=(const Transformation& rhs) const { return !(*this == rhs); }
private:
    Vector3f _translation{0.f, 0.f, 0.f};
    Vector3f _scale{1.f, 1.f, 1.f};
    Quaternionf _rotation;

    SERIALIZATION_FRIEND(Transformation)
};
}

#endif // TRANSFORMATION_H

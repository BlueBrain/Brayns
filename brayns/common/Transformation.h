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
* @brief Defines the translation, rotation and scale parameters to be applied
* to a scene asset.
*/
class Transformation : public BaseObject
{
public:
    Transformation() = default;

    Transformation(const Vector3f& translation, const Vector3f& scale,
                   const Quaternionf& rotation, const Vector3f& rotationCenter)
        : _translation(translation)
        , _scale(scale)
        , _rotation(rotation)
        , _rotationCenter(rotationCenter)
    {
    }

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
    const Vector3f& getRotationCenter() const { return _rotationCenter; }
    void setRotationCenter(const Vector3f& value)
    {
        _updateValue(_rotationCenter, value);
    }

    bool operator==(const Transformation& rhs) const
    {
        return _translation == rhs._translation && _rotation == rhs._rotation &&
               _scale == rhs._scale && _rotationCenter == rhs._rotationCenter;
    }
    bool operator!=(const Transformation& rhs) const { return !(*this == rhs); }
    // only applies rotation and translation, use scaling separately if needed
    Matrix4f toMatrix() const
    {
        Matrix4f matrix(getRotation(), getTranslation());
        return matrix;
    }

private:
    Vector3f _translation{0.f, 0.f, 0.f};
    Vector3f _scale{1.f, 1.f, 1.f};
    Quaternionf _rotation;
    Vector3f _rotationCenter{0.f, 0.f, 0.f};

    SERIALIZATION_FRIEND(Transformation)
};
inline Transformation operator*(const Transformation& a,
                                const Transformation& b)
{
    const auto matrix = a.toMatrix() * b.toMatrix();
    return {matrix.getTranslation(), a.getScale() * b.getScale(), matrix,
            a.getRotationCenter()};
}

inline Boxf transformBox(const Boxf& box, const Transformation& trafo)
{
    return {trafo.toMatrix() * box.getMin(), trafo.toMatrix() * box.getMax()};
}
}

#endif // TRANSFORMATION_H

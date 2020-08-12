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

#pragma once

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

    Transformation(const Vector3d& translation, const Vector3d& scale,
                   const Quaterniond& rotation, const Vector3d& rotationCenter)
        : _translation(translation)
        , _scale(scale)
        , _rotation(rotation)
        , _rotationCenter(rotationCenter)
    {
    }

    const Vector3d& getTranslation() const { return _translation; }
    void setTranslation(const Vector3d& value)
    {
        _updateValue(_translation, value);
    }
    const Vector3d& getScale() const { return _scale; }
    void setScale(const Vector3d& value) { _updateValue(_scale, value); }
    const Quaterniond& getRotation() const { return _rotation; }
    void setRotation(const Quaterniond& value)
    {
        _updateValue(_rotation, value);
    }
    const Vector3d& getRotationCenter() const { return _rotationCenter; }
    void setRotationCenter(const Vector3d& value)
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
    Matrix4d toMatrix(bool withScale = false) const
    {
        Matrix4d matrix;
        matrix = matrix * glm::translate(_translation);
        matrix = matrix * glm::translate(_rotationCenter);
        matrix = matrix * glm::toMat4(_rotation);
        matrix = matrix * glm::translate(-1.0 * _rotationCenter);
        if (withScale)
        {
            matrix = glm::scale(matrix, _scale);
            matrix[3][0] *= _scale.x;
            matrix[3][1] *= _scale.y;
            matrix[3][2] *= _scale.z;
        }
        return matrix;
    }

private:
    Vector3d _translation{0, 0, 0};
    Vector3d _scale{1, 1, 1};
    Quaterniond _rotation{1, 0, 0, 0};
    Vector3d _rotationCenter{0, 0, 0};

    SERIALIZATION_FRIEND(Transformation)
};
inline Transformation operator*(const Transformation& a,
                                const Transformation& b)
{
    const auto matrix = a.toMatrix() * b.toMatrix();
    return {matrix[3], a.getScale() * b.getScale(), matrix,
            a.getRotationCenter()};
}

inline Boxd transformBox(const Boxd& box, const Transformation& transformation)
{
    const auto& scale = transformation.getScale();
    return {transformation.toMatrix() * Vector4d(box.getMin(), 1.) *
                Vector4d(scale, 1.),
            transformation.toMatrix() * Vector4d(box.getMax(), 1.) *
                Vector4d(scale, 1.)};
}
} // namespace brayns

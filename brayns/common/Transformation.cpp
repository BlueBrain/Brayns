/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include "Transformation.h"

namespace brayns
{
Transformation::Transformation(
    const Vector3d &translation,
    const Vector3d &scale,
    const Quaterniond &rotation,
    const Vector3d &rotationCenter)
    : _translation(translation)
    , _scale(scale)
    , _rotation(rotation)
    , _rotationCenter(rotationCenter)
{
}

const Vector3d &Transformation::getTranslation() const
{
    return _translation;
}

void Transformation::setTranslation(const Vector3d &value)
{
    _updateValue(_translation, value);
}

const Vector3d &Transformation::getScale() const
{
    return _scale;
}

void Transformation::setScale(const Vector3d &value)
{
    _updateValue(_scale, value);
}

const Quaterniond &Transformation::getRotation() const
{
    return _rotation;
}

void Transformation::setRotation(const Quaterniond &value)
{
    _updateValue(_rotation, value);
}

const Vector3d &Transformation::getRotationCenter() const
{
    return _rotationCenter;
}

void Transformation::setRotationCenter(const Vector3d &value)
{
    _updateValue(_rotationCenter, value);
}

Matrix4d Transformation::toMatrix() const
{
    return glm::translate(Matrix4d(1.), _rotationCenter)
        * (glm::toMat4(_rotation)
           * (glm::translate(Matrix4d(1.), _translation - _rotationCenter) * glm::scale(Matrix4d(1.), _scale)));
}

bool Transformation::operator==(const Transformation &rhs) const
{
    return _translation == rhs._translation && _rotation == rhs._rotation && _scale == rhs._scale
        && _rotationCenter == rhs._rotationCenter;
}

bool Transformation::operator!=(const Transformation &rhs) const
{
    return !(*this == rhs);
}

Transformation operator*(const Transformation &a, const Transformation &b)
{
    return {
        a.getTranslation() + b.getTranslation(),
        a.getScale() * b.getScale(),
        a.getRotation() * b.getRotation(),
        a.getRotationCenter()};
}

Boxd transformBox(const Boxd &box, const Transformation &transformation)
{
    const brayns::Matrix4d matrix = transformation.toMatrix();
    return {matrix * Vector4d(box.getMin(), 1.), matrix * Vector4d(box.getMax(), 1.)};
}
} // namespace brayns

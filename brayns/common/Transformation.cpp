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

#include <brayns/common/Transformation.h>

namespace brayns
{
Transformation::Transformation(
    const Vector3f &translation,
    const Vector3f &scale,
    const Quaternion &rotation,
    const Vector3f &rotationCenter)
    : _translation(translation)
    , _scale(scale)
    , _rotation(rotation)
    , _rotationCenter(rotationCenter)
{
}

const Vector3f &Transformation::getTranslation() const noexcept
{
    return _translation;
}

void Transformation::setTranslation(const Vector3f &value) noexcept
{
    _updateValue(_translation, value);
}

const Vector3f &Transformation::getScale() const noexcept
{
    return _scale;
}

void Transformation::setScale(const Vector3f &value) noexcept
{
    _updateValue(_scale, value);
}

const Quaternion &Transformation::getRotation() const noexcept
{
    return _rotation;
}

void Transformation::setRotation(const Quaternion &value) noexcept
{
    _updateValue(_rotation, value);
}

const Vector3f &Transformation::getRotationCenter() const noexcept
{
    return _rotationCenter;
}

void Transformation::setRotationCenter(const Vector3f &value) noexcept
{
    _updateValue(_rotationCenter, value);
}

Matrix4f Transformation::toMatrix() const
{
    return glm::translate(Matrix4f(1.), _rotationCenter)
        * (glm::mat4_cast(_rotation)
           * (glm::translate(Matrix4f(1.), _translation - _rotationCenter) * glm::scale(Matrix4f(1.), _scale)));
}
} // namespace brayns

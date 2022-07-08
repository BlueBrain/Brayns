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

#include <brayns/common/Transform.h>

namespace brayns
{
Transform::Transform(const Vector3f &translation, const Quaternion &rotation, const Vector3f &scale)
    : _translation(translation)
    , _rotation(rotation)
    , _scale(scale)
{
}

const Vector3f &Transform::getTranslation() const noexcept
{
    return _translation;
}

void Transform::setTranslation(const Vector3f &value) noexcept
{
    _translation = value;
}

const Quaternion &Transform::getRotation() const noexcept
{
    return _rotation;
}

void Transform::setRotation(const Quaternion &value) noexcept
{
    _rotation = value;
}

const Vector3f &Transform::getScale() const noexcept
{
    return _scale;
}

void Transform::setScale(const Vector3f &value) noexcept
{
    _scale = value;
}

Matrix4f Transform::toMatrix() const
{
    return glm::translate(Matrix4f(1.), _translation) * glm::mat4_cast(_rotation) * glm::scale(Matrix4f(1.), _scale);
}

bool operator==(const Transform &lhs, const Transform &rhs)
{
    auto &lhsT = lhs.getTranslation();
    auto &lhsR = lhs.getRotation();
    auto &lhsS = lhs.getScale();

    auto &rhsT = rhs.getTranslation();
    auto &rhsR = rhs.getRotation();
    auto &rhsS = rhs.getScale();

    return lhsT == rhsT && lhsR == rhsR && lhsS == rhsS;
}

bool operator!=(const Transform &lhs, const Transform &rhs)
{
    return !(lhs == rhs);
}
} // namespace brayns

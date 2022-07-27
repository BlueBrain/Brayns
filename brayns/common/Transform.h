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

#pragma once

#include <brayns/common/MathTypes.h>

namespace brayns
{
/**
 * @brief Defines the translation, rotation and scale parameters to be applied
 * to a scene asset.
 */
class Transform
{
public:
    Transform() = default;

    Transform(const Vector3f &translation, const Quaternion &rotation, const Vector3f &scale);

    const Vector3f &getTranslation() const noexcept;
    void setTranslation(const Vector3f &value) noexcept;

    const Quaternion &getRotation() const noexcept;
    void setRotation(const Quaternion &value) noexcept;

    const Vector3f &getScale() const noexcept;
    void setScale(const Vector3f &value) noexcept;

    Matrix4f toMatrix() const;

private:
    Vector3f _translation{0.f};
    Quaternion _rotation{1, 0, 0, 0};
    Vector3f _scale{1.f};
};

bool operator==(const Transform &lhs, const Transform &rhs);
bool operator!=(const Transform &lhs, const Transform &rhs);
} // namespace brayns

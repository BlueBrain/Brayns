/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
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
#include <brayns/common/MathTypes.h>

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

    Transformation(
        const Vector3d &translation,
        const Vector3d &scale,
        const Quaterniond &rotation,
        const Vector3d &rotationCenter);

    const Vector3d &getTranslation() const;
    void setTranslation(const Vector3d &value);
    const Vector3d &getScale() const;
    void setScale(const Vector3d &value);
    const Quaterniond &getRotation() const;
    void setRotation(const Quaterniond &value);
    const Vector3d &getRotationCenter() const;
    void setRotationCenter(const Vector3d &value);
    Matrix4d toMatrix() const;

    bool operator==(const Transformation &rhs) const;
    bool operator!=(const Transformation &rhs) const;

private:
    Vector3d _translation{0, 0, 0};
    Vector3d _scale{1, 1, 1};
    Quaterniond _rotation{1, 0, 0, 0};
    Vector3d _rotationCenter{0, 0, 0};
};

Transformation operator*(const Transformation &a, const Transformation &b);

Boxd transformBox(const Boxd &box, const Transformation &transformation);
} // namespace brayns

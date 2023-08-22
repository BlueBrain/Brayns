/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include <brayns/utils/MathTypes.h>

namespace brayns
{
/**
 * @brief Defines the translation, rotation and scale parameters to be applied
 * to a scene asset.
 */
struct Transform
{
    Vector3f translation{0.f};
    Quaternion rotation{1, 0, 0, 0};
    Vector3f scale{1.f};

    TransformMatrix toMatrix() const noexcept;
    bool operator==(const Transform &other) const noexcept;
    bool operator!=(const Transform &other) const noexcept;
};
} // namespace brayns

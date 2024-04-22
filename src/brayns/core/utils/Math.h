/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#define RKCOMMON_NO_SIMD
#include <rkcommon/math/AffineSpace.h>
#include <rkcommon/math/Quaternion.h>
#include <rkcommon/math/vec.h>

namespace brayns
{
using Index3 = rkcommon::math::vec3ui;
using Index4 = rkcommon::math::vec4ui;

using Size3 = rkcommon::math::vec3ul;

using Vector2 = rkcommon::math::vec2f;
using Vector3 = rkcommon::math::vec3f;
using Vector4 = rkcommon::math::vec4f;

using Box1 = rkcommon::math::box1f;
using Box2 = rkcommon::math::box2f;
using Box3 = rkcommon::math::box3f;

using Color3 = rkcommon::math::vec3f;
using Color4 = rkcommon::math::vec4f;

using Quaternion = rkcommon::math::quaternionf;

using Affine3 = rkcommon::math::AffineSpace3f;

struct Transform
{
    Vector3 translation = {0, 0, 0};
    Quaternion rotation = {1, 0, 0, 0};
    Vector3 scale = {1, 1, 1};

    auto operator<=>(const Transform &other) const = default;
};

inline Affine3 toAffine(const Transform &transform)
{
    auto rotation = rkcommon::math::LinearSpace3f(transform.rotation);
    auto scale = rkcommon::math::LinearSpace3f::scale(transform.scale);
    auto &translation = transform.translation;
    return Affine3(rotation * scale, translation);
}
}

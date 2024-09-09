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

#include <cmath>

namespace brayns
{
template<typename T, int S>
using Vector = rkcommon::math::vec_t<T, S>;

using Index3 = Vector<std::uint32_t, 3>;
using Index4 = Vector<std::uint32_t, 4>;

using Size2 = Vector<std::size_t, 2>;
using Size3 = Vector<std::size_t, 3>;

using Stride2 = Vector<std::ptrdiff_t, 2>;
using Stride3 = Vector<std::ptrdiff_t, 3>;

using Vector2 = Vector<float, 2>;
using Vector3 = Vector<float, 3>;
using Vector4 = Vector<float, 4>;

using Color3 = Vector3;
using Color4 = Vector4;

using Quaternion = rkcommon::math::quaternionf;

template<typename T>
using Range = rkcommon::math::range_t<T>;

template<typename T, int S>
using BoxT = rkcommon::math::box_t<T, S>;

using Box1 = Range<float>;
using Box2 = BoxT<float, 2>;
using Box3 = BoxT<float, 3>;

using Affine2 = rkcommon::math::AffineSpace2f;
using Affine3 = rkcommon::math::AffineSpace3f;

template<typename T>
constexpr T radians(T degrees)
{
    return degrees * T(M_PI) / T(360);
}

template<typename T>
constexpr T degrees(T radians)
{
    return radians * T(360) / T(M_PI);
}

template<typename T, int S>
T reduceMultiply(const Vector<T, S> &value)
{
    return rkcommon::math::reduce_mul(value);
}

template<typename T, int S>
Vector<T, S> dot(const Vector<T, S> &left, const Vector<T, S> &right)
{
    return rkcommon::math::dot(left, right);
}

template<typename T>
Vector<T, 3> cross(const Vector<T, 3> &left, const Vector<T, 3> &right)
{
    return rkcommon::math::cross(left, right);
}

template<typename T, int S>
Vector<T, S> normalize(const Vector<T, S> &value)
{
    return rkcommon::math::normalize(value);
}

inline Quaternion normalize(const Quaternion &value)
{
    return rkcommon::math::normalize(value);
}

struct Transform
{
    Vector3 translation = {0.0F, 0.0F, 0.0F};
    Quaternion rotation = {1.0F, 0.0F, 0.0F, 0.0F};
    Vector3 scale = {1.0F, 1.0F, 1.0F};

    auto operator<=>(const Transform &other) const = default;
};

inline Affine3 toAffine(const Transform &transform)
{
    auto &translation = transform.translation;
    auto rotation = rkcommon::math::LinearSpace3f(transform.rotation);
    auto scale = rkcommon::math::LinearSpace3f::scale(transform.scale);
    return Affine3(rotation * scale, translation);
}
}

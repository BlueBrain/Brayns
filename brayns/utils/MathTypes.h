/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

// Rkcommon simd math code has big precission issues
// And previous math lib (glm) was being used without simd anyway
#define RKCOMMON_NO_SIMD
#include <rkcommon/math/AffineSpace.h>
#include <rkcommon/math/Quaternion.h>
#include <rkcommon/math/vec.h>

#include <concepts>

namespace brayns
{
namespace math = rkcommon::math;

/**
 * Vector definitions
 */
using Vector2i = math::vec2i;
using Vector2l = math::vec2l;
using Vector3i = math::vec3i;
using Vector3l = math::vec3l;

using Vector2ui = math::vec2ui;
using Vector2ul = math::vec2ul;
using Vector3ui = math::vec3ui;
using Vector3ul = math::vec3ul;

using Vector2f = math::vec2f;
using Vector3f = math::vec3f;
using Vector4f = math::vec4f;

/**
 * Quaternion definitions
 */
using Quaternion = math::quaternionf;

/**
 * AABB definitions
 */
using AxisAlignedBounds = math::box3f;

/**
 * 2D box
 */
using Box2 = math::box2f;

/**
 * Matrix definitions
 */
class TransformMatrix
{
public:
    TransformMatrix() = default;

    TransformMatrix(const Vector3f &translation, const Quaternion &rotation, const Vector3f &scale):
        affine(math::LinearSpace3f(rotation) * math::LinearSpace3f::scale(scale), translation)
    {
    }

    Vector3f transformPoint(const Vector3f &point) const
    {
        return math::xfmPoint(affine, point);
    }

    Vector3f transformVector(const Vector3f &vector) const
    {
        return math::xfmVector(affine, vector);
    }

    Vector3f transformNormal(const Vector3f &normal) const
    {
        return math::xfmNormal(affine, normal);
    }

    AxisAlignedBounds transformBounds(const AxisAlignedBounds &box) const
    {
        return math::xfmBounds(affine, box);
    }

    TransformMatrix &operator*=(const TransformMatrix &rhs)
    {
        affine *= rhs.affine;
        return *this;
    }

    TransformMatrix operator*(const TransformMatrix &rhs) const
    {
        TransformMatrix result;
        result.affine = affine * rhs.affine;
        return result;
    }

    math::AffineSpace3f affine = math::AffineSpace3f(math::OneTy());
};

} // namespace brayns

namespace rkcommon::math
{
template<typename T, int S>
constexpr vec_t<T, S> lerp(float alpha, const vec_t<T, S> &a, const vec_t<T, S> &b)
{
    constexpr auto limit = static_cast<std::size_t>(S);
    auto result = vec_t<T, S>();
    for (std::size_t i = 0; i < limit; ++i)
    {
        result[i] = a[i] * (1.f - alpha) + b[i] * alpha;
    }
    return result;
}

template<std::floating_point T, int S>
constexpr vec_t<std::int32_t, S> isfinite(const vec_t<T, S> &input)
{
    auto result = vec_t<std::int32_t, S>();

    for (int i = 0; i < S; ++i)
    {
        result[i] = std::isfinite(input[i]);
    }
    return result;
}
}

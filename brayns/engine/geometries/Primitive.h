/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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
#include <brayns/engine/RenderableType.h>

namespace brayns
{
struct Primitive
{
    Vector3f p0;
    float r0;
    Vector3f p1;
    float r1;

    static Primitive cylinder(const Vector3f &p0, const Vector3f &p1, const float radius) noexcept;

    static Primitive cone(const Vector3f &p0, const float r0, const Vector3f &p1, const float r1) noexcept;

    static Primitive sphere(const Vector3f &center, const float radius) noexcept;
};

template<>
std::string_view RenderableOSPRayID<Primitive>::get();

template<>
void RenderableBoundsUpdater<Primitive>::update(const Primitive &s, const Matrix4f &t, Bounds &b);

template<>
void Geometry<Primitive>::commitGeometrySpecificParams();
}

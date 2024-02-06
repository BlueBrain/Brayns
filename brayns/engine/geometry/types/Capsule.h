/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <brayns/engine/geometry/GeometryTraits.h>

namespace brayns
{
struct Capsule
{
    Vector3f p0;
    float r0;
    Vector3f p1;
    float r1;
};

class CapsuleFactory
{
public:
    static Capsule cylinder(const Vector3f &p0, const Vector3f &p1, const float radius) noexcept;

    static Capsule cone(const Vector3f &p0, const float r0, const Vector3f &p1, const float r1) noexcept;

    static Capsule sphere(const Vector3f &center, const float radius) noexcept;
};

template<>
class GeometryTraits<Capsule>
{
public:
    static inline const std::string handleName = "curve";
    static inline const std::string name = "capsule";

    static Bounds computeBounds(const TransformMatrix &matrix, const Capsule &data);
    static void updateData(ospray::cpp::Geometry &handle, std::vector<Capsule> &data);
};
}

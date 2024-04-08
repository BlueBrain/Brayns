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

#include <brayns/core/engine/geometry/GeometryTraits.h>

#include <ospray/ospray_cpp/Traits.h>

namespace brayns
{
struct Plane
{
    // A, B, C D from Ax + By + Cz + D = 0
    Vector4f coefficients;
};

template<>
class GeometryTraits<Plane>
{
public:
    static inline const std::string handleName = "plane";
    static inline const std::string name = "plane";

    static Bounds computeBounds(const TransformMatrix &matrix, const Plane &data);
    static void updateData(ospray::cpp::Geometry &handle, std::vector<Plane> &data);
};
}

namespace ospray
{
OSPTYPEFOR_SPECIALIZATION(brayns::Plane, OSP_VEC4F)
} // namespace ospray

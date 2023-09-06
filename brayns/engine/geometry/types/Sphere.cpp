/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include "Sphere.h"

#include <ospray/ospray_cpp/Data.h>

namespace
{
struct SphereParameters
{
    static inline const std::string position = "sphere.position";
    static inline const std::string radius = "sphere.radius";
};
}

namespace brayns
{
Bounds GeometryTraits<Sphere>::computeBounds(const TransformMatrix &matrix, const Sphere &data)
{
    Vector3f radiusDelta(data.radius);
    auto sphereMin = data.center - radiusDelta;
    auto sphereMax = data.center + radiusDelta;

    Bounds bounds;
    bounds.expand(matrix.transformPoint(sphereMin));
    bounds.expand(matrix.transformPoint(sphereMax));
    return bounds;
}

void GeometryTraits<Sphere>::updateData(ospray::cpp::Geometry &handle, std::vector<Sphere> &data)
{
    constexpr auto stride = 4 * sizeof(float);
    auto basePtr = &(data.front().center.x);
    auto positionPtr = basePtr;
    auto radiiPtr = basePtr + 3;
    auto size = data.size();

    ospray::cpp::SharedData positionData(positionPtr, OSP_VEC3F, size, stride);
    ospray::cpp::SharedData radiiData(radiiPtr, OSP_FLOAT, size, stride);
    handle.setParam(SphereParameters::position, positionData);
    handle.setParam(SphereParameters::radius, radiiData);
}
}

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

#include <brayns/engine/geometries/Sphere.h>

namespace brayns
{
std::string_view GeometryOSPRayID<Sphere>::get()
{
    return "sphere";
}

void GeometryBoundsUpdater<Sphere>::update(const Sphere &s, const Matrix4f &t, Bounds &b)
{
    const Vector3f radiusDelta(s.radius);
    auto sphereMin = s.center - radiusDelta;
    auto sphereMax = s.center + radiusDelta;

    sphereMin = Vector3f(t * Vector4f(sphereMin, 1.f));
    sphereMax = Vector3f(t * Vector4f(sphereMax, 1.f));

    b.expand(sphereMin);
    b.expand(sphereMax);
}

void GeometryCommitter<Sphere>::commit(OSPGeometry handle, const std::vector<Sphere> &geometries)
{
    constexpr auto stride = 4 * sizeof(float);
    auto basePtr = reinterpret_cast<const float *>(geometries.data());
    auto positionPtr = basePtr;
    auto radiiPtr = basePtr + 3;
    auto size = geometries.size();

    auto positionData = ospNewSharedData(positionPtr, OSP_VEC3F, size, stride);
    auto radiiData = ospNewSharedData(radiiPtr, OSP_FLOAT, size, stride);

    ospSetParam(handle, "sphere.position", OSP_DATA, &positionData);
    ospSetParam(handle, "sphere.radius", OSP_DATA, &radiiData);

    ospRelease(positionData);
    ospRelease(radiiData);
}
}

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

#include <brayns/engine/geometries/Primitive.h>

namespace brayns
{
Primitive Primitive::cylinder(const Vector3f& p0, const Vector3f& p1, const float radius) noexcept
{
    return Primitive {p0, radius, p1, radius};
}

Primitive Primitive::cone(const Vector3f& p0, const float r0, const Vector3f& p1, const float r1) noexcept
{
    return Primitive {p0, r0, p1, r1};
}

Primitive Primitive::sphere(const Vector3f& center, const float radius) noexcept
{
    return Primitive {center, radius, center + Vector3f(0.f, .01f, 0.f), radius};
}

template<>
void GeometryBoundsUpdater<Primitive>::update(const Primitive& p, const Matrix4f& t, Bounds& b)
{
    const Vector3f p0Delta (p.r0);
    const auto p0Min = p.p0 - p0Delta;
    const auto p0Max = p.p0 + p0Delta;
    const Vector3f p1Delta (p.r1);
    const auto p1Min = p.p1 - p1Delta;
    const auto p1Max = p.p1 + p1Delta;

    b.expand(Vector3f(t * Vector4f(p0Min, 1.f)));
    b.expand(Vector3f(t * Vector4f(p0Max, 1.f)));
    b.expand(Vector3f(t * Vector4f(p1Min, 1.f)));
    b.expand(Vector3f(t * Vector4f(p1Max, 1.f)));
}

template<>
void Geometry<Primitive>::initializeHandle()
{
    _handle = ospNewGeometry("curve");
}

template<>
void Geometry<Primitive>::commitGeometrySpecificParams()
{
    const auto numGeoms = _geometries.size();

    std::vector<uint32_t> indexData (numGeoms);
    for(uint32_t i = 0, index = 0; i < numGeoms; ++i, index = index + 2)
        indexData[i] = index;

    const auto type = OSP_ROUND;
    const auto basis = OSP_LINEAR;

    OSPData primitiveDataHandle = ospNewSharedData(_geometries.data(), OSPDataType::OSP_VEC4F, numGeoms * 2);

    OSPData indexSharedDataHandle = ospNewSharedData(indexData.data(), OSPDataType::OSP_UINT, numGeoms);
    OSPData indexDataHandle = ospNewData(OSPDataType::OSP_UINT, numGeoms);
    ospCopyData(indexSharedDataHandle, indexDataHandle);
    ospRelease(indexSharedDataHandle);

    ospSetParam(_handle, "type", OSP_UCHAR, &type);
    ospSetParam(_handle, "basis", OSP_UCHAR, &basis);
    ospSetParam(_handle, "vertex.position_radius", OSP_DATA, &primitiveDataHandle);
    ospSetParam(_handle, "index", OSP_DATA, &indexDataHandle);

    ospRelease(primitiveDataHandle);
    ospRelease(indexDataHandle);
}
}

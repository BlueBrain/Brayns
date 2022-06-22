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

#include "Primitive.h"

#include <ospray/ospray_cpp/Data.h>

namespace
{
struct CurveParameters
{
    inline static const std::string osprayName = "curve";
    inline static const std::string type = "type";
    inline static const std::string basis = "basis";
    inline static const std::string position = "vertex.position_radius";
    inline static const std::string index = "index";
};
}

namespace brayns
{
Primitive Primitive::cylinder(const Vector3f &p0, const Vector3f &p1, const float radius) noexcept
{
    return Primitive{p0, radius, p1, radius};
}

Primitive Primitive::cone(const Vector3f &p0, const float r0, const Vector3f &p1, const float r1) noexcept
{
    return Primitive{p0, r0, p1, r1};
}

Primitive Primitive::sphere(const Vector3f &center, const float radius) noexcept
{
    return Primitive{center, radius, center + Vector3f(0.f, .01f, 0.f), radius};
}

const std::string &OsprayGeometryName<Primitive>::get()
{
    return CurveParameters::osprayName;
}

void GeometryBoundsUpdater<Primitive>::update(const Primitive &p, const Matrix4f &t, Bounds &b)
{
    const Vector3f p0Delta(p.r0);
    const auto p0Min = p.p0 - p0Delta;
    const auto p0Max = p.p0 + p0Delta;
    const Vector3f p1Delta(p.r1);
    const auto p1Min = p.p1 - p1Delta;
    const auto p1Max = p.p1 + p1Delta;

    b.expand(Vector3f(t * Vector4f(p0Min, 1.f)));
    b.expand(Vector3f(t * Vector4f(p0Max, 1.f)));
    b.expand(Vector3f(t * Vector4f(p1Min, 1.f)));
    b.expand(Vector3f(t * Vector4f(p1Max, 1.f)));
}

void GeometryCommitter<Primitive>::commit(
    const ospray::cpp::Geometry &osprayGeometry,
    const std::vector<Primitive> &primitives)
{
    const auto numPrimitives = primitives.size();

    std::vector<uint32_t> indexData(numPrimitives);
    for (uint32_t i = 0, index = 0; i < numPrimitives; ++i, index = index + 2)
    {
        indexData[i] = index;
    }

    const auto type = OSPCurveType::OSP_ROUND;
    const auto basis = OSPCurveBasis::OSP_LINEAR;

    osprayGeometry.setParam(CurveParameters::type, type);
    osprayGeometry.setParam(CurveParameters::basis, basis);
    osprayGeometry.setParam(CurveParameters::index, ospray::cpp::CopiedData(indexData));
    ospray::cpp::SharedData primitivesData(primitives.data(), OSPDataType::OSP_VEC4F, numPrimitives * 2);
    osprayGeometry.setParam(CurveParameters::position, primitivesData);
}
}

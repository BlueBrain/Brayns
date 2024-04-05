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

#include "Capsule.h"

#include <ospray/ospray_cpp/Data.h>

namespace
{
struct CapsuleParameters
{
    static inline const std::string type = "type";
    static inline const std::string basis = "basis";
    static inline const std::string position = "vertex.position_radius";
    static inline const std::string index = "index";
};
}

namespace brayns
{
Capsule CapsuleFactory::cylinder(const Vector3f &p0, const Vector3f &p1, const float radius) noexcept
{
    return Capsule{p0, radius, p1, radius};
}

Capsule CapsuleFactory::cone(const Vector3f &p0, const float r0, const Vector3f &p1, const float r1) noexcept
{
    return Capsule{p0, r0, p1, r1};
}

Capsule CapsuleFactory::sphere(const Vector3f &center, const float radius) noexcept
{
    return Capsule{center, radius, center + Vector3f(0.f, .01f, 0.f), radius};
}

Bounds GeometryTraits<Capsule>::computeBounds(const TransformMatrix &matrix, const Capsule &data)
{
    Vector3f p0Delta(data.r0);
    auto p0Min = data.p0 - p0Delta;
    auto p0Max = data.p0 + p0Delta;
    Vector3f p1Delta(data.r1);
    auto p1Min = data.p1 - p1Delta;
    auto p1Max = data.p1 + p1Delta;

    Bounds bounds;
    bounds.expand(matrix.transformPoint(p0Min));
    bounds.expand(matrix.transformPoint(p0Max));
    bounds.expand(matrix.transformPoint(p1Min));
    bounds.expand(matrix.transformPoint(p1Max));
    return bounds;
}

void GeometryTraits<Capsule>::updateData(ospray::cpp::Geometry &handle, std::vector<Capsule> &data)
{
    auto capsuleCount = data.size();

    std::vector<uint32_t> indexData;
    indexData.reserve(capsuleCount);
    for (uint32_t i = 0, index = 0; i < capsuleCount; ++i, index = index + 2)
    {
        indexData.push_back(index);
    }

    const auto type = OSPCurveType::OSP_ROUND;
    const auto basis = OSPCurveBasis::OSP_LINEAR;

    handle.setParam(CapsuleParameters::type, type);
    handle.setParam(CapsuleParameters::basis, basis);
    handle.setParam(CapsuleParameters::index, ospray::cpp::CopiedData(indexData));
    ospray::cpp::SharedData primitivesData(data.data(), OSPDataType::OSP_VEC4F, capsuleCount * 2);
    handle.setParam(CapsuleParameters::position, primitivesData);
}
}

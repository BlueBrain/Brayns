/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Jonas Karlsson <jonas.karlsson@epfl.ch>
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

namespace brayns
{
enum class SDFType : uint8_t
{
    Sphere = 0,
    Pill = 1,
    ConePill = 2,
    ConePillSigmoid = 3
};

struct SDFGeometry
{
    uint64_t userData{};
    Vector3f center; // TO BE REMOVED, for v3 cache compatibility only
    Vector3f p0;
    Vector3f p1;
    float r0 = -1.f;
    float r1 = -1.f;
    uint64_t neighboursIndex = 0;
    uint8_t numNeighbours = 0;
    SDFType type;
};

inline SDFGeometry createSDFSphere(const Vector3f& center, const float radius,
                                   const uint64_t data = 0)
{
    SDFGeometry geom{};
    geom.userData = data;
    geom.p0 = geom.p1 = center;
    geom.r0 = geom.r1 = radius;
    geom.type = SDFType::Sphere;
    return geom;
}

inline SDFGeometry createSDFPill(const Vector3f& p0, const Vector3f& p1,
                                 const float radius, const uint64_t data = 0)
{
    SDFGeometry geom{};
    geom.userData = data;
    geom.p0 = p0;
    geom.p1 = p1;
    geom.r0 = geom.r1 = radius;
    geom.type = SDFType::Pill;
    return geom;
}

inline SDFGeometry createSDFConePill(const Vector3f& p0, const Vector3f& p1,
                                     const float r0, const float r1,
                                     const uint64_t data = 0)
{
    SDFGeometry geom{};
    geom.userData = data;
    geom.p0 = p0;
    geom.p1 = p1;
    geom.r0 = r0;
    geom.r1 = r1;

    if (r0 < r1)
    {
        std::swap(geom.p0, geom.p1);
        std::swap(geom.r0, geom.r1);
    }

    geom.type = SDFType::ConePill;
    return geom;
}

inline SDFGeometry createSDFConePillSigmoid(const Vector3f& p0,
                                            const Vector3f& p1, const float r0,
                                            const float r1,
                                            const uint64_t data = 0)
{
    SDFGeometry geom = createSDFConePill(p0, p1, r0, r1, data);
    geom.type = SDFType::ConePillSigmoid;
    return geom;
}

inline Boxd getSDFBoundingBox(const SDFGeometry& geom)
{
    Boxd bounds;
    switch (geom.type)
    {
    case brayns::SDFType::Sphere:
    {
        bounds.merge(geom.p0 - Vector3f(geom.r0));
        bounds.merge(geom.p0 + Vector3f(geom.r0));
        break;
    }
    case brayns::SDFType::Pill:
    {
        bounds.merge(geom.p0 - Vector3f(geom.r0));
        bounds.merge(geom.p0 + Vector3f(geom.r0));
        bounds.merge(geom.p1 - Vector3f(geom.r0));
        bounds.merge(geom.p1 + Vector3f(geom.r0));
        break;
    }
    case brayns::SDFType::ConePill:
    case brayns::SDFType::ConePillSigmoid:
    {
        bounds.merge(geom.p0 - Vector3f(geom.r0));
        bounds.merge(geom.p0 + Vector3f(geom.r0));
        bounds.merge(geom.p1 - Vector3f(geom.r1));
        bounds.merge(geom.p1 + Vector3f(geom.r1));
        break;
    }
    default:
        throw std::runtime_error("No bounds found for SDF type.");
    }
    return bounds;
}

} // namespace brayns

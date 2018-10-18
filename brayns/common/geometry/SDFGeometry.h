/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
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

#include <brayns/common/types.h>

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
    uint64_t userData;
    Vector3f center;
    Vector3f p0;
    Vector3f p1;
    float radius = -1.f;
    float radius_tip = -1.f;
    uint64_t neighboursIndex = 0;
    uint8_t numNeighbours = 0;
    SDFType type;
};

inline SDFGeometry createSDFSphere(const Vector3f& center, const float radius,
                                   const uint64_t data = 0)
{
    SDFGeometry geom;
    geom.userData = data;
    geom.center = center;
    geom.radius = radius;
    geom.type = SDFType::Sphere;
    return geom;
}

inline SDFGeometry createSDFPill(const Vector3f& p0, const Vector3f& p1,
                                 const float radius, const uint64_t data = 0)
{
    SDFGeometry geom;
    geom.userData = data;
    geom.p0 = p0;
    geom.p1 = p1;
    geom.radius = radius;
    geom.type = SDFType::Pill;
    return geom;
}

inline SDFGeometry createSDFConePill(const Vector3f& p0, const Vector3f& p1,
                                     const float radiusBottom,
                                     const float radiusTip,
                                     const uint64_t data = 0)
{
    SDFGeometry geom;
    geom.userData = data;
    geom.p0 = p0;
    geom.p1 = p1;
    geom.radius = radiusBottom;
    geom.radius_tip = radiusTip;

    if (radiusBottom < radiusTip)
    {
        std::swap(geom.p0, geom.p1);
        std::swap(geom.radius, geom.radius_tip);
    }

    geom.type = SDFType::ConePill;
    return geom;
}

inline SDFGeometry createSDFConePillSigmoid(const Vector3f& p0,
                                            const Vector3f& p1,
                                            const float radiusBottom,
                                            const float radiusTip,
                                            const uint64_t data = 0)
{
    SDFGeometry geom = createSDFConePill(p0, p1, radiusBottom, radiusTip, data);
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
        bounds.merge(geom.center - Vector3f(geom.radius));
        bounds.merge(geom.center + Vector3f(geom.radius));
        break;
    }
    case brayns::SDFType::Pill:
    {
        bounds.merge(geom.p0 - Vector3f(geom.radius));
        bounds.merge(geom.p0 + Vector3f(geom.radius));
        bounds.merge(geom.p1 - Vector3f(geom.radius));
        bounds.merge(geom.p1 + Vector3f(geom.radius));
        break;
    }
    case brayns::SDFType::ConePill:
    case brayns::SDFType::ConePillSigmoid:
    {
        bounds.merge(geom.p0 - Vector3f(geom.radius));
        bounds.merge(geom.p0 + Vector3f(geom.radius));
        bounds.merge(geom.p1 - Vector3f(geom.radius_tip));
        bounds.merge(geom.p1 + Vector3f(geom.radius_tip));
        break;
    }
    default:
        throw std::runtime_error("No bounds found for SDF type.");
    }
    return bounds;
}

} // namespace brayns

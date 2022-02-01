/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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
    uint64_t userData = 0;
    Vector3f center; // TO BE REMOVED, for v3 cache compatibility only
    Vector3f p0;
    Vector3f p1;
    float r0 = -1.f;
    float r1 = -1.f;
    uint64_t neighboursIndex = 0;
    uint8_t numNeighbours = 0;
    SDFType type;
};

SDFGeometry createSDFSphere(const Vector3f &center, const float radius, const uint64_t data = 0);

SDFGeometry createSDFPill(const Vector3f &p0, const Vector3f &p1, const float radius, const uint64_t data = 0);

SDFGeometry
    createSDFConePill(const Vector3f &p0, const Vector3f &p1, const float r0, const float r1, const uint64_t data = 0);

SDFGeometry createSDFConePillSigmoid(
    const Vector3f &p0,
    const Vector3f &p1,
    const float r0,
    const float r1,
    const uint64_t data = 0);

Boxd getSDFBoundingBox(const SDFGeometry &geom);
} // namespace brayns

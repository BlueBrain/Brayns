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

#pragma once

#include <brayns/common/MathTypes.h>
#include <brayns/engine/Geometry.h>
#include <brayns/json/JsonAdapterMacro.h>

#include <vector>

namespace brayns
{
struct Plane
{
    // A, B, C D from Ax + By + Cz + D = 0
    Vector4f coefficents;
};

template<>
void GeometryBoundsUpdater<Plane>::update(const Plane& p, const Matrix4f& t, Bounds& b);

template<>
void Geometry<Plane>::initializeHandle();

template<>
void Geometry<Plane>::commitGeometrySpecificParams();

BRAYNS_JSON_ADAPTER_BEGIN(Plane)
BRAYNS_JSON_ADAPTER_ENTRY(coefficents, "Plane equation coefficents (A, B, C, D from Ax + By + Cz + D = 0)")
BRAYNS_JSON_ADAPTER_END()
}

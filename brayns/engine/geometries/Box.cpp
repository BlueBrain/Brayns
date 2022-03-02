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

#include <brayns/engine/geometries/Box.h>

namespace brayns
{
template<>
void GeometryBoundsUpdater<Box>::update(const Box& box, const Matrix4f& t, Bounds& b)
{
    const auto& min = box.min;
    const auto& max = box.max;

    b.expand(Vector3f(t * Vector4f(min, 1.f)));
    b.expand(Vector3f(t * Vector4f(max, 1.f)));
}

template<>
void Geometry<Box>::initializeHandle()
{
    _handle = ospNewGeometry("box");
}

template<>
void Geometry<Box>::commitGeometrySpecificParams()
{
    const auto numGeoms = _geometries.size();

    OSPData boxDataHandle = ospNewSharedData(_geometries.data(), OSPDataType::OSP_VEC3F, numGeoms * 2);

    ospSetParam(_handle, "box", OSP_DATA, &boxDataHandle);

    ospRelease(boxDataHandle);
}
}
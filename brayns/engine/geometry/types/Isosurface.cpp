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

#include "Isosurface.h"

#include <ospray/ospray_cpp/Data.h>

namespace
{
struct IsosurfaceParameters
{
    static inline const std::string volume = "volume";
    static inline const std::string isovalue = "isovalue";
};
}

namespace brayns
{
Bounds GeometryTraits<Isosurface>::computeBounds(const Matrix4f &matrix, const Isosurface &data)
{
    auto &volume = data.volume;
    return volume.computeBounds(matrix);
}

void GeometryTraits<Isosurface>::updateData(ospray::cpp::Geometry &handle, std::vector<Isosurface> &primitives)
{
    assert(primitives.size() == 1);
    auto &primitive = primitives.front();
    auto &isoValues = primitive.isovalues;
    auto &volume = primitive.volume;

    volume.commit();
    handle.setParam(IsosurfaceParameters::volume, volume.getHandle());
    handle.setParam(IsosurfaceParameters::isovalue, ospray::cpp::SharedData(isoValues));
}
}

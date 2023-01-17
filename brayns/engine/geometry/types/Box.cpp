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

#include "Box.h"

#include <ospray/ospray_cpp/Data.h>

namespace
{
struct BoxParameters
{
    inline static const std::string box = "box";
};
}

namespace brayns
{
Bounds GeometryTraits<Box>::computeBounds(const Matrix4f &matrix, const Box &box)
{
    const auto &min = box.min;
    const auto &max = box.max;
    Bounds bounds;
    bounds.expand(matrix * Vector4f(min, 1.f));
    bounds.expand(matrix * Vector4f(max.x, min.y, min.z, 1.f));
    bounds.expand(matrix * Vector4f(min.x, min.y, max.z, 1.f));
    bounds.expand(matrix * Vector4f(max.x, min.y, max.z, 1.f));
    bounds.expand(matrix * Vector4f(min.x, max.y, min.z, 1.f));
    bounds.expand(matrix * Vector4f(max.x, max.y, min.z, 1.f));
    bounds.expand(matrix * Vector4f(min.x, max.y, max.z, 1.f));
    bounds.expand(matrix * Vector4f(max, 1.f));
    return bounds;
}

void GeometryTraits<Box>::updateData(ospray::cpp::Geometry &handle, std::vector<Box> &data)
{
    handle.setParam(BoxParameters::box, ospray::cpp::SharedData(data));
}
}

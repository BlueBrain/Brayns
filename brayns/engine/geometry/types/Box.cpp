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

#include "Box.h"

#include <ospray/ospray_cpp/Data.h>

namespace
{
struct BoxParameters
{
    inline static const std::string osprayName = "box";
    inline static const std::string box = "box";
};
}

namespace brayns
{
const std::string &OsprayGeometryName<Box>::get()
{
    return BoxParameters::osprayName;
}

void GeometryBoundsUpdater<Box>::update(const Box &box, const Matrix4f &t, Bounds &b)
{
    const auto &min = box.min;
    const auto &max = box.max;

    b.expand(Vector3f(t * Vector4f(min, 1.f)));
    b.expand(Vector3f(t * Vector4f(max, 1.f)));
}

void GeometryCommitter<Box>::commit(const ospray::cpp::Geometry &osprayGeometry, const std::vector<Box> &primitives)
{
    osprayGeometry.setParam(BoxParameters::box, ospray::cpp::SharedData(primitives));
}
}

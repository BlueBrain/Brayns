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

#include "Plane.h"

#include <ospray/ospray_cpp/Data.h>

namespace brayns
{
std::string_view OsprayGeometryName<Plane>::get()
{
    return "plane";
}

void GeometryBoundsUpdater<Plane>::update(const Plane &p, const Matrix4f &t, Bounds &b)
{
    // NOOP
    // Planes are infinite. They can be limited, but on Brayns we only use them for clipping
    // https://github.com/ospray/ospray#planes
    (void)p;
    (void)t;
    (void)b;
}

void GeometryCommitter<Plane>::commit(const ospray::cpp::Geometry &osprayGeometry, const std::vector<Plane> &primitives)
{
    static const std::string coefficientsParameter = "plane.coefficients";
    osprayGeometry.setParam(coefficientsParameter, ospray::cpp::SharedData(primitives));
}
}

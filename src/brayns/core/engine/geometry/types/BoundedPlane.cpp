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

#include "BoundedPlane.h"

#include <ospray/ospray_cpp/Data.h>

namespace
{
struct PlaneParameters
{
    static inline const std::string coefficients = "plane.coefficients";
    static inline const std::string bounds = "plane.bounds";
};
}

namespace brayns
{
Bounds GeometryTraits<BoundedPlane>::computeBounds(const TransformMatrix &matrix, const BoundedPlane &data)
{
    return GeometryTraits<Box>::computeBounds(matrix, data.bounds);
}

void GeometryTraits<BoundedPlane>::updateData(ospray::cpp::Geometry &handle, std::vector<BoundedPlane> &data)
{
    auto stride = sizeof(BoundedPlane);

    auto &first = data.front();
    auto size = data.size();

    auto initCoefficients = &first.coefficients.x;
    auto coefficientsData = ospray::cpp::SharedData(initCoefficients, OSPDataType::OSP_VEC4F, size, stride);
    handle.setParam(PlaneParameters::coefficients, coefficientsData);

    auto initBounds = &first.bounds.min.x;
    auto boundsData = ospray::cpp::SharedData(initBounds, OSPDataType::OSP_BOX3F, size, stride);
    handle.setParam(PlaneParameters::bounds, boundsData);
}
}

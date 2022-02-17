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

#include <brayns/io/loaders/xyzb/XYZBModel.h>

namespace brayns
{
XYZBModel::XYZBModel(const std::vector<Sphere>& spheres)
{
    _geometry.add(spheres);
}

Bounds XYZBModel::computeBounds(const Matrix4f& transform) const noexcept
{
    return _geometry.computeBounds(transform);
}

void XYZBModel::setColor(const Vector4f &color) noexcept
{
    _updateValue(_color, color);
}

const Vector4f &XYZBModel::getColor() const noexcept
{
    return _color;
}

void XYZBModel::setRadius(const float newRadius) noexcept
{
    _geometry.mainpulateAll([&](const uint32_t index, Sphere& sphere)
    {
        (void) index;
        sphere.radius = newRadius;
    });

    markModified(false);
}

void XYZBModel::commitGeometryModel()
{
    auto ospHandle = handle();

    _geometry.commit();

    auto geomHandle = _geometry.handle();
    ospSetParam(ospHandle, "geometry", OSPDataType::OSP_GEOMETRY, &geomHandle);
    ospSetParam(ospHandle, "color", OSPDataType::OSP_VEC4F, &_color);
}
}

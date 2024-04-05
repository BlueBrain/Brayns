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

#include "GeometryView.h"

namespace
{
struct GeometryViewParameters
{
    static inline const std::string material = "material";
    static inline const std::string color = "color";
    static inline const std::string index = "index";
    static inline const std::string invertNormals = "invertNormals";
};
}

namespace brayns
{
GeometryView::GeometryView(const Geometry &geometry):
    _handle(geometry.getHandle())
{
    (void)geometry;
}

void GeometryView::setMaterial(const Material &material)
{
    _handle.setParam(GeometryViewParameters::material, material.getHandle());
    _flag.setModified(true);
}

void GeometryView::setNormalsInverted(bool inverted)
{
    _handle.setParam(GeometryViewParameters::invertNormals, inverted);
    _flag.setModified(true);
}

void GeometryView::setColor(const Vector3f &color)
{
    setColor(Vector4f(color, 1.f));
}

void GeometryView::setColor(const Vector4f &color)
{
    _handle.setParam(GeometryViewParameters::color, color);
    _handle.removeParam(GeometryViewParameters::index);
    _flag.setModified(true);
}

bool GeometryView::commit()
{
    if (!_flag)
    {
        return false;
    }
    _flag.setModified(false);
    _handle.commit();
    return true;
}

const ospray::cpp::GeometricModel &GeometryView::getHandle() const noexcept
{
    return _handle;
}

void GeometryView::_setColorPerPrimitive(const OSPData handle)
{
    _handle.setParam(GeometryViewParameters::color, OSPDataType::OSP_DATA, &handle);
    _handle.removeParam(GeometryViewParameters::index);
    _flag.setModified(true);
}

void GeometryView::_setColorMap(const OSPData indexHandle, const OSPData colorHandle)
{
    _handle.setParam(GeometryViewParameters::index, OSPDataType::OSP_DATA, &indexHandle);
    _handle.setParam(GeometryViewParameters::color, OSPDataType::OSP_DATA, &colorHandle);
    _flag.setModified(true);
}
}

/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "GeometryObject.h"

#include <brayns/engine/ospray/OsprayMathtypesTraits.h>

namespace
{
struct GeometricModelParameters
{
    static inline const std::string material = "material";
    static inline const std::string color = "color";
    static inline const std::string index = "index";
};
}

namespace brayns
{
void GeometryObject::setMaterial(const Material &material)
{
    const auto &osprayMaterial = material.getOsprayMaterial();
    _osprayObject.setParam(GeometricModelParameters::material, osprayMaterial);
}

void GeometryObject::setColor(const brayns::Vector3f &color)
{
    setColor(brayns::Vector4f(color, 1.f));
}

void GeometryObject::setColor(const brayns::Vector4f &color)
{
    _osprayObject.removeParam(GeometricModelParameters::index);
    _osprayObject.setParam(GeometricModelParameters::color, color);
}

void GeometryObject::setColorPerPrimitive(OSPData colors)
{
    _osprayObject.removeParam(GeometricModelParameters::index);
    _osprayObject.setParam(GeometricModelParameters::color, colors);
}

void GeometryObject::setColorMap(OSPData colors, OSPData indices)
{
    _osprayObject.setParam(GeometricModelParameters::color, colors);
    _osprayObject.setParam(GeometricModelParameters::index, indices);
}

void GeometryObject::commit()
{
    _osprayObject.commit();
}

const ospray::cpp::GeometricModel &GeometryObject::getOsprayObject() const noexcept
{
    return _osprayObject;
}
}

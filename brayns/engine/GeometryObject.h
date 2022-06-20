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

#pragma once

#include "Geometry.h"
#include "Material.h"

#include <ospray/ospray_cpp/Data.h>
#include <ospray/ospray_cpp/GeometricModel.h>
#include <ospray/ospray_cpp/Geometry.h>

namespace brayns
{
class GeometryObject
{
public:
    template<typename T>
    GeometryObject(const Geometry<T> &geometry)
        : _osprayObject(geometry.getOsprayGeometry())
    {
    }

    void setMaterial(const Material &material);
    void setColor(const brayns::Vector3f &color);
    void setColor(const brayns::Vector4f &color);
    void setColorPerPrimitive(OSPData colors);
    void setColorMap(OSPData colors, OSPData indices);
    void commit();
    const ospray::cpp::GeometricModel &getOsprayObject() const noexcept;

private:
    ospray::cpp::GeometricModel _osprayObject;
};
}

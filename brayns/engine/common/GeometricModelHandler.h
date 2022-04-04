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

#pragma once

#include <brayns/engine/Material.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/RenderableType.h>
#include <brayns/engine/common/DataHandler.h>

#include <ospray/ospray.h>

namespace brayns
{
struct GeometricModelHandler
{
    static OSPGeometricModel create();

    static void destory(OSPGeometricModel &model);

    template<typename T>
    static void setGeometry(OSPGeometricModel model, Geometry<T> &geometry)
    {
            auto geometryHandle = geometry.handle();
            ospSetParam(model, "geometry", OSPDataType::OSP_GEOMETRY, &geometryHandle);
    }

    static void setMaterial(OSPGeometricModel model, Material &material);

    static void setColor(OSPGeometricModel model, const Vector3f &color);

    static void setColor(OSPGeometricModel model, const Vector4f &color);

    static void setColors(OSPGeometricModel model, OSPBuffer &colors);

    static void setColorMap(OSPGeometricModel model, OSPBuffer &colors, OSPBuffer &indices);

    static void commitModel(OSPGeometricModel model);

    static void addToGeometryGroup(OSPGeometricModel model, Model &group);

    static void removeFromGeometryGroup(OSPGeometricModel model, Model &group);

    static void addToClippingGroup(OSPGeometricModel model, Model &group);

    static void removeFromClippingGroup(OSPGeometricModel model, Model &group);
};
}

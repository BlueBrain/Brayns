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

#include "GeometricModelHandler.h"

namespace brayns
{
OSPGeometricModel GeometricModelHandler::create()
{
    return ospNewGeometricModel();
}

void GeometricModelHandler::destroy(OSPGeometricModel &model)
{
    if (model)
    {
        ospRelease(model);
        model = nullptr;
    }
}

void GeometricModelHandler::setMaterial(OSPGeometricModel model, Material &material)
{
    auto handle = material.handle();
    ospSetParam(model, "material", OSPDataType::OSP_MATERIAL, &handle);
}

void GeometricModelHandler::setColor(OSPGeometricModel model, const Vector3f &color)
{
    setColor(model, Vector4f(color, 1.f));
}

void GeometricModelHandler::setColor(OSPGeometricModel model, const Vector4f &color)
{
    ospSetParam(model, "color", OSPDataType::OSP_VEC4F, &color);
    ospRemoveParam(model, "index");
}

void GeometricModelHandler::setColors(OSPGeometricModel model, OSPBuffer &colors)
{
    ospSetParam(model, "color", OSPDataType::OSP_DATA, &colors.handle);
    ospRemoveParam(model, "index");
}

void GeometricModelHandler::setColorMap(OSPGeometricModel model, OSPBuffer &colors, OSPBuffer &indices)
{
    ospSetParam(model, "color", OSPDataType::OSP_DATA, &colors.handle);
    ospSetParam(model, "index", OSPDataType::OSP_DATA, &indices.handle);
}

void GeometricModelHandler::commitModel(OSPGeometricModel model)
{
    ospCommit(model);
}

void GeometricModelHandler::addToGeometryGroup(OSPGeometricModel model, Model &group)
{
    auto &groupObj = group.getGroup();
    groupObj.addGeometricModel(model);
}

void GeometricModelHandler::removeFromGeometryGroup(OSPGeometricModel model, Model &group)
{
    auto &groupObj = group.getGroup();
    groupObj.removeGeometricModel(model);
}

void GeometricModelHandler::addToClippingGroup(OSPGeometricModel model, Model &group)
{
    auto &groupObj = group.getGroup();
    groupObj.addClippingModel(model);
}

void GeometricModelHandler::removeFromClippingGroup(OSPGeometricModel model, Model &group)
{
    auto &groupObj = group.getGroup();
    groupObj.removeClippingModel(model);
}
}

/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include "GeometricModel.h"

namespace
{
using namespace brayns;

void setMaterialParams(OSPGeometricModel handle, const Materials &materials)
{
    setObjectData(handle, "material", materials.materials);
    setObjectDataIfNotEmpty(handle, "color", materials.colors);
    setObjectDataIfNotEmpty(handle, "index", materials.indices);
}
}

namespace brayns
{
void GeometricModel::setMaterials(const Materials &materials)
{
    auto handle = getHandle();
    setMaterialParams(handle, materials);
    commitObject(handle);
}

void GeometricModel::invertNormals(bool invert)
{
    auto handle = getHandle();
    setObjectParam(handle, "invertNormals", invert);
    commitObject(handle);
}

GeometricModel createGeometricModel(Device &device, const GeometricModelSettings &settings)
{
    auto handle = ospNewGeometricModel();
    auto model = wrapObjectHandleAs<GeometricModel>(device, handle);

    setObjectParam(handle, "geometry", settings.geometry);
    setMaterialParams(handle, settings.materials);
    setObjectParam(handle, "invertNormals", settings.invertedNormals);
    setObjectParam(handle, "id", settings.id);

    commitObject(handle);

    return model;
}
}

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
using brayns::Color4;
using namespace brayns::experimental;

void setMaterialParams(OSPGeometricModel handle, const PrimitiveMaterials &materials)
{
    setObjectData(handle, "material", materials.rendererIndices);
    setObjectDataIfNotEmpty(handle, "color", materials.colors);
    setObjectDataIfNotEmpty(handle, "index", materials.indices);
}
}

namespace brayns::experimental
{
void GeometricModel::setMaterials(const PrimitiveMaterials &materials)
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

OSPGeometricModel ObjectReflector<GeometricModel>::createHandle(OSPDevice device, const Settings &settings)
{
    auto handle = ospNewGeometricModel();
    throwLastDeviceErrorIfNull(device, handle);

    setObjectParam(handle, "geometry", settings.geometry);
    setMaterialParams(handle, settings.materials);
    setObjectParam(handle, "invertNormals", settings.invertedNormals);
    setObjectParam(handle, "id", settings.id);

    commitObject(handle);

    return handle;
}

void ClippingModel::invertNormals(bool invert)
{
    auto handle = getHandle();
    setObjectParam(handle, "invertNormals", invert);
    commitObject(handle);
}

OSPGeometricModel ObjectReflector<ClippingModel>::createHandle(OSPDevice device, const Settings &settings)
{
    auto handle = ospNewGeometricModel();
    throwLastDeviceErrorIfNull(device, handle);

    setObjectParam(handle, "geometry", settings.geometry);
    setObjectParam(handle, "invertNormals", settings.invertedNormals);
    setObjectParam(handle, "id", settings.id);

    commitObject(handle);

    return handle;
}
}

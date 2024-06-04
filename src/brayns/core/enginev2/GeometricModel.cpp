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

void setMaterialParam(OSPGeometricModel handle, const PrimitiveMaterials &materials)
{
    std::visit([=](const auto &value) { setObjectParam(handle, "material", value); }, materials);
}

void setColorParam(OSPGeometricModel handle, std::monostate)
{
    removeObjectParam(handle, "color");
}

void setColorParam(OSPGeometricModel handle, const Data1D<Color4> &colors)
{
    setObjectParam(handle, "color", colors);
}

void setColorParam(OSPGeometricModel handle, const Color4 &color)
{
    setObjectParam(handle, "color", color);
}

void setColorParams(OSPGeometricModel handle, const PrimitiveColors &colors)
{
    std::visit([&](const auto &value) { setColorParam(handle, value); }, colors);
}

void setIndicesParam(OSPGeometricModel handle, const MaterialAndColorIndices &indices)
{
    if (!indices)
    {
        removeObjectParam(handle, "index");
        return;
    }
    setObjectParam(handle, "index", *indices);
}
}

namespace brayns::experimental
{
void loadGeometricModelParams(OSPGeometricModel handle, const GeometricModelSettings &settings)
{
    setObjectParam(handle, "geometry", settings.geometry);
    setMaterialParam(handle, settings.materials);
    setColorParams(handle, settings.colors);
    setIndicesParam(handle, settings.indices);
    setObjectParam(handle, "invertNormals", settings.invertedNormals);
    setObjectParam(handle, "id", settings.id);
    commitObject(handle);
}

void GeometricModel::setMaterials(const PrimitiveMaterials &materials)
{
    auto handle = getHandle();
    setMaterialParam(handle, materials);
    commitObject(handle);
}

void GeometricModel::setColors(const PrimitiveColors &colors)
{
    auto handle = getHandle();
    setColorParams(handle, colors);
    commitObject(handle);
}

void GeometricModel::setIndices(const MaterialAndColorIndices &indices)
{
    auto handle = getHandle();
    setIndicesParam(handle, indices);
    commitObject(handle);
}

void GeometricModel::invertNormals(bool invert)
{
    auto handle = getHandle();
    setObjectParam(handle, "invertNormals", invert);
    commitObject(handle);
}

void loadClippingModelParams(OSPGeometricModel handle, const ClippingModelSettings &settings)
{
    setObjectParam(handle, "geometry", settings.geometry);
    setObjectParam(handle, "invertNormals", settings.invertedNormals);
    setObjectParam(handle, "id", settings.id);
    commitObject(handle);
}

void ClippingModel::invertNormals(bool invert)
{
    auto handle = getHandle();
    setObjectParam(handle, "invertNormals", invert);
    commitObject(handle);
}
}

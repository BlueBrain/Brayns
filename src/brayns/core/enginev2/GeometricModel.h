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

#pragma once

#include <optional>
#include <variant>

#include "Data.h"
#include "Geometry.h"
#include "Object.h"

namespace brayns::experimental
{
using RendererIndex = std::uint32_t;
using PrimitiveMaterials = std::variant<RendererIndex, Data1D<RendererIndex>>;
using PrimitiveColors = std::variant<std::monostate, Color4, Data1D<Color4>>;
using UseGeometryColors = std::monostate;
using MaterialAndColorIndices = std::optional<Data1D<std::uint8_t>>;

struct GeometricModelSettings
{
    Geometry geometry;
    PrimitiveMaterials materials = RendererIndex(0);
    PrimitiveColors colors = UseGeometryColors();
    MaterialAndColorIndices indices = std::nullopt;
    bool invertedNormals = false;
    std::uint32_t id = std::uint32_t(-1);
};

void loadGeometricModelParams(OSPGeometricModel handle, const GeometricModelSettings &settings);

class GeometricModel : public Managed<OSPGeometricModel>
{
public:
    using Managed::Managed;

    void setMaterials(const PrimitiveMaterials &materials);
    void setColors(const PrimitiveColors &colors);
    void setIndices(const MaterialAndColorIndices &indices);
    void invertNormals(bool inverted);
};

struct ClippingModelSettings
{
    Geometry geometry;
    bool invertedNormals = false;
    std::uint32_t id = std::uint32_t(-1);
};

void loadClippingModelParams(OSPGeometricModel handle, const ClippingModelSettings &settings);

class ClippingModel : public Managed<OSPGeometricModel>
{
public:
    using Managed::Managed;

    void invertNormals(bool inverted);
};
}

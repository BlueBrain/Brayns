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

#include "Device.h"
#include "Object.h"
#include "Texture.h"

namespace brayns
{
struct MaterialTexture2D
{
    Texture2D value;
    Transform2D transform = {};
};

struct MaterialVolumeTexture
{
    VolumeTexture value;
    Transform transform = {};
};

using MaterialTexture = std::variant<MaterialTexture2D, MaterialVolumeTexture>;

template<OsprayDataType T>
struct MaterialField
{
    T factor;
    std::optional<MaterialTexture> texture = {};
};

class Material : public Managed<OSPMaterial>
{
public:
    using Managed::Managed;
};

struct AoMaterialSettings
{
    MaterialField<Color3> diffuse = {{0.8F, 0.8F, 0.8F}};
    MaterialField<float> opacity = {1.0F};
};

class AoMaterial : public Material
{
public:
    using Material::Material;

    void update(const AoMaterialSettings &settings);
};

AoMaterial createAoMaterial(Device &device, const AoMaterialSettings &settings = {});

struct ScivisMaterialSettings
{
    MaterialField<Color3> diffuse = {{0.8F, 0.8F, 0.8F}};
    MaterialField<float> opacity = {1.0F};
    MaterialField<Color3> specular = {{0.0F, 0.0F, 0.0F}};
    MaterialField<float> shininess{10.0F};
    Color3 transparencyFilter = {0.0F, 0.0F, 0.0F};
};

class ScivisMaterial : public Material
{
public:
    using Material::Material;

    void update(const ScivisMaterialSettings &settings);
};

ScivisMaterial createScivisMaterial(Device &device, const ScivisMaterialSettings &settings = {});

struct PrincipledMaterialSettings
{
    MaterialField<Color3> baseColor = {{0.8F, 0.8F, 0.8F}};
    MaterialField<Color3> edgeColor = {{1.0F, 1.0F, 1.0F}};
    MaterialField<float> metallic = {0.0F};
    MaterialField<float> diffuse = {1.0F};
    MaterialField<float> specular = {1.0F};
    MaterialField<float> ior = {1.0F};
    MaterialField<float> transmission = {0.0F};
    MaterialField<Color3> transmissionColor = {{1.0F, 1.0F, 1.0F}};
    MaterialField<float> transmissionDepth = {1.0F};
    MaterialField<float> roughness = {0.0F};
    MaterialField<float> anisotropy = {0.0F};
    MaterialField<float> rotation = {0.0F};
    MaterialField<float> normal = {1.0F};
    MaterialField<float> baseNormal = {1.0F};
    bool thin = false;
    MaterialField<float> thickness = {1.0F};
    MaterialField<float> backlight = {0.0F};
    MaterialField<float> coat = {0.0F};
    MaterialField<float> coatIor = {1.5F};
    MaterialField<Color3> coatColor = {{1.0F, 1.0F, 1.0F}};
    MaterialField<float> coatThickness = {1.0F};
    MaterialField<float> coatRoughness = {0.0F};
    MaterialField<float> coatNormal = {1.0F};
    MaterialField<float> sheen = {0.0F};
    MaterialField<Color3> sheenColor = {{1.0F, 1.0F, 1.0F}};
    MaterialField<float> sheenTint = {0.0F};
    MaterialField<float> sheenRoughness = {0.2F};
    MaterialField<float> opacity = {1.0F};
    MaterialField<Color3> emissiveColor = {{0.0F, 0.0F, 0.0F}};
};

class PrincipledMaterial : public Material
{
public:
    using Material::Material;

    void update(const PrincipledMaterialSettings &settings);
};

PrincipledMaterial createPrincipledMaterial(Device &device, const PrincipledMaterialSettings &settings = {});
}

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

#include <variant>

#include "Object.h"
#include "Texture.h"

namespace brayns::experimental
{
struct MaterialTexture2D
{
    Texture2D value;
    TextureTransform transform = {};
};

struct MaterialVolumeTexture
{
    VolumeTexture value;
    Transform transform = {};
};

using MaterialTexture = std::variant<std::monostate, MaterialTexture2D, MaterialVolumeTexture>;

template<typename T>
struct MaterialParam
{
    T factor;
    MaterialTexture texture = {};
};

class Material : public Managed<OSPMaterial>
{
public:
    using Managed::Managed;
};

struct AmbientOcclusionMaterialSettings
{
    MaterialParam<Color3> diffuse = {{0.8F, 0.8F, 0.8F}};
    MaterialParam<float> opacity = {1.0F};
};

class AmbientOcclusionMaterial : public Material
{
public:
    using Material::Material;
};

template<>
struct ObjectReflector<AmbientOcclusionMaterial>
{
    using Settings = AmbientOcclusionMaterialSettings;

    static OSPMaterial createHandle(OSPDevice device, const Settings &settings);
};

struct ScivisMaterialSettings
{
    MaterialParam<Color3> diffuse = {{0.8F, 0.8F, 0.8F}};
    MaterialParam<float> opacity = {1.0F};
    MaterialParam<Color3> specular = {{0.0F, 0.0F, 0.0F}};
    MaterialParam<float> shininess = {10.0F};
    Color3 transparencyFilter = {0.0F, 0.0F, 0.0F};
};

class ScivisMaterial : public Material
{
public:
    using Material::Material;
};

template<>
struct ObjectReflector<ScivisMaterial>
{
    using Settings = ScivisMaterialSettings;

    static OSPMaterial createHandle(OSPDevice device, const Settings &settings);
};

struct PrincipledMaterialSettings
{
    MaterialParam<Color3> baseColor = {{0.8F, 0.8F, 0.8F}};
    MaterialParam<MaterialTexture> baseColorMap = {{}};
    MaterialParam<Color3> edgeColor = {{1.0F, 1.0F, 1.0F}};
    MaterialParam<float> metallic = {0.0F};
    MaterialParam<float> diffuse = {1.0F};
    MaterialParam<float> specular = {1.0F};
    MaterialParam<float> ior = {1.0F};
    MaterialParam<float> transmission = {0.0F};
    MaterialParam<Color3> transmissionColor = {{1.0F, 1.0F, 1.0F}};
    MaterialParam<float> transmissionDepth = {1.0F};
    MaterialParam<float> roughness = {0.0F};
    MaterialParam<float> anisotropy = {0.0F};
    MaterialParam<float> rotation = {0.0F};
    MaterialParam<float> normal = {1.0F};
    MaterialParam<MaterialTexture> normalMap = {{}};
    MaterialParam<float> baseNormal = {1.0F};
    MaterialParam<bool> thin = {false};
    MaterialParam<float> thickness = {1.0F};
    MaterialParam<float> backlight = {0.0F};
    MaterialParam<float> coat = {0.0F};
    MaterialParam<float> coatIor = {1.5F};
    MaterialParam<Color3> coatColor = {{1.0F, 1.0F, 1.0F}};
    MaterialParam<float> coatThickness = {1.0F};
    MaterialParam<float> coatRoughness = {0.0F};
    MaterialParam<float> coatNormal = {1.0F};
    MaterialParam<float> sheen = {0.0F};
    MaterialParam<Color3> sheenColor = {{1.0F, 1.0F, 1.0F}};
    MaterialParam<float> sheenTint = {0.0F};
    MaterialParam<float> sheenRoughness = {0.2F};
    MaterialParam<float> opacity = {1.0F};
    MaterialParam<Color3> emissiveColor = {{0.0F, 0.0F, 0.0F}};
};

class PrincipledMaterial : public Material
{
public:
    using Material::Material;
};

template<>
struct ObjectReflector<PrincipledMaterial>
{
    using Settings = PrincipledMaterialSettings;

    static OSPMaterial createHandle(OSPDevice device, const Settings &settings);
};
}

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

class Material : public Managed<OSPMaterial>
{
public:
    using Managed::Managed;
};

struct AmbientOcclusionMaterialSettings
{
    Color3 diffuse = {0.8F, 0.8F, 0.8F};
    MaterialTexture diffuseMap = {};
    float opacity = 1.0F;
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

    static inline const std::string name = "obj";

    static void loadParams(OSPMaterial handle, const Settings &settings);
};

struct ScivisMaterialSettings : AmbientOcclusionMaterialSettings
{
    Color3 specular = {0.0F, 0.0F, 0.0F};
    float shininess = 10.0F;
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

    static inline const std::string name = "obj";

    static void loadParams(OSPMaterial handle, const Settings &settings);
};

struct PrincipledMaterialSettings
{
    Color3 baseColor = {0.8F, 0.8F, 0.8F};
    MaterialTexture baseColorMap = {};
    Color3 edgeColor = {1.0F, 1.0F, 1.0F};
    float metallic = 0.0F;
    float diffuse = 1.0F;
    float specular = 1.0F;
    float ior = 1.0F;
    float transmission = 0.0F;
    Color3 transmissionColor = {1.0F, 1.0F, 1.0F};
    float transmissionDepth = 1.0F;
    float roughness = 0.0F;
    float anisotropy = 0.0F;
    float rotation = 0.0F;
    float normal = 1.0F;
    MaterialTexture normalMap = {};
    float baseNormal = 1.0F;
    bool thin = false;
    float thickness = 1.0F;
    float backlight = 0.0F;
    float coat = 0.0F;
    float coatIor = 1.5F;
    Color3 coatColor = {1.0F, 1.0F, 1.0F};
    float coatThickness = 1.0F;
    float coatRoughness = 0.0F;
    float coatNormal = 1.0F;
    float sheen = 0.0F;
    Color3 sheenColor = {1.0F, 1.0F, 1.0F};
    float sheenTint = 0.0F;
    float sheenRoughness = 0.2F;
    float opacity = 1.0F;
    Color3 emissiveColor = {0.0F, 0.0F, 0.0F};
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

    static inline const std::string name = "principled";

    static void loadParams(OSPMaterial handle, const Settings &settings);
};
}

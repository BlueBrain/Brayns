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
template<typename T, OsprayDataType U>
struct MaterialTexture2DOf
{
    T value;
    Transform2D transform = {};
    std::optional<U> factor = {};
};

template<OsprayDataType T>
using MaterialTexture2D = MaterialTexture2DOf<Texture2D, T>;

template<typename T, OsprayDataType U>
struct MaterialVolumeTextureOf
{
    T value;
    Transform transform = {};
    std::optional<U> factor = {};
};

template<OsprayDataType T>
using MaterialVolumeTexture = MaterialVolumeTextureOf<VolumeTexture, T>;

template<OsprayDataType T, typename U, typename V = U>
using MaterialFieldOf = std::variant<T, MaterialTexture2DOf<U, T>, MaterialVolumeTextureOf<V, T>>;

template<OsprayDataType T>
using MaterialField = MaterialFieldOf<T, Texture2D, VolumeTexture>;

class Material : public Managed<OSPMaterial>
{
public:
    using Managed::Managed;
};

template<template<typename> typename Field>
struct AoMaterialSettingsOf
{
    Field<Color3> diffuse = Color3{0.8F, 0.8F, 0.8F};
    Field<float> opacity = 1.0F;
};

using AoMaterialSettings = AoMaterialSettingsOf<MaterialField>;

template<typename T, template<typename> typename U>
T converMaterialSettingsTo(const AoMaterialSettingsOf<U> &settings, auto &&mapper)
{
    return {
        .diffuse = mapper(settings.diffuse),
        .opacity = mapper(settings.opacity),
    };
}

class AoMaterial : public Material
{
public:
    using Material::Material;

    void update(const AoMaterialSettings &settings);
};

AoMaterial createAoMaterial(Device &device, const AoMaterialSettings &settings = {});

template<template<typename> typename Field>
struct ScivisMaterialSettingsOf
{
    Field<Color3> diffuse = Color3{0.8F, 0.8F, 0.8F};
    Field<float> opacity = 1.0F;
    Field<Color3> specular = Color3{0.0F, 0.0F, 0.0F};
    Field<float> shininess = 10.0F;
    Color3 transparencyFilter = {0.0F, 0.0F, 0.0F};
};

using ScivisMaterialSettings = ScivisMaterialSettingsOf<MaterialField>;

template<typename T, template<typename> typename U>
T converMaterialSettingsTo(const ScivisMaterialSettingsOf<U> &settings, auto &&mapper)
{
    return {
        .diffuse = mapper(settings.diffuse),
        .opacity = mapper(settings.opacity),
        .specular = mapper(settings.specular),
        .shininess = mapper(settings.shininess),
        .transparencyFilter = settings.transparencyFilter,
    };
}

class ScivisMaterial : public Material
{
public:
    using Material::Material;

    void update(const ScivisMaterialSettings &settings);
};

ScivisMaterial createScivisMaterial(Device &device, const ScivisMaterialSettings &settings = {});

template<template<typename> typename Field>
struct PrincipledMaterialSettingsOf
{
    Field<Color3> baseColor = Color3{0.8F, 0.8F, 0.8F};
    Field<Color3> edgeColor = Color3{1.0F, 1.0F, 1.0F};
    Field<float> metallic = 0.0F;
    Field<float> diffuse = 1.0F;
    Field<float> specular = 1.0F;
    Field<float> ior = 1.0F;
    Field<float> transmission = 0.0F;
    Field<Color3> transmissionColor = Color3{1.0F, 1.0F, 1.0F};
    Field<float> transmissionDepth = 1.0F;
    Field<float> roughness = 0.0F;
    Field<float> anisotropy = 0.0F;
    Field<float> rotation = 0.0F;
    Field<float> normal = 1.0F;
    Field<float> baseNormal = 1.0F;
    bool thin = false;
    Field<float> thickness = 1.0F;
    Field<float> backlight = 0.0F;
    Field<float> coat = 0.0F;
    Field<float> coatIor = 1.5F;
    Field<Color3> coatColor = Color3{1.0F, 1.0F, 1.0F};
    Field<float> coatThickness = 1.0F;
    Field<float> coatRoughness = 0.0F;
    Field<float> coatNormal = 1.0F;
    Field<float> sheen = 0.0F;
    Field<Color3> sheenColor = Color3{1.0F, 1.0F, 1.0F};
    Field<float> sheenTint = 0.0F;
    Field<float> sheenRoughness = 0.2F;
    Field<float> opacity = 1.0F;
    Field<Color3> emissiveColor = Color3{0.0F, 0.0F, 0.0F};
};

using PrincipledMaterialSettings = PrincipledMaterialSettingsOf<MaterialField>;

template<typename T, template<typename> typename U>
T converMaterialSettingsTo(const PrincipledMaterialSettingsOf<U> &settings, auto &&mapper)
{
    return {
        .baseColor = mapper(settings.baseColor),
        .edgeColor = mapper(settings.edgeColor),
        .metallic = mapper(settings.metallic),
        .diffuse = mapper(settings.diffuse),
        .specular = mapper(settings.specular),
        .ior = mapper(settings.ior),
        .transmission = mapper(settings.transmission),
        .transmissionColor = mapper(settings.transmissionColor),
        .transmissionDepth = mapper(settings.transmissionDepth),
        .roughness = mapper(settings.roughness),
        .anisotropy = mapper(settings.anisotropy),
        .rotation = mapper(settings.rotation),
        .normal = mapper(settings.normal),
        .baseNormal = mapper(settings.baseNormal),
        .thin = settings.thin,
        .thickness = mapper(settings.thickness),
        .backlight = mapper(settings.backlight),
        .coat = mapper(settings.coat),
        .coatIor = mapper(settings.coatIor),
        .coatColor = mapper(settings.coatColor),
        .coatThickness = mapper(settings.coatThickness),
        .coatRoughness = mapper(settings.coatRoughness),
        .coatNormal = mapper(settings.coatNormal),
        .sheen = mapper(settings.sheen),
        .sheenColor = mapper(settings.sheenColor),
        .sheenTint = mapper(settings.sheenTint),
        .sheenRoughness = mapper(settings.sheenRoughness),
        .opacity = mapper(settings.opacity),
        .emissiveColor = mapper(settings.emissiveColor),
    };
}

class PrincipledMaterial : public Material
{
public:
    using Material::Material;

    void update(const PrincipledMaterialSettings &settings);
};

PrincipledMaterial createPrincipledMaterial(Device &device, const PrincipledMaterialSettings &settings = {});
}

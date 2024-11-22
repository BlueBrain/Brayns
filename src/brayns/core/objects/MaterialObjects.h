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

#include <any>
#include <concepts>
#include <functional>

#include <brayns/core/engine/Material.h>
#include <brayns/core/manager/ObjectManager.h>

#include "TextureObjects.h"

namespace brayns
{
struct UserMaterial
{
    std::any value;
    std::function<Material()> get;
};

template<ReflectedJsonObject Settings, std::derived_from<Material> T>
struct UserMaterialOf
{
    Settings settings;
    T value;
    std::vector<Stored<UserTexture>> textures;
};

struct MaterialTexture2DParams
{
    ObjectId texture2d;
    Transform2D transform;
};

template<>
struct JsonObjectReflector<MaterialTexture2DParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<MaterialTexture2DParams>();
        builder.field("texture2d", [](auto &object) { return &object.texture2d; }).description("ID of the texture 2D to use for the material field");
        builder.extend([](auto &object) { return &object.transform; });
        return builder.build();
    }
};

struct MaterialVolumeTextureParams
{
    ObjectId volumeTexture;
    Transform transform;
};

template<>
struct JsonObjectReflector<MaterialVolumeTextureParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<MaterialVolumeTextureParams>();
        builder.field("volumeTexture", [](auto &object) { return &object.volumeTexture; })
            .description("ID of the volume texture to use for the material field");
        builder.extend([](auto &object) { return &object.transform; });
        return builder.build();
    }
};

using MaterialTextureParams = std::variant<MaterialTexture2DParams, MaterialVolumeTextureParams>;

template<OsprayDataType T>
struct TexturedMaterialParams
{
    T factor;
    std::optional<MaterialTextureParams> texture;
};

template<OsprayDataType T>
struct JsonObjectReflector<TexturedMaterialParams<T>>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<TexturedMaterialParams<T>>();
        builder.field("factor", [](auto &object) { return &object.factor; })
            .description("Material field value or factor depending if a texture is applied");
        builder.field("texture", [](auto &object) { return &object.texture; })
            .description("Optional material field texture, pre-multiplied by factor");
        return builder.build();
    }
};

template<OsprayDataType T>
using MaterialParams = std::variant<T, TexturedMaterialParams<T>>;

struct AoMaterialParams
{
    MaterialParams<Color3> diffuse;
    MaterialParams<float> opacity;
};

template<>
struct JsonObjectReflector<AoMaterialParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<AoMaterialParams>();
        builder.field("diffuse", [](auto &object) { return &object.diffuse; }).description("Diffuse color").defaultValue(Color3{0.8F, 0.8F, 0.8F});
        builder.field("opacity", [](auto &object) { return &object.opacity; }).description("Opacity").defaultValue(1.0F);
        return builder.build();
    }
};

using CreateAoMaterialParams = CreateParamsOf<AoMaterialParams>;
using GetAoMaterialResult = GetResultOf<AoMaterialParams>;
using UpdateAoMaterialParams = UpdateParamsOf<AoMaterialParams>;
using UserAoMaterial = UserMaterialOf<AoMaterialParams, AoMaterial>;

CreateObjectResult createAoMaterial(ObjectManager &objects, Device &device, const CreateAoMaterialParams &params);
GetAoMaterialResult getAoMaterial(ObjectManager &objects, const GetObjectParams &params);
void updateAoMaterial(ObjectManager &objects, Device &device, const UpdateAoMaterialParams &params);

struct ScivisMaterialParams
{
    MaterialParams<Color3> diffuse;
    MaterialParams<float> opacity;
    MaterialParams<Color3> specular;
    MaterialParams<float> shininess;
    Color3 transparencyFilter;
};

template<>
struct JsonObjectReflector<ScivisMaterialParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<ScivisMaterialParams>();
        builder.field("diffuse", [](auto &object) { return &object.diffuse; }).description("Diffuse color").defaultValue(Color3{0.8F, 0.8F, 0.8F});
        builder.field("opacity", [](auto &object) { return &object.opacity; }).description("Opacity").defaultValue(1.0F);
        builder.field("specular", [](auto &object) { return &object.specular; }).description("Specular color").defaultValue(Color3{0.0F, 0.0F, 0.0F});
        builder.field("shininess", [](auto &object) { return &object.shininess; }).description("Phong exponent").defaultValue(10);
        builder.field("transparencyFilter", [](auto &object) { return &object.transparencyFilter; })
            .description("Transparency filter")
            .defaultValue(Color3{0.0F, 0.0F, 0.0F});
        return builder.build();
    }
};

using CreateScivisMaterialParams = CreateParamsOf<ScivisMaterialParams>;
using GetScivisMaterialResult = GetResultOf<ScivisMaterialParams>;
using UpdateScivisMaterialParams = UpdateParamsOf<ScivisMaterialParams>;
using UserScivisMaterial = UserMaterialOf<ScivisMaterialParams, ScivisMaterial>;

CreateObjectResult createScivisMaterial(ObjectManager &objects, Device &device, const CreateScivisMaterialParams &params);
GetScivisMaterialResult getScivisMaterial(ObjectManager &objects, const GetObjectParams &params);
void updateScivisMaterial(ObjectManager &objects, Device &device, const UpdateScivisMaterialParams &params);

struct PrincipledMaterialParams
{
    MaterialParams<Color3> baseColor;
    MaterialParams<Color3> edgeColor;
    MaterialParams<float> metallic;
    MaterialParams<float> diffuse;
    MaterialParams<float> specular;
    MaterialParams<float> ior;
    MaterialParams<float> transmission;
    MaterialParams<Color3> transmissionColor;
    MaterialParams<float> transmissionDepth;
    MaterialParams<float> roughness;
    MaterialParams<float> anisotropy;
    MaterialParams<float> rotation;
    MaterialParams<float> normal;
    MaterialParams<float> baseNormal;
    bool thin;
    MaterialParams<float> thickness;
    MaterialParams<float> backlight;
    MaterialParams<float> coat;
    MaterialParams<float> coatIor;
    MaterialParams<Color3> coatColor;
    MaterialParams<float> coatThickness;
    MaterialParams<float> coatRoughness;
    MaterialParams<float> coatNormal;
    MaterialParams<float> sheen;
    MaterialParams<Color3> sheenColor;
    MaterialParams<float> sheenTint;
    MaterialParams<float> sheenRoughness;
    MaterialParams<float> opacity;
    MaterialParams<Color3> emissiveColor;
};

template<>
struct JsonObjectReflector<PrincipledMaterialParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<PrincipledMaterialParams>();
        builder.field("baseColor", [](auto &object) { return &object.baseColor; })
            .description("Base color, linear RGB")
            .defaultValue(Color3{0.8F, 0.8F, 0.8F});
        builder.field("edgeColor", [](auto &object) { return &object.edgeColor; })
            .description("Edge color, linear RGB, metallic only")
            .defaultValue(Color3{1.0F, 1.0F, 1.0F});
        builder.field("metallic", [](auto &object) { return &object.metallic; })
            .description("Mix between dielectric and metallic (1 = full metal)")
            .defaultValue(0.0F);
        builder.field("diffuse", [](auto &object) { return &object.diffuse; }).description("Diffuse reflection weight").defaultValue(1.0F);
        builder.field("specular", [](auto &object) { return &object.specular; }).description("Specular reflection weight").defaultValue(1.0F);
        builder.field("ior", [](auto &object) { return &object.ior; }).description("Index of refraction").defaultValue(1.0F);
        builder.field("transmission", [](auto &object) { return &object.transmission; })
            .description("Specular transmission weight")
            .defaultValue(0.0F);
        builder.field("transmissionColor", [](auto &object) { return &object.transmissionColor; })
            .description("Attenuated color due to transmission")
            .defaultValue(Color3{1.0F, 1.0F, 1.0F});
        builder.field("transmissionDepth", [](auto &object) { return &object.transmissionDepth; })
            .description("Distance at which color attenuation is transmissionColor")
            .defaultValue(1.0F);
        builder.field("roughness", [](auto &object) { return &object.roughness; })
            .description("Diffuse and specular roughness (0 = smooth)")
            .defaultValue(0.0F);
        builder.field("anisotropy", [](auto &object) { return &object.anisotropy; }).description("Specular anisotropy").defaultValue(0.0F);
        builder.field("rotation", [](auto &object) { return &object.rotation; })
            .description("Rotation of anisotropy direction (1 = full circle)")
            .defaultValue(0.0F);
        builder.field("normal", [](auto &object) { return &object.normal; }).description("Normal map").defaultValue(1.0F);
        builder.field("baseNormal", [](auto &object) { return &object.baseNormal; }).description("Overrides default normal").defaultValue(1.0F);
        builder.field("thin", [](auto &object) { return &object.thin; })
            .description("")
            .description("Wether the material is thin or solid")
            .defaultValue(false);
        builder.field("thickness", [](auto &object) { return &object.thickness; }).description("Thickness of thin material").defaultValue(1.0F);
        builder.field("backlight", [](auto &object) { return &object.backlight; })
            .description("Diffuse transmission of thin material, 0 = reflection, 1 = 50/50, 2 = transmission")
            .defaultValue(0.0F);
        builder.field("coat", [](auto &object) { return &object.coat; }).description("Coat layer weight").defaultValue(0.0F);
        builder.field("coatIor", [](auto &object) { return &object.coatIor; }).description("Coat layer IOR").defaultValue(1.5F);
        builder.field("coatColor", [](auto &object) { return &object.coatColor; })
            .description("Coat layer color")
            .defaultValue(Color3{1.0F, 1.0F, 1.0F});
        builder.field("coatThickness", [](auto &object) { return &object.coatThickness; }).description("Coat layer thickness").defaultValue(1.0F);
        builder.field("coatRoughness", [](auto &object) { return &object.coatRoughness; }).description("Coat layer roughness").defaultValue(0.0F);
        builder.field("coatNormal", [](auto &object) { return &object.coatNormal; }).description("Coat layer normal map").defaultValue(1.0F);
        builder.field("sheen", [](auto &object) { return &object.sheen; }).description("Sheen layer weight").defaultValue(0.0F);
        builder.field("sheenColor", [](auto &object) { return &object.sheenColor; })
            .description("Sheen layer color")
            .defaultValue(Color3{1.0F, 1.0F, 1.0F});
        builder.field("sheenTint", [](auto &object) { return &object.sheenTint; }).description("Sheen layer color").defaultValue(0.0F);
        builder.field("sheenRoughness", [](auto &object) { return &object.sheenRoughness; }).description("Sheen layer roughness").defaultValue(0.2F);
        builder.field("opacity", [](auto &object) { return &object.opacity; }).description("Cut-out opacity").defaultValue(1.0F);
        builder.field("emissiveColor", [](auto &object) { return &object.emissiveColor; })
            .description("Color and intensity of light emission")
            .defaultValue(Color3{0.0F, 0.0F, 0.0F});
        return builder.build();
    }
};

using CreatePrincipledMaterialParams = CreateParamsOf<PrincipledMaterialParams>;
using GetPrincipledMaterialResult = GetResultOf<PrincipledMaterialParams>;
using UpdatePrincipledMaterialParams = UpdateParamsOf<PrincipledMaterialParams>;
using UserPrincipledMaterial = UserMaterialOf<PrincipledMaterialParams, PrincipledMaterial>;

CreateObjectResult createPrincipledMaterial(ObjectManager &objects, Device &device, const CreatePrincipledMaterialParams &params);
GetPrincipledMaterialResult getPrincipledMaterial(ObjectManager &objects, const GetObjectParams &params);
void updatePrincipledMaterial(ObjectManager &objects, Device &device, const UpdatePrincipledMaterialParams &params);
}

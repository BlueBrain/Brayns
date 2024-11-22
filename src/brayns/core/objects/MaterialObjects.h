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

CreateObjectResult createAoMaterial(ObjectManager &objects, Device &device, CreateAoMaterialParams params);
GetAoMaterialResult getAoMaterial(ObjectManager &objects, const GetObjectParams &params);
void updateAoMaterial(ObjectManager &objects, Device &device, const UpdateAoMaterialParams &params);
}

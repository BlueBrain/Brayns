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

#include "MaterialObjects.h"

namespace
{
using namespace brayns;

MaterialTexture retreiveTexture(ObjectManager &objects, std::vector<Stored<UserTexture>> &textures, const MaterialTexture2DParams &params)
{
    auto object = objects.getAsStored<UserTexture>(params.texture2d);
    auto &texture = *castAsShared<UserTexture2D>(object.get().value, object);

    textures.push_back(std::move(object));

    return MaterialTexture2D{texture.value, params.transform};
}

MaterialTexture retreiveTexture(ObjectManager &objects, std::vector<Stored<UserTexture>> &textures, const MaterialVolumeTextureParams &params)
{
    auto object = objects.getAsStored<UserTexture>(params.volumeTexture);
    auto &texture = *castAsShared<UserVolumeTexture>(object.get().value, object);

    textures.push_back(std::move(object));

    return MaterialVolumeTexture{texture.value, params.transform};
}

MaterialTexture retreiveTexture(ObjectManager &objects, std::vector<Stored<UserTexture>> &textures, const MaterialTextureParams &params)
{
    return std::visit([&](const auto &value) { return retreiveTexture(objects, textures, value); }, params);
}

template<OsprayDataType T>
MaterialField<T> storeAndGet(ObjectManager &objects, std::vector<Stored<UserTexture>> &textures, const TexturedMaterialParams<T> &params)
{
    if (!params.texture)
    {
        return {params.factor};
    }

    auto texture = retreiveTexture(objects, textures, *params.texture);

    return {params.factor, std::move(texture)};
}

template<OsprayDataType T>
MaterialField<T> storeAndGet(ObjectManager &objects, std::vector<Stored<UserTexture>> &textures, const T &params)
{
    (void)objects;
    (void)textures;
    return {params};
}

template<OsprayDataType T>
MaterialField<T> storeAndGet(ObjectManager &objects, std::vector<Stored<UserTexture>> &textures, const MaterialParams<T> &params)
{
    return std::visit([&](const auto &value) { return storeAndGet(objects, textures, value); }, params);
}

AoMaterialSettings parseSettings(ObjectManager &objects, std::vector<Stored<UserTexture>> &textures, const AoMaterialParams &params)
{
    return {
        .diffuse = storeAndGet(objects, textures, params.diffuse),
        .opacity = storeAndGet(objects, textures, params.opacity),
    };
}

ScivisMaterialSettings parseSettings(ObjectManager &objects, std::vector<Stored<UserTexture>> &textures, const ScivisMaterialParams &params)
{
    return {
        .diffuse = storeAndGet(objects, textures, params.diffuse),
        .opacity = storeAndGet(objects, textures, params.opacity),
        .specular = storeAndGet(objects, textures, params.specular),
        .shininess = storeAndGet(objects, textures, params.shininess),
        .transparencyFilter = params.transparencyFilter,
    };
}

PrincipledMaterialSettings parseSettings(ObjectManager &objects, std::vector<Stored<UserTexture>> &textures, const PrincipledMaterialParams &params)
{
    return {
        .baseColor = storeAndGet(objects, textures, params.baseColor),
        .edgeColor = storeAndGet(objects, textures, params.edgeColor),
        .metallic = storeAndGet(objects, textures, params.metallic),
        .diffuse = storeAndGet(objects, textures, params.diffuse),
        .specular = storeAndGet(objects, textures, params.specular),
        .ior = storeAndGet(objects, textures, params.ior),
        .transmission = storeAndGet(objects, textures, params.transmission),
        .transmissionColor = storeAndGet(objects, textures, params.transmissionColor),
        .transmissionDepth = storeAndGet(objects, textures, params.transmissionDepth),
        .roughness = storeAndGet(objects, textures, params.roughness),
        .anisotropy = storeAndGet(objects, textures, params.anisotropy),
        .rotation = storeAndGet(objects, textures, params.rotation),
        .normal = storeAndGet(objects, textures, params.normal),
        .baseNormal = storeAndGet(objects, textures, params.baseNormal),
        .thin = params.thin,
        .thickness = storeAndGet(objects, textures, params.thickness),
        .backlight = storeAndGet(objects, textures, params.backlight),
        .coat = storeAndGet(objects, textures, params.coat),
        .coatIor = storeAndGet(objects, textures, params.coatIor),
        .coatColor = storeAndGet(objects, textures, params.coatColor),
        .coatThickness = storeAndGet(objects, textures, params.coatThickness),
        .coatRoughness = storeAndGet(objects, textures, params.coatRoughness),
        .coatNormal = storeAndGet(objects, textures, params.coatNormal),
        .sheen = storeAndGet(objects, textures, params.sheen),
        .sheenColor = storeAndGet(objects, textures, params.sheenColor),
        .sheenTint = storeAndGet(objects, textures, params.sheenTint),
        .sheenRoughness = storeAndGet(objects, textures, params.sheenRoughness),
        .opacity = storeAndGet(objects, textures, params.opacity),
        .emissiveColor = storeAndGet(objects, textures, params.emissiveColor),
    };
}

template<typename T>
CreateObjectResult createMaterialAs(ObjectManager &objects, Device &device, const auto &params, auto create, std::string name)
{
    const auto &[base, derived] = params;

    auto textures = std::vector<Stored<UserTexture>>();

    auto settings = parseSettings(objects, textures, derived);

    auto material = create(device, settings);

    auto ptr = toShared(T{derived, std::move(material), std::move(textures)});

    auto object = UserMaterial{
        .value = ptr,
        .get = [=] { return ptr->value; },
    };

    auto stored = objects.add(std::move(object), {std::move(name)}, std::move(base));

    return getResult(stored);
}

template<typename T>
auto getMaterialAs(ObjectManager &objects, const GetObjectParams &params)
{
    auto stored = objects.getAsStored<UserMaterial>(params.id);
    auto &material = *castAsShared<T>(stored.get().value, stored);
    return getResult(material.settings);
}

template<typename T>
void updateMaterialAs(ObjectManager &objects, Device &device, const auto &params)
{
    auto stored = objects.getAsStored<UserMaterial>(params.id);
    auto &material = *castAsShared<T>(stored.get().value, stored);

    auto updated = getUpdatedParams(params, material.settings);

    auto textures = std::vector<Stored<UserTexture>>();

    auto settings = parseSettings(objects, textures, updated);

    material.value.update(settings);
    device.throwIfError();

    material.settings = std::move(updated);
}
}

namespace brayns
{
CreateObjectResult createAoMaterial(ObjectManager &objects, Device &device, const CreateAoMaterialParams &params)
{
    auto create = [](auto &device, const auto &settings) { return createAoMaterial(device, settings); };
    return createMaterialAs<UserAoMaterial>(objects, device, params, create, "AoMaterial");
}

GetAoMaterialResult getAoMaterial(ObjectManager &objects, const GetObjectParams &params)
{
    return getMaterialAs<UserAoMaterial>(objects, params);
}

void updateAoMaterial(ObjectManager &objects, Device &device, const UpdateAoMaterialParams &params)
{
    updateMaterialAs<UserAoMaterial>(objects, device, params);
}

CreateObjectResult createScivisMaterial(ObjectManager &objects, Device &device, const CreateScivisMaterialParams &params)
{
    auto create = [](auto &device, const auto &settings) { return createScivisMaterial(device, settings); };
    return createMaterialAs<UserScivisMaterial>(objects, device, params, create, "ScivisMaterial");
}

GetScivisMaterialResult getScivisMaterial(ObjectManager &objects, const GetObjectParams &params)
{
    return getMaterialAs<UserScivisMaterial>(objects, params);
}

void updateScivisMaterial(ObjectManager &objects, Device &device, const UpdateScivisMaterialParams &params)
{
    updateMaterialAs<UserScivisMaterial>(objects, device, params);
}

CreateObjectResult createPrincipledMaterial(ObjectManager &objects, Device &device, const CreatePrincipledMaterialParams &params)
{
    auto create = [](auto &device, const auto &settings) { return createPrincipledMaterial(device, settings); };
    return createMaterialAs<UserPrincipledMaterial>(objects, device, params, create, "PrincipledMaterial");
}

GetPrincipledMaterialResult getPrincipledMaterial(ObjectManager &objects, const GetObjectParams &params)
{
    return getMaterialAs<UserPrincipledMaterial>(objects, params);
}

void updatePrincipledMaterial(ObjectManager &objects, Device &device, const UpdatePrincipledMaterialParams &params)
{
    updateMaterialAs<UserPrincipledMaterial>(objects, device, params);
}
}

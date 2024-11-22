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

#include "common/Objects.h"

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

AoMaterialSettings storeTexturesAndGetSettings(ObjectManager &objects, std::vector<Stored<UserTexture>> &textures, const AoMaterialParams &params)
{
    return {
        .diffuse = storeAndGet(objects, textures, params.diffuse),
        .opacity = storeAndGet(objects, textures, params.opacity),
    };
}
}

namespace brayns
{
CreateObjectResult createAoMaterial(ObjectManager &objects, Device &device, CreateAoMaterialParams params)
{
    auto &[base, derived] = params;

    auto textures = std::vector<Stored<UserTexture>>();

    auto settings = storeTexturesAndGetSettings(objects, textures, derived);

    auto material = createAoMaterial(device, settings);

    auto ptr = toShared(UserAoMaterial{std::move(derived), std::move(material), std::move(textures)});

    auto object = UserMaterial{
        .value = ptr,
        .get = [=] { return ptr->value; },
    };

    auto stored = objects.add(std::move(object), {"AoMaterial"}, std::move(base));

    return getResult(stored);
}

GetAoMaterialResult getAoMaterial(ObjectManager &objects, const GetObjectParams &params)
{
    auto stored = objects.getAsStored<UserMaterial>(params.id);
    auto &material = *castAsShared<UserAoMaterial>(stored.get().value, stored);
    return getResult(material.settings);
}

void updateAoMaterial(ObjectManager &objects, Device &device, const UpdateAoMaterialParams &params)
{
    auto stored = objects.getAsStored<UserMaterial>(params.id);
    auto &material = *castAsShared<UserAoMaterial>(stored.get().value, stored);

    auto updated = getUpdatedParams(params, material.settings);

    auto textures = std::vector<Stored<UserTexture>>();

    auto settings = storeTexturesAndGetSettings(objects, textures, updated);

    material.value.update(settings);
    device.throwIfError();

    material.settings = std::move(updated);
}
}

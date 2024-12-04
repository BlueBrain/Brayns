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

#include "TextureObjects.h"

#include "common/Binary.h"
#include "common/Objects.h"

namespace
{
using namespace brayns;

VolumeTextureStorage createVolumeTextureStorage(ObjectManager &objects, const VolumeTextureParams &params)
{
    auto volume = objects.getAsStored<UserVolume>(params.volume);
    auto transferFunction = objects.getAsStored<UserTransferFunction>(params.transferFunction);
    return {std::move(volume), std::move(transferFunction)};
}

VolumeTextureSettings createVolumeTextureSettings(const VolumeTextureStorage &storage)
{
    return {storage.volume.get().get(), storage.transferFunction.get().get()};
}

VolumeTextureParams createVolumeTextureParams(const VolumeTextureStorage &storage)
{
    auto &[volume, transferFunction] = storage;
    return {volume.getId(), transferFunction.getId()};
}
}

namespace brayns
{
CreateObjectResult createTexture2D(ObjectManager &objects, Device &device, CreateTexture2DParams params)
{
    auto &[json, binary] = params;
    auto &[base, derived] = json;

    auto texelSize = getSize(derived.format);
    auto texelCount = reduceMultiply(derived.size);

    sanitizeBinary(binary, {texelSize, texelCount});

    auto data = TextureData2D{std::move(binary), derived.format, derived.size};
    auto texture = createTexture2D(device, data, derived.value);

    auto storage = Texture2DStorage{std::move(data), std::move(derived.value)};

    auto ptr = toShared(UserTexture2D{std::move(storage), std::move(texture)});

    auto object = UserTexture{
        .value = ptr,
        .get = [=] { return ptr->value; },
    };

    auto stored = objects.add(std::move(object), {"Texture2D"}, base);

    return getResult(stored);
}

GetTexture2DResult getTexture2D(ObjectManager &objects, const GetObjectParams &params)
{
    auto object = objects.getAsStored<UserTexture>(params.id);
    auto &texture = castTextureAs<UserTexture2D>(object);
    auto &storage = texture.storage;

    auto result = Texture2DParams{storage.data.format, storage.data.size, storage.settings};

    return getResult(result);
}

void updateTexture2D(ObjectManager &objects, Device &device, const UpdateTexture2DParams &params)
{
    auto stored = objects.getAsStored<UserTexture>(params.id);
    auto &texture = castTextureAs<UserTexture2D>(stored);

    auto settings = getUpdatedParams(params, texture.storage.settings);

    texture.value.update(settings);
    device.throwIfError();

    texture.storage.settings = settings;
}

CreateObjectResult createVolumeTexture(ObjectManager &objects, Device &device, const CreateVolumeTextureParams &params)
{
    const auto &[base, derived] = params;

    auto storage = createVolumeTextureStorage(objects, derived);
    auto settings = createVolumeTextureSettings(storage);

    auto texture = createVolumeTexture(device, settings);

    auto ptr = toShared(UserVolumeTexture{std::move(storage), std::move(texture)});

    auto object = UserTexture{
        .value = ptr,
        .get = [=] { return ptr->value; },
    };

    auto stored = objects.add(std::move(object), {"Texture2D"}, base);

    return getResult(stored);
}

GetVolumeTextureResult getVolumeTexture(ObjectManager &objects, const GetObjectParams &params)
{
    auto stored = objects.getAsStored<UserTexture>(params.id);
    auto &texture = *castAsShared<UserVolumeTexture>(stored.get().value, stored);
    auto &[volume, transferFunction] = texture.storage;

    auto result = VolumeTextureParams{volume.getId(), transferFunction.getId()};

    return getResult(result);
}

void updateVolumeTexture(ObjectManager &objects, Device &device, const UpdateVolumeTextureParams &params)
{
    auto stored = objects.getAsStored<UserTexture>(params.id);
    auto &texture = *castAsShared<UserVolumeTexture>(stored.get().value, stored);

    auto current = createVolumeTextureParams(texture.storage);
    auto updated = getUpdatedParams(params, current);

    auto storage = createVolumeTextureStorage(objects, updated);
    auto settings = createVolumeTextureSettings(storage);

    texture.value.update(settings);
    device.throwIfError();

    texture.storage = std::move(storage);
}
}

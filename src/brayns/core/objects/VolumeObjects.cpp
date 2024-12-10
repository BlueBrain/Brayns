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

#include "VolumeObjects.h"

#include "common/Binary.h"

namespace brayns
{
CreateObjectResult createRegularVolume(ObjectManager &objects, Device &device, CreateRegularVolumeParams params)
{
    auto &[json, binary] = params;
    auto &[base, derived] = json;

    auto voxelSize = getSize(derived.voxelType);
    auto voxelCount = reduceMultiply(derived.size);

    sanitizeBinary(binary, {voxelSize, voxelCount});

    auto data = RegularVolumeData{std::move(binary), derived.voxelType, derived.size};
    auto volume = createRegularVolume(device, data, derived.value);

    auto storage = RegularVolumeStorage{std::move(data), std::move(derived.value)};

    auto ptr = toShared(UserRegularVolume{{std::move(storage)}, std::move(volume)});

    auto object = UserVolume{
        .value = ptr,
        .get = [=] { return ptr->value; },
    };

    auto stored = objects.add(std::move(object), {"RegularVolume"}, base);

    return getResult(stored);
}

GetRegularVolumeResult getRegularVolume(ObjectManager &objects, const GetObjectParams &params)
{
    auto stored = objects.getAsStored<UserVolume>(params.id);
    auto &volume = *castAsShared<UserRegularVolume>(stored.get().value, stored);
    auto &storage = volume.storage;

    auto result = RegularVolumeParams{storage.data.type, storage.data.size, storage.settings};

    return getResult(result);
}

void updateRegularVolume(ObjectManager &objects, Device &device, const UpdateRegularVolumeParams &params)
{
    auto stored = objects.getAsStored<UserVolume>(params.id);
    auto &volume = *castAsShared<UserRegularVolume>(stored.get().value, stored);

    auto settings = getUpdatedParams(params, volume.storage.settings);

    volume.value.update(settings);
    device.throwIfError();

    volume.storage.settings = std::move(settings);
}
}

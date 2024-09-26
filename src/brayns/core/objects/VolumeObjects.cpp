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

namespace
{
using namespace brayns;

RegularVolumeData createVolumeData(Device &device, const RegularVolumeParams &params, std::string_view binary)
{
    switch (params.voxelType)
    {
    case VoxelType::U8:
        return createData3DFromBinaryOf<std::uint8_t>(device, params.voxelCount, binary);
    case VoxelType::U16:
        return createData3DFromBinaryOf<std::uint16_t>(device, params.voxelCount, binary);
    case VoxelType::F32:
        return createData3DFromBinaryOf<float>(device, params.voxelCount, binary);
    case VoxelType::F64:
        return createData3DFromBinaryOf<double>(device, params.voxelCount, binary);
    default:
        throw std::invalid_argument("Invalid voxel data type");
    }
}
}

namespace brayns
{
CreateObjectResult createRegularVolume(ObjectManager &objects, Device &device, const CreateRegularVolumeParams &params)
{
    const auto &[json, binary] = params;

    auto data = createVolumeData(device, json.derived, binary);
    auto volume = createRegularVolume(device, data, json.derived.value);

    auto ptr = toShared(UserRegularVolume{json.derived, std::move(volume)});

    auto object = UserVolume{
        .value = ptr,
        .get = [=] { return ptr->value; },
    };

    auto stored = objects.add(std::move(object), {"RegularVolume"}, json.base);

    return getResult(stored);
}

GetRegularVolumeResult getRegularVolume(ObjectManager &objects, const GetObjectParams &params)
{
    auto stored = objects.getAsStored<UserVolume>(params.id);
    auto &operation = *castAsShared<UserRegularVolume>(stored.get().value, stored);
    return getResult(operation.settings);
}

void updateRegularVolume(ObjectManager &objects, Device &device, const UpdateRegularVolumeParams &params)
{
    auto stored = objects.getAsStored<UserVolume>(params.id);
    auto &volume = *castAsShared<UserRegularVolume>(stored.get().value, stored);

    auto settings = getUpdatedParams(params, volume.settings.value);

    volume.value.update(settings);
    device.throwIfError();

    volume.settings.value = std::move(settings);
}
}

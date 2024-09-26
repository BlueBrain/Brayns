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

#include "FramebufferObjects.h"

namespace
{
using namespace brayns;

std::vector<Stored<UserImageOperation>> getImageOperations(ObjectManager &objects, const std::vector<ObjectId> &ids)
{
    auto operations = std::vector<Stored<UserImageOperation>>();
    operations.reserve(ids.size());

    for (auto id : ids)
    {
        auto interface = objects.getAsStored<UserImageOperation>(id);
        operations.push_back(std::move(interface));
    }

    return operations;
}

std::vector<ObjectId> getImageOperationIds(const std::vector<Stored<UserImageOperation>> &operations)
{
    auto ids = std::vector<ObjectId>();
    ids.reserve(operations.size());

    for (const auto &operation : operations)
    {
        ids.push_back(operation.getId());
    }

    return ids;
}

std::optional<Data<ImageOperation>> createImageOperationData(Device &device, const std::vector<Stored<UserImageOperation>> &operations)
{
    auto itemCount = operations.size();

    if (itemCount == 0)
    {
        return std::nullopt;
    }

    auto data = allocateData<ImageOperation>(device, itemCount);
    auto items = data.getItems();

    for (auto i = std::size_t(0); i < itemCount; ++i)
    {
        const auto &interface = operations[i].get();
        items[i] = interface.get();
    }

    return data;
}

UserFramebuffer createUserFramebuffer(ObjectManager &objects, Device &device, const FramebufferParams &params)
{
    auto settings = params.value;

    auto operations = getImageOperations(objects, params.operations);
    settings.operations = createImageOperationData(device, operations);

    auto framebuffer = createFramebuffer(device, settings);

    return UserFramebuffer{
        .settings = std::move(settings),
        .operations = std::move(operations),
        .value = std::move(framebuffer),
    };
}
}

namespace brayns
{
CreateObjectResult createFramebuffer(ObjectManager &objects, Device &device, const CreateFramebufferParams &params)
{
    auto object = createUserFramebuffer(objects, device, params.derived);

    auto stored = objects.add(std::move(object), {"Framebuffer"}, params.base);

    return getResult(stored);
}

GetFramebufferResult getFramebuffer(ObjectManager &objects, const GetObjectParams &params)
{
    auto &framebuffer = objects.getAs<UserFramebuffer>(params.id);

    auto ids = getImageOperationIds(framebuffer.operations);
    auto variance = framebuffer.value.getVariance();

    return getResult(FramebufferInfo{{framebuffer.settings, std::move(ids)}, variance});
}

void updateFramebuffer(ObjectManager &objects, Device &device, const UpdateFramebufferParams &params)
{
    auto &object = objects.getAs<UserFramebuffer>(params.id);

    auto ids = getImageOperationIds(object.operations);
    auto current = FramebufferParams{object.settings, std::move(ids)};

    auto settings = getUpdatedParams(params, current);

    object = createUserFramebuffer(objects, device, settings);
}

void clearFramebuffer(ObjectManager &objects, const GetObjectParams &params)
{
    auto &framebuffer = objects.getAs<UserFramebuffer>(params.id);
    framebuffer.value.resetAccumulation();
}
}

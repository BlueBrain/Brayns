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

#include "FramebufferEndpoints.h"

namespace brayns
{
std::vector<Stored<ImageOperationInterface>> getImageOperations(ObjectManager &objects, const std::vector<ObjectId> &ids)
{
    auto interfaces = std::vector<Stored<ImageOperationInterface>>();
    interfaces.reserve(ids.size());

    for (auto id : ids)
    {
        auto interface = objects.getStored<ImageOperationInterface>(id);
        interfaces.push_back(std::move(interface));
    }

    return interfaces;
}

std::vector<ObjectId> getImageOperationIds(const std::vector<Stored<ImageOperationInterface>> &operations)
{
    auto ids = std::vector<ObjectId>();
    ids.reserve(operations.size());

    for (const auto &operation : operations)
    {
        ids.push_back(operation.getId());
    }

    return ids;
}

Data<ImageOperation> createImageOperationData(
    Device &device,
    const std::vector<Stored<ImageOperationInterface>> &operations)
{
    auto itemCount = operations.size();

    auto data = allocateData<ImageOperation>(device, itemCount);
    auto items = data.getItems();

    for (auto i = std::size_t(0); i < itemCount; ++i)
    {
        const auto &interface = operations[i].get();

        items[i] = interface.getDeviceObject();
    }

    return data;
}

UserFramebuffer createUserFramebuffer(ObjectManager &objects, Device &device, FramebufferParams params)
{
    auto operations = getImageOperations(objects, params.imageOperations);

    if (!operations.empty())
    {
        params.settings.imageOperations = createImageOperationData(device, operations);
    }

    auto framebuffer = createFramebuffer(device, params.settings);

    return {std::move(framebuffer), std::move(params.settings), std::move(operations)};
}

ObjectResult addFramebuffer(LockedObjects &locked, Device &device, FramebufferParams params)
{
    return locked.visit(
        [&](ObjectManager &objects)
        {
            auto framebuffer = createUserFramebuffer(objects, device, std::move(params));
            auto stored = objects.add(std::move(framebuffer));

            return stored.getResult();
        });
}

FramebufferParams getFramebuffer(LockedObjects &locked, const ObjectParams &params)
{
    return locked.visit(
        [&](ObjectManager &objects)
        {
            auto &framebuffer = objects.get<UserFramebuffer>(params.id);

            auto ids = getImageOperationIds(framebuffer.imageOperations);

            return FramebufferParams{framebuffer.settings, std::move(ids)};
        });
}

struct FramebufferOperations
{
    std::vector<ObjectId> imageOperations;
};

template<>
struct JsonObjectReflector<FramebufferOperations>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<FramebufferOperations>();
        builder.field("imageOperations", [](auto &object) { return &object.imageOperations; })
            .description("IDs of the image operations to attach to the framebuffer");
        return builder.build();
    }
};

using FramebufferUpdate = UpdateParams<FramebufferOperations>;

void updateFramebuffer(LockedObjects &locked, Device &device, const FramebufferUpdate &params)
{
    locked.visit(
        [&](ObjectManager &objects)
        {
            auto &framebuffer = objects.get<UserFramebuffer>(params.id);

            auto operations = getImageOperations(objects, params.properties.imageOperations);
            auto data = createImageOperationData(device, operations);

            framebuffer.deviceObject.setImageOperations(data);
            device.throwIfError();

            framebuffer.imageOperations = std::move(operations);
            framebuffer.settings.imageOperations = std::move(data);
        });
}

void addFramebufferEndpoints(ApiBuilder &builder, LockedObjects &objects, Device &device)
{
    builder
        .endpoint("createFramebuffer", [&](FramebufferParams params) { return addFramebuffer(objects, device, params); })
        .description("Create a new framebuffer");

    builder.endpoint("getFramebuffer", [&](ObjectParams params) { return getFramebuffer(objects, params); })
        .description("Get properties of a given framebuffer");

    builder.endpoint("updateFramebuffer", [&](FramebufferUpdate params) { updateFramebuffer(objects, device, params); })
        .description("Get properties of a given framebuffer");
}
}

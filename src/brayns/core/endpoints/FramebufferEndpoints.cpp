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

#include <cmath>

namespace brayns
{
template<>
struct EnumReflector<FramebufferFormat>
{
    static auto reflect()
    {
        auto builder = EnumBuilder<FramebufferFormat>();
        builder.field("Rgba8", FramebufferFormat::Rgba8).description("8 bit linear RGBA");
        builder.field("Srgba8", FramebufferFormat::Srgba8).description("8 bit gamma-encoded RGB and linear A");
        builder.field("Rgba32F", FramebufferFormat::Rgba32F).description("32 bit float RGBA");
        return builder.build();
    }
};

template<>
struct JsonObjectReflector<Accumulation>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<Accumulation>();
        builder.field("variance", [](auto &object) { return &object.variance; })
            .description("Wether to store per-pixel variance in a channel")
            .defaultValue(false);
        return builder.build();
    }
};

struct FramebufferParams
{
    FramebufferSettings settings;
    std::vector<ObjectId> imageOperations;
};

template<>
struct JsonObjectReflector<FramebufferParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<FramebufferParams>();
        builder.field("resolution", [](auto &object) { return &object.settings.resolution; })
            .description("Framebuffer resolution in pixel (max supported by ospray is currently 20kx20k)")
            .items()
            .maximum(20'000);
        builder.field("format", [](auto &object) { return &object.settings.format; })
            .description("Format of the framebuffer color channel")
            .defaultValue(FramebufferFormat::Srgba8);
        builder.field("channels", [](auto &object) { return &object.settings.channels; })
            .description("Framebuffer channels that can be accessed by user")
            .defaultValue(std::set<FramebufferChannel>{FramebufferChannel::Color});
        builder.field("accumulation", [](auto &object) { return &object.settings.accumulation; })
            .description("If not null, the framebuffer will use accumulation with given settings");
        builder.field("imageOperations", [](auto &object) { return &object.imageOperations; })
            .description("List of image operation IDs that will be applied on the framebuffer")
            .defaultValue(std::vector<ObjectId>())
            .uniqueItems(true);
        return builder.build();
    }
};

struct FramebufferResult
{
    FramebufferParams params;
    std::optional<float> variance;
};

template<>
struct JsonObjectReflector<FramebufferResult>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<FramebufferResult>();
        builder.field("params", [](auto &object) { return &object.params; })
            .description("Params used to create the framebuffer");
        builder.field("variance", [](auto &object) { return &object.variance; })
            .description("Variance of the framebuffer (null if no estimate is available)");
        return builder.build();
    }
};

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

std::optional<Data<ImageOperation>> createImageOperationData(
    Device &device,
    const std::vector<Stored<ImageOperationInterface>> &operations)
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

        items[i] = interface.getDeviceObject();
    }

    return data;
}

UserFramebuffer createUserFramebuffer(ObjectManager &objects, Device &device, FramebufferParams params)
{
    auto operations = getImageOperations(objects, params.imageOperations);

    params.settings.imageOperations = createImageOperationData(device, operations);

    auto framebuffer = createFramebuffer(device, params.settings);

    return {std::move(framebuffer), std::move(params.settings), std::move(operations)};
}

ObjectResult addFramebuffer(LockedObjects &locked, Device &device, FramebufferParams params)
{
    return locked.visit(
        [&](ObjectManager &objects)
        {
            auto framebuffer = createUserFramebuffer(objects, device, std::move(params));
            auto stored = objects.add(std::move(framebuffer), "Framebuffer");

            return ObjectResult{stored.getId()};
        });
}

FramebufferResult getFramebuffer(LockedObjects &locked, const ObjectParams &params)
{
    return locked.visit(
        [&](ObjectManager &objects)
        {
            auto &framebuffer = objects.get<UserFramebuffer>(params.id);

            auto ids = getImageOperationIds(framebuffer.imageOperations);

            auto params = FramebufferParams{framebuffer.settings, std::move(ids)};

            auto variance = framebuffer.deviceObject.getVariance();

            return FramebufferResult{std::move(params), variance};
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

void clearFramebuffer(LockedObjects &locked, const ObjectParams &params)
{
    locked.visit(
        [&](ObjectManager &objects)
        {
            auto &framebuffer = objects.get<UserFramebuffer>(params.id);
            framebuffer.deviceObject.resetAccumulation();
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
        .description("Update properties of a given framebuffer");

    builder.endpoint("clearFramebuffer", [&](ObjectParams params) { clearFramebuffer(objects, params); })
        .description("Reset accumulating channels of the framebuffer");
}
}

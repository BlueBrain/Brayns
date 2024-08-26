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

#include "ImageOperationEndpoints.h"

namespace brayns
{
template<typename T>
struct ImageOperationReflector;

template<std::derived_from<ImageOperation> T>
using GetImageOperationSettings = typename ImageOperationReflector<T>::Settings;

template<std::derived_from<ImageOperation> T>
using ImageOperationParams = ComposedParams<NullJson, GetImageOperationSettings<T>>;

template<std::derived_from<ImageOperation> T>
using ImageOperationUpdate = UpdateParams<GetImageOperationSettings<T>>;

template<typename T>
concept ReflectedImageOperation =
    ReflectedJson<GetImageOperationSettings<T>>
    && std::same_as<std::string, decltype(ImageOperationReflector<T>::getType())>
    && std::same_as<
        T,
        decltype(ImageOperationReflector<T>::create(std::declval<Device &>(), GetImageOperationSettings<T>()))>
    && std::is_void_v<decltype(ImageOperationReflector<T>::update(std::declval<T &>(), GetImageOperationSettings<T>()))>;

template<ReflectedImageOperation T>
std::string getImageOperationType()
{
    return ImageOperationReflector<T>::getType();
}

template<ReflectedImageOperation T>
T createImageOperation(Device &device, const GetImageOperationSettings<T> &settings)
{
    return ImageOperationReflector<T>::create(device, settings);
}

template<ReflectedImageOperation T>
void updateImageOperation(T &operation, const GetImageOperationSettings<T> &settings)
{
    return ImageOperationReflector<T>::update(operation, settings);
}

template<ReflectedImageOperation T>
struct UserImageOperation
{
    T deviceObject;
    GetImageOperationSettings<T> settings;
};

template<ReflectedImageOperation T>
ImageOperationInterface createImageOperationInterface(const std::shared_ptr<UserImageOperation<T>> &operation)
{
    return {
        .value = operation,
        .getType = [] { return getImageOperationType<T>(); },
        .getDeviceObject = [=] { return operation->deviceObject; },
    };
}

template<ReflectedImageOperation T>
UserImageOperation<T> &getImageOperation(ObjectManager &objects, ObjectId id)
{
    auto &interface = objects.get<ImageOperationInterface>(id);
    auto *ptr = std::any_cast<std::shared_ptr<UserImageOperation<T>>>(&interface.value);

    if (ptr != nullptr)
    {
        return **ptr;
    }

    auto type = interface.getType();
    throw InvalidParams(fmt::format("Invalid image operation type for object {}: {}", id, type));
}

template<ReflectedImageOperation T>
ObjectResult addImageOperation(LockedObjects &locked, Device &device, const GetImageOperationSettings<T> &settings)
{
    return locked.visit(
        [&](ObjectManager &objects)
        {
            auto operation = createImageOperation<T>(device, settings);
            auto object = UserImageOperation<T>{operation, settings};
            auto ptr = std::make_shared<decltype(object)>(std::move(object));

            auto interface = createImageOperationInterface(ptr);

            auto stored = objects.add(std::move(interface));

            return stored.getResult();
        });
}

template<ReflectedImageOperation T>
GetImageOperationSettings<T> getImageOperationAs(LockedObjects &locked, const ObjectParams &params)
{
    return locked.visit(
        [&](ObjectManager &objects)
        {
            auto &operation = getImageOperation<T>(objects, params.id);

            return operation.settings;
        });
}

template<ReflectedImageOperation T>
void updateImageOperationAs(LockedObjects &locked, Device &device, const ImageOperationUpdate<T> &params)
{
    locked.visit(
        [&](ObjectManager &objects)
        {
            auto &operation = getImageOperation<T>(objects, params.id);

            updateImageOperation(operation.deviceObject, params.properties);
            device.throwIfError();

            operation.settings = params.properties;
        });
}

template<ReflectedImageOperation T>
void addImageOperationType(ApiBuilder &builder, LockedObjects &objects, Device &device)
{
    auto type = getImageOperationType<T>();

    builder
        .endpoint(
            "create" + type,
            [&](ImageOperationParams<T> params) { return addImageOperation<T>(objects, device, params.derived); })
        .description("Create an image operation of type " + type);

    builder.endpoint("get" + type, [&](ObjectParams params) { return getImageOperationAs<T>(objects, params); })
        .description("Get derived properties of an image operation of type " + type);

    builder
        .endpoint(
            "update" + type,
            [&](ImageOperationUpdate<T> params) { updateImageOperationAs<T>(objects, device, params); })
        .description("Update derived properties of an image operation of type " + type);
}

template<>
struct JsonObjectReflector<ToneMapperSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<ToneMapperSettings>();
        builder.field("exposure", [](auto &object) { return &object.exposure; })
            .description("Amount of light per unit area")
            .defaultValue(1.0F);
        builder.field("contrast", [](auto &object) { return &object.contrast; })
            .description("Constrast (toe of the curve)")
            .defaultValue(1.6773F);
        builder.field("shoulder", [](auto &object) { return &object.shoulder; })
            .description("Highlight compression (shoulder of the curve)")
            .defaultValue(0.9714F);
        builder.field("midIn", [](auto &object) { return &object.midIn; })
            .description("Mid-level anchor input")
            .defaultValue(0.18F);
        builder.field("midOut", [](auto &object) { return &object.midOut; })
            .description("Mid-level anchor output")
            .defaultValue(0.18F);
        builder.field("hdrMax", [](auto &object) { return &object.hdrMax; })
            .description("Maximum HDR input that is not clipped")
            .defaultValue(11.0785F);
        builder.field("acesColor", [](auto &object) { return &object.acesColor; })
            .description("Apply the ACES color transforms")
            .defaultValue(true);
        return builder.build();
    }
};

template<>
struct ImageOperationReflector<ToneMapper>
{
    using Settings = ToneMapperSettings;

    static std::string getType()
    {
        return "ToneMapper";
    }

    static ToneMapper create(Device &device, const ToneMapperSettings &settings)
    {
        return createToneMapper(device, settings);
    }

    static void update(ToneMapper &toneMapper, const ToneMapperSettings &settings)
    {
        toneMapper.update(settings);
    }
};

void addImageOperationEndpoints(ApiBuilder &builder, LockedObjects &objects, Device &device)
{
    addImageOperationType<ToneMapper>(builder, objects, device);
}
}

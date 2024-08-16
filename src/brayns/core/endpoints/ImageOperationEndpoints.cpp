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
struct OperationReflector;

template<std::derived_from<ImageOperation> T>
using GetOperationSettings = typename OperationReflector<T>::Settings;

template<std::derived_from<ImageOperation> T>
using OperationParams = ComposedParams<NullJson, GetOperationSettings<T>>;

template<std::derived_from<ImageOperation> T>
using OperationUpdate = UpdateParams<GetOperationSettings<T>>;

template<typename T>
concept ReflectedOperation =
    ReflectedJson<GetOperationSettings<T>> && std::same_as<std::string, decltype(OperationReflector<T>::getType())>
    && std::same_as<T, decltype(OperationReflector<T>::create(std::declval<Device &>(), GetOperationSettings<T>()))>
    && std::is_void_v<decltype(OperationReflector<T>::update(std::declval<T &>(), GetOperationSettings<T>()))>;

template<ReflectedOperation T>
std::string getOperationType()
{
    return OperationReflector<T>::getType();
}

template<ReflectedOperation T>
T createOperation(Device &device, const GetOperationSettings<T> &settings)
{
    return OperationReflector<T>::create(device, settings);
}

template<ReflectedOperation T>
void updateOperation(T &operation, const GetOperationSettings<T> &settings)
{
    return OperationReflector<T>::update(operation, settings);
}

template<ReflectedOperation T>
struct UserOperation
{
    T deviceObject;
    GetOperationSettings<T> settings;
};

template<ReflectedOperation T>
ImageOperationInterface createOperationInterface(const std::shared_ptr<UserOperation<T>> &operation)
{
    return {
        .value = operation,
        .getType = [] { return getOperationType<T>(); },
        .getDeviceObject = [=] { return operation->deviceObject; },
    };
}

template<ReflectedOperation T>
UserOperation<T> &getOperation(ObjectManager &objects, ObjectId id)
{
    auto &interface = objects.get<ImageOperationInterface>(id);
    auto *ptr = std::any_cast<std::shared_ptr<UserOperation<T>>>(&interface.value);

    if (ptr != nullptr)
    {
        return **ptr;
    }

    auto type = interface.getType();
    throw InvalidParams(fmt::format("Invalid image operation type for object {}: {}", id, type));
}

template<ReflectedOperation T>
ObjectResult addOperation(LockedObjects &locked, Device &device, const GetOperationSettings<T> &settings)
{
    return locked.visit(
        [&](ObjectManager &objects)
        {
            auto operation = createOperation<T>(device, settings);
            auto object = UserOperation<T>{operation, settings};
            auto ptr = std::make_shared<decltype(object)>(std::move(object));

            auto interface = createOperationInterface(ptr);

            auto stored = objects.add(std::move(interface));

            return stored.getResult();
        });
}

template<ReflectedOperation T>
GetOperationSettings<T> getOperationAs(LockedObjects &locked, const ObjectParams &params)
{
    return locked.visit(
        [&](ObjectManager &objects)
        {
            auto &operation = getOperation<T>(objects, params.id);

            return operation.settings;
        });
}

template<ReflectedOperation T>
void updateOperationAs(LockedObjects &locked, Device &device, const OperationUpdate<T> &params)
{
    locked.visit(
        [&](ObjectManager &objects)
        {
            auto &operation = getOperation<T>(objects, params.id);

            updateOperation(operation.deviceObject, params.properties);
            device.throwIfError();

            operation.settings = params.properties;
        });
}

template<ReflectedOperation T>
void addOperationType(ApiBuilder &builder, LockedObjects &objects, Device &device)
{
    auto type = getOperationType<T>();

    builder
        .endpoint(
            "create" + type,
            [&](OperationParams<T> params) { return addOperation<T>(objects, device, params.derived); })
        .description("Create an image operation of type " + type);

    builder.endpoint("get" + type, [&](ObjectParams params) { return getOperationAs<T>(objects, params); })
        .description("Get derived properties of an image operation of type " + type);

    builder.endpoint("update" + type, [&](OperationUpdate<T> params) { updateOperationAs<T>(objects, device, params); })
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
struct OperationReflector<ToneMapper>
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
    addOperationType<ToneMapper>(builder, objects, device);
}
}

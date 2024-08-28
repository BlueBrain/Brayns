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

#include "TransferFunctionEndpoints.h"

namespace brayns
{
template<typename T>
struct TransferFunctionReflector;

template<std::derived_from<TransferFunction> T>
using GetTransferFunctionSettings = typename TransferFunctionReflector<T>::Settings;

template<std::derived_from<TransferFunction> T>
using TransferFunctionParams = ComposedParams<NullJson, GetTransferFunctionSettings<T>>;

template<std::derived_from<TransferFunction> T>
using TransferFunctionUpdateOf = UpdateParams<GetTransferFunctionSettings<T>>;

template<typename T>
concept ReflectedTransferFunction = ReflectedJson<GetTransferFunctionSettings<T>>
    && std::same_as<T,
                    decltype(TransferFunctionReflector<T>::create(std::declval<Device &>(), TransferFunctionParams<T>()))>
    && std::is_void_v<decltype(TransferFunctionReflector<T>::update(
        std::declval<Device &>(),
        std::declval<T &>(),
        GetTransferFunctionSettings<T>()))>;

template<ReflectedTransferFunction T>
struct UserTransferFunction
{
    T deviceObject;
    TransferFunctionParams<T> params;
};

template<ReflectedTransferFunction T>
TransferFunctionInterface createTransferFunctionInterface(const std::shared_ptr<UserTransferFunction<T>> &function)
{
    return {
        .value = function,
        .getDeviceObject = [=] { return function->deviceObject; },
    };
}

template<ReflectedTransferFunction T>
UserTransferFunction<T> &castTransferFunctionAs(ObjectManager &objects, ObjectId id)
{
    auto interface = objects.getStored<TransferFunctionInterface>(id);
    return castObjectAs<UserTransferFunction<T>>(interface->value, interface.getInfo());
}

template<ReflectedTransferFunction T>
ObjectResult createTransferFunctionAs(LockedObjects &locked, Device &device, const TransferFunctionParams<T> &params)
{
    return locked.visit(
        [&](ObjectManager &objects)
        {
            auto function = TransferFunctionReflector<T>::create(device, params);
            auto object = UserTransferFunction<T>{function, params};
            auto ptr = std::make_shared<decltype(object)>(std::move(object));

            auto interface = createTransferFunctionInterface(ptr);
            auto type = TransferFunctionReflector<T>::getType();

            auto stored = objects.add(std::move(interface), std::move(type));

            return ObjectResult{stored.getId()};
        });
}

template<ReflectedTransferFunction T>
GetTransferFunctionSettings<T> getTransferFunctionAs(LockedObjects &locked, const ObjectParams &params)
{
    return locked.visit(
        [&](ObjectManager &objects)
        {
            auto &function = castTransferFunctionAs<T>(objects, params.id);
            return function.params.derived;
        });
}

template<ReflectedTransferFunction T>
void updateTransferFunctionAs(LockedObjects &locked, Device &device, const TransferFunctionUpdateOf<T> &params)
{
    locked.visit(
        [&](ObjectManager &objects)
        {
            auto &function = castTransferFunctionAs<T>(objects, params.id);

            TransferFunctionReflector<T>::update(device, function.deviceObject, params.properties);
            device.throwIfError();

            function.params.derived = params.properties;
        });
}

template<ReflectedTransferFunction T>
void addTransferFunctionType(ApiBuilder &builder, LockedObjects &objects, Device &device)
{
    auto type = TransferFunctionReflector<T>::getType();

    builder
        .endpoint(
            "create" + type,
            [&](TransferFunctionParams<T> params) { return createTransferFunctionAs<T>(objects, device, params); })
        .description("Create an image function of type " + type);

    builder.endpoint("get" + type, [&](ObjectParams params) { return getTransferFunctionAs<T>(objects, params); })
        .description("Get derived properties of an image function of type " + type);

    builder
        .endpoint(
            "update" + type,
            [&](TransferFunctionUpdateOf<T> params) { updateTransferFunctionAs<T>(objects, device, params); })
        .description("Update derived properties of an image function of type " + type);
}

struct LinearTransferFunctionParams
{
    Box1 scalarRange;
    std::vector<Color4> colors;
};

template<>
struct JsonObjectReflector<LinearTransferFunctionParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<LinearTransferFunctionParams>();
        builder.field("scalarRange", [](auto &object) { return &object.scalarRange; })
            .description("Range of the scalar values sampled from the volume that will be mapped to colors");
        builder.field("colors", [](auto &object) { return &object.colors; })
            .description("Colors to map the values sampled from the volume")
            .minItems(1);
        return builder.build();
    }
};

template<>
struct TransferFunctionReflector<LinearTransferFunction>
{
    using Settings = LinearTransferFunctionParams;

    static std::string getType()
    {
        return "LinearTransferFunction";
    }

    static LinearTransferFunction create(Device &device, const TransferFunctionParams<LinearTransferFunction> &params)
    {
        auto data = createData<Color4>(device, params.derived.colors);

        return createLinearTransferFunction(device, {params.derived.scalarRange, std::move(data)});
    }

    static void update(Device &device, LinearTransferFunction &function, const LinearTransferFunctionParams &params)
    {
        auto data = createData<Color4>(device, params.colors);

        function.update({params.scalarRange, std::move(data)});
    }
};

void addTransferFunctionEndpoints(ApiBuilder &builder, LockedObjects &objects, Device &device)
{
    addTransferFunctionType<LinearTransferFunction>(builder, objects, device);
}
}

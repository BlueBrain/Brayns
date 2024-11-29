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

template<OsprayDataType T>
MaterialStorage<T> toMaterialStorage(ObjectManager &objects, const MaterialTexture2DParams<T> &params)
{
    auto object = objects.getAsStored<UserTexture>(params.value);
    return MaterialTexture2DStorage<T>{std::move(object), params.transform, params.factor};
}

template<OsprayDataType T>
MaterialStorage<T> toMaterialStorage(ObjectManager &objects, const MaterialVolumeTextureParams<T> &params)
{
    auto object = objects.getAsStored<UserTexture>(params.value);
    return MaterialVolumeTextureStorage<T>{std::move(object), params.transform, params.factor};
}

template<OsprayDataType T>
MaterialStorage<T> toMaterialStorage(ObjectManager &objects, const T &params)
{
    (void)objects;
    return params;
}

template<OsprayDataType T>
MaterialStorage<T> toMaterialStorage(ObjectManager &objects, const MaterialParams<T> &params)
{
    return std::visit([&](const auto &value) { return toMaterialStorage(objects, value); }, params);
}

template<template<template<typename> typename> typename T>
T<MaterialStorage> prepareStorage(ObjectManager &objects, const T<MaterialParams> &params)
{
    return converMaterialSettingsTo<T<MaterialStorage>>(params, [&](const auto &value) { return toMaterialStorage(objects, value); });
}

template<OsprayDataType T>
MaterialField<T> toMaterialField(const MaterialTexture2DStorage<T> &params)
{
    auto &texture = *castAsShared<UserTexture2D>(params.value.get().value, params.value);
    return MaterialTexture2D<T>{texture.value, params.transform, params.factor};
}

template<OsprayDataType T>
MaterialField<T> toMaterialField(const MaterialVolumeTextureStorage<T> &params)
{
    auto &texture = *castAsShared<UserVolumeTexture>(params.value.get().value, params.value);
    return MaterialVolumeTexture<T>{texture.value, params.transform, params.factor};
}

template<OsprayDataType T>
MaterialField<T> toMaterialField(const T &params)
{
    return params;
}

template<OsprayDataType T>
MaterialField<T> toMaterialField(const MaterialStorage<T> &params)
{
    return std::visit([&](const auto &value) { return toMaterialField(value); }, params);
}

template<template<template<typename> typename> typename T>
T<MaterialField> prepareSettings(const T<MaterialStorage> &storage)
{
    return converMaterialSettingsTo<T<MaterialField>>(storage, [](const auto &value) { return toMaterialField(value); });
}

template<OsprayDataType T>
MaterialParams<T> toMaterialParams(const MaterialTexture2DStorage<T> &params)
{
    return MaterialTexture2DParams<T>{params.value.getId(), params.transform, params.factor};
}

template<OsprayDataType T>
MaterialParams<T> toMaterialParams(const MaterialVolumeTextureStorage<T> &params)
{
    return MaterialVolumeTextureParams<T>{params.value.getId(), params.transform, params.factor};
}

template<OsprayDataType T>
MaterialParams<T> toMaterialParams(const T &params)
{
    return params;
}

template<OsprayDataType T>
MaterialParams<T> toMaterialParams(const MaterialStorage<T> &params)
{
    return std::visit([&](const auto &value) { return toMaterialParams(value); }, params);
}

template<template<template<typename> typename> typename T>
T<MaterialParams> prepareParams(const T<MaterialStorage> &storage)
{
    return converMaterialSettingsTo<T<MaterialParams>>(storage, [](const auto &value) { return toMaterialParams(value); });
}

template<typename T>
CreateObjectResult createMaterialAs(ObjectManager &objects, Device &device, const auto &params, auto &&create, std::string name)
{
    const auto &[base, derived] = params;

    auto storage = prepareStorage(objects, derived);
    auto settings = prepareSettings(storage);

    auto material = create(device, settings);

    auto ptr = toShared(T{std::move(storage), std::move(material)});

    auto object = UserMaterial{
        .value = ptr,
        .get = [=] { return ptr->value; },
    };

    auto stored = objects.add(std::move(object), {std::move(name)}, std::move(base));

    return getResult(stored);
}

template<typename T>
auto getMaterialAs(ObjectManager &objects, const GetObjectParams &params)
{
    auto stored = objects.getAsStored<UserMaterial>(params.id);
    auto &material = *castAsShared<T>(stored.get().value, stored);

    auto result = prepareParams(material.storage);

    return getResult(std::move(result));
}

template<typename T>
void updateMaterialAs(ObjectManager &objects, Device &device, const auto &params)
{
    auto stored = objects.getAsStored<UserMaterial>(params.id);
    auto &material = *castAsShared<T>(stored.get().value, stored);

    auto current = prepareParams(material.storage);
    auto updated = getUpdatedParams(params, current);

    auto storage = prepareStorage(objects, updated);
    auto settings = prepareSettings(storage);

    material.value.update(settings);
    device.throwIfError();

    material.storage = std::move(storage);
}
}

namespace brayns
{
CreateObjectResult createAoMaterial(ObjectManager &objects, Device &device, const CreateAoMaterialParams &params)
{
    auto create = [](auto &device, const auto &settings) { return createAoMaterial(device, settings); };
    return createMaterialAs<UserAoMaterial>(objects, device, params, create, "AoMaterial");
}

GetAoMaterialResult getAoMaterial(ObjectManager &objects, const GetObjectParams &params)
{
    return getMaterialAs<UserAoMaterial>(objects, params);
}

void updateAoMaterial(ObjectManager &objects, Device &device, const UpdateAoMaterialParams &params)
{
    updateMaterialAs<UserAoMaterial>(objects, device, params);
}

CreateObjectResult createScivisMaterial(ObjectManager &objects, Device &device, const CreateScivisMaterialParams &params)
{
    auto create = [](auto &device, const auto &settings) { return createScivisMaterial(device, settings); };
    return createMaterialAs<UserScivisMaterial>(objects, device, params, create, "ScivisMaterial");
}

GetScivisMaterialResult getScivisMaterial(ObjectManager &objects, const GetObjectParams &params)
{
    return getMaterialAs<UserScivisMaterial>(objects, params);
}

void updateScivisMaterial(ObjectManager &objects, Device &device, const UpdateScivisMaterialParams &params)
{
    updateMaterialAs<UserScivisMaterial>(objects, device, params);
}

CreateObjectResult createPrincipledMaterial(ObjectManager &objects, Device &device, const CreatePrincipledMaterialParams &params)
{
    auto create = [](auto &device, const auto &settings) { return createPrincipledMaterial(device, settings); };
    return createMaterialAs<UserPrincipledMaterial>(objects, device, params, create, "PrincipledMaterial");
}

GetPrincipledMaterialResult getPrincipledMaterial(ObjectManager &objects, const GetObjectParams &params)
{
    return getMaterialAs<UserPrincipledMaterial>(objects, params);
}

void updatePrincipledMaterial(ObjectManager &objects, Device &device, const UpdatePrincipledMaterialParams &params)
{
    updateMaterialAs<UserPrincipledMaterial>(objects, device, params);
}
}

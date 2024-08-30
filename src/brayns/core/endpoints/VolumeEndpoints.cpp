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

#include "VolumeEndpoints.h"

#include "common/Binary.h"

namespace brayns
{
template<typename T>
struct VolumeReflector;

template<std::derived_from<Volume> T>
using GetVolumeSettings = typename VolumeReflector<T>::Settings;

template<std::derived_from<Volume> T>
using VolumeParams = ComposedParams<NullJson, GetVolumeSettings<T>>;

template<std::derived_from<Volume> T>
using GetVolumeUpdate = typename VolumeReflector<T>::Update;

template<std::derived_from<Volume> T>
using VolumeUpdateOf = UpdateParams<GetVolumeUpdate<T>>;

template<typename T>
concept ReflectedVolume = ReflectedJson<GetVolumeSettings<T>> && ReflectedJson<GetVolumeUpdate<T>>
    && std::same_as<T, decltype(VolumeReflector<T>::create(std::declval<Device &>(), Params<VolumeParams<T>>()))>
    && std::is_void_v<decltype(VolumeReflector<T>::update(std::declval<T &>(), GetVolumeUpdate<T>()))>;

template<ReflectedVolume T>
struct UserVolume
{
    T deviceObject;
    VolumeParams<T> params;
};

template<ReflectedVolume T>
VolumeInterface createVolumeInterface(const std::shared_ptr<UserVolume<T>> &volume)
{
    return {
        .value = volume,
        .getDeviceObject = [=] { return volume->deviceObject; },
    };
}

template<ReflectedVolume T>
UserVolume<T> &castVolumeAs(ObjectManager &objects, ObjectId id)
{
    auto interface = objects.getStored<VolumeInterface>(id);
    return castObjectAs<UserVolume<T>>(interface->value, interface.getInfo());
}

template<ReflectedVolume T>
ObjectResult createVolumeAs(LockedObjects &locked, Device &device, Params<VolumeParams<T>> params)
{
    return locked.visit(
        [&](ObjectManager &objects)
        {
            auto jsonPart = params.value;
            auto volume = VolumeReflector<T>::create(device, std::move(params));
            auto object = UserVolume<T>{volume, std::move(jsonPart)};
            auto ptr = std::make_shared<decltype(object)>(std::move(object));

            auto interface = createVolumeInterface(ptr);
            auto type = VolumeReflector<T>::getType();

            auto stored = objects.add(std::move(interface), std::move(type));

            return ObjectResult{stored.getId()};
        });
}

template<ReflectedVolume T>
GetVolumeSettings<T> getVolumeAs(LockedObjects &locked, const ObjectParams &params)
{
    return locked.visit(
        [&](ObjectManager &objects)
        {
            auto &volume = castVolumeAs<T>(objects, params.id);
            return volume.params.derived;
        });
}

template<ReflectedVolume T>
void updateVolumeAs(LockedObjects &locked, Device &device, const VolumeUpdateOf<T> &params)
{
    locked.visit(
        [&](ObjectManager &objects)
        {
            auto &volume = castVolumeAs<T>(objects, params.id);

            VolumeReflector<T>::update(volume.deviceObject, params.properties);
            device.throwIfError();

            volume.params.derived.settings = params.properties;
        });
}

template<ReflectedVolume T>
void addVolumeType(ApiBuilder &builder, LockedObjects &objects, Device &device)
{
    auto type = VolumeReflector<T>::getType();

    builder
        .endpoint(
            "create" + type,
            [&](Params<VolumeParams<T>> params) { return createVolumeAs<T>(objects, device, std::move(params)); })
        .description("Create an image volume of type " + type);

    builder.endpoint("get" + type, [&](ObjectParams params) { return getVolumeAs<T>(objects, params); })
        .description("Get derived properties of an image volume of type " + type);

    builder.endpoint("update" + type, [&](VolumeUpdateOf<T> params) { updateVolumeAs<T>(objects, device, params); })
        .description("Update derived properties of an image volume of type " + type);
}

enum class VoxelDataType
{
    U8,
    U16,
    F32,
    F64,
};

template<>
struct EnumReflector<VoxelDataType>
{
    static auto reflect()
    {
        auto builder = EnumBuilder<VoxelDataType>();
        builder.field("U8", VoxelDataType::U8).description("8 bit unsigned int");
        builder.field("U16", VoxelDataType::U16).description("16 bit unsigned int");
        builder.field("F32", VoxelDataType::F32).description("32 bit float");
        builder.field("F64", VoxelDataType::F64).description("32 bit float");
        return builder.build();
    }
};

template<>
struct EnumReflector<VolumeFilter>
{
    static auto reflect()
    {
        auto builder = EnumBuilder<VolumeFilter>();
        builder.field("Nearest", VolumeFilter::Nearest).description("Voxel sampling does no interpolations");
        builder.field("Linear", VolumeFilter::Linear).description("Voxel sampling does linear interpolation");
        builder.field("Cubic", VolumeFilter::Cubic).description("Voxel sampling does cubic interpolation");
        return builder.build();
    }
};

template<>
struct EnumReflector<VolumeSchema>
{
    static auto reflect()
    {
        auto builder = EnumBuilder<VolumeSchema>();
        builder.field("CellCentered", VolumeSchema::CellCentered).description("Volume data is provided per cell");
        builder.field("VertexCentered", VolumeSchema::VertexCentered).description("Volume data is provided per vertex");
        return builder.build();
    }
};

template<>
struct JsonObjectReflector<RegularVolumeSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<RegularVolumeSettings>();
        builder.field("origin", [](auto &object) { return &object.origin; })
            .description("Position of the volume origin")
            .defaultValue(Vector3(0, 0, 0));
        builder.field("spacing", [](auto &object) { return &object.spacing; })
            .description("Size of the volume cells")
            .defaultValue(Vector3(0, 0, 0));
        builder.field("schema", [](auto &object) { return &object.schema; })
            .description("Wether the data is provided per vertex or per cell")
            .defaultValue(VolumeSchema::VertexCentered);
        builder.field("filter", [](auto &object) { return &object.filter; })
            .description("How to interpolate sampled voxels")
            .defaultValue(VolumeFilter::Linear);
        builder.field("background", [](auto &object) { return &object.background; })
            .description("Value used when sampling an undefined region outside the volume (null to use NaN)");
        return builder.build();
    }
};

struct RegularVolumeParams
{
    VoxelDataType voxelDataType;
    Size3 voxelCount;
    RegularVolumeSettings settings;
};

template<>
struct JsonObjectReflector<RegularVolumeParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<RegularVolumeParams>();
        builder.field("voxelDataType", [](auto &object) { return &object.voxelDataType; })
            .description("Type of the provided voxels");
        builder.field("voxelCount", [](auto &object) { return &object.voxelCount; })
            .description("Volume dimensions in voxels");
        builder.field("settings", [](auto &object) { return &object.settings; })
            .description("Additional settings that can be updated");
        return builder.build();
    }
};

RegularVolumeData createVolumeData(Device &device, const RegularVolumeParams &params, std::string_view binary)
{
    switch (params.voxelDataType)
    {
    case VoxelDataType::U8:
        return createData3DFromBinaryOf<std::uint8_t>(device, params.voxelCount, binary);
    case VoxelDataType::U16:
        return createData3DFromBinaryOf<std::uint16_t>(device, params.voxelCount, binary);
    case VoxelDataType::F32:
        return createData3DFromBinaryOf<float>(device, params.voxelCount, binary);
    case VoxelDataType::F64:
        return createData3DFromBinaryOf<double>(device, params.voxelCount, binary);
    default:
        throw std::invalid_argument("Invalid voxel data type");
    }
}

template<>
struct VolumeReflector<RegularVolume>
{
    using Settings = RegularVolumeParams;
    using Update = RegularVolumeSettings;

    static std::string getType()
    {
        return "RegularVolume";
    }

    static RegularVolume create(Device &device, const Params<VolumeParams<RegularVolume>> &params)
    {
        auto data = createVolumeData(device, params.value.derived, params.binary);

        return createRegularVolume(device, data, params.value.derived.settings);
    }

    static void update(RegularVolume &volume, const RegularVolumeSettings &settings)
    {
        volume.update(settings);
    }
};

void addVolumeEndpoints(ApiBuilder &builder, LockedObjects &objects, Device &device)
{
    addVolumeType<RegularVolume>(builder, objects, device);
}
}

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

#pragma once

#include <any>
#include <functional>
#include <string_view>

#include <brayns/core/api/ApiBuilder.h>
#include <brayns/core/engine/Volume.h>
#include <brayns/core/manager/ObjectManager.h>

namespace brayns
{
template<ReflectedJson T>
using VolumeParams = ComposedParams<NullJson, T>;

struct UserVolume
{
    std::reference_wrapper<Device> device;
    std::any value;
    std::function<Volume()> get;
};

template<ReflectedJson Settings, std::derived_from<Volume> T>
struct DerivedVolume
{
    Settings settings;
    T value;
};

enum class VoxelType
{
    U8,
    U16,
    F32,
    F64,
};

template<>
struct EnumReflector<VoxelType>
{
    static auto reflect()
    {
        auto builder = EnumBuilder<VoxelType>();
        builder.field("U8", VoxelType::U8).description("8 bit unsigned int");
        builder.field("U16", VoxelType::U16).description("16 bit unsigned int");
        builder.field("F32", VoxelType::F32).description("32 bit float");
        builder.field("F64", VoxelType::F64).description("32 bit float");
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
struct EnumReflector<VolumeType>
{
    static auto reflect()
    {
        auto builder = EnumBuilder<VolumeType>();
        builder.field("CellCentered", VolumeType::CellCentered).description("Volume data is provided per cell");
        builder.field("VertexCentered", VolumeType::VertexCentered).description("Volume data is provided per vertex");
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
        builder.field("type", [](auto &object) { return &object.type; })
            .description("Wether the data is provided per vertex or per cell")
            .defaultValue(VolumeType::VertexCentered);
        builder.field("filter", [](auto &object) { return &object.filter; })
            .description("How to interpolate sampled voxels")
            .defaultValue(VolumeFilter::Linear);
        builder.field("background", [](auto &object) { return &object.background; })
            .description("Value used when sampling an undefined region outside the volume (null to use NaN)");
        return builder.build();
    }
};

struct RegularVolumeInfo
{
    VoxelType voxelType;
    Size3 voxelCount;
    RegularVolumeSettings settings;
};

template<>
struct JsonObjectReflector<RegularVolumeInfo>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<RegularVolumeInfo>();
        builder.field("voxelType", [](auto &object) { return &object.voxelType; })
            .description("Type of the provided voxels");
        builder.field("voxelCount", [](auto &object) { return &object.voxelCount; })
            .description("Volume dimensions XYZ");
        builder.field("settings", [](auto &object) { return &object.settings; })
            .description("Additional settings that can be updated");
        return builder.build();
    }
};

using RegularVolumeParams = Params<VolumeParams<RegularVolumeInfo>>;
using RegularVolumeUpdate = UpdateParams<RegularVolumeSettings>;
using UserRegularVolume = DerivedVolume<RegularVolumeInfo, RegularVolume>;

ObjectResult createRegularVolume(ObjectRegistry &objects, Device &device, const RegularVolumeParams &params);
RegularVolumeInfo getRegularVolume(ObjectRegistry &objects, const ObjectParams &params);
void updateRegularVolume(ObjectRegistry &objects, const RegularVolumeUpdate &params);
}

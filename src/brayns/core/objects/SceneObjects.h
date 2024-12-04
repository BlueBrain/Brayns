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
#include <concepts>
#include <functional>

#include <brayns/core/engine/GeometricModel.h>
#include <brayns/core/engine/VolumetricModel.h>
#include <brayns/core/engine/World.h>
#include <brayns/core/jsonrpc/PayloadReflector.h>
#include <brayns/core/manager/ObjectManager.h>

#include "GeometryObjects.h"
#include "LightObjects.h"
#include "MaterialObjects.h"
#include "TransferFunctionObjects.h"
#include "VolumeObjects.h"

namespace brayns
{
struct GeometricModelParams
{
    GeometricModelSettings value;
    ObjectId geometry;
};

template<>
struct JsonObjectReflector<GeometricModelParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<GeometricModelParams>();
        builder.field("geometry", [](auto &object) { return &object.geometry; }).description("ID of the geometry to use in the model");
        builder.field("materials", [](auto &object) { return &object.value.materials; })
            .description("Indices of materials stored in the renderer to apply per primitive on the geometry")
            .defaultValue(std::vector<ObjectId>{0});
        builder.field("colors", [](auto &object) { return &object.value.colors; })
            .description("List of colors to apply per primitive on the geometry")
            .defaultValue(std::vector<Color4>());
        builder.field("indices", [](auto &object) { return &object.value.indices; })
            .description("Optional indices to map materials and colors")
            .defaultValue(std::vector<std::uint32_t>());
        builder.field("invertNormals", [](auto &object) { return &object.value.invertNormals; })
            .description("Wether to invert or not shading normals (mostly for clipping)")
            .defaultValue(std::vector<std::uint32_t>());
        builder.field("id", [](auto &object) { return &object.value.id; })
            .description("Optional ID to retreive the model in framebuffer or pick result")
            .defaultValue(std::uint32_t(-1));
        return builder.build();
    }
};

struct UserGeometricModel
{
    GeometricModelSettings settings;
    GeometricModel value;
    Stored<UserGeometry> geometry;
};

using CreateGeometricModelParams = CreateParamsOf<GeometricModelParams>;
using GetGeometricModelResult = GetResultOf<GeometricModelParams>;
using UpdateGeometricModelParams = UpdateParamsOf<GeometricModelParams>;

CreateObjectResult createGeometricModel(ObjectManager &objects, Device &device, CreateGeometricModelParams params);
GetGeometricModelResult getGeometricModel(ObjectManager &objects, const GetObjectParams &params);
void updateGeometricModel(ObjectManager &objects, Device &device, const UpdateGeometricModelParams &params);

struct VolumetricModelParams
{
    VolumetricModelSettings value;
    ObjectId volume;
    ObjectId transferFunction;
};

template<>
struct JsonObjectReflector<VolumetricModelParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<VolumetricModelParams>();
        builder.field("volume", [](auto &object) { return &object.volume; }).description("ID of the volume to use in the model");
        builder.field("transferFunction", [](auto &object) { return &object.transferFunction; })
            .description("ID of the transfer function to use in the model");
        builder.field("densityScale", [](auto &object) { return &object.value.densityScale; })
            .description("Make volume uniformly thinner or thicker")
            .defaultValue(1.0F);
        builder.field("anisotropy", [](auto &object) { return &object.value.anisotropy; })
            .description("Anisotropy of the (Henyey-Greenstein) phase function")
            .defaultValue(0.0F)
            .minimum(-1)
            .maximum(1);
        builder.field("id", [](auto &object) { return &object.value.id; })
            .description("Optional ID to retreive the model in framebuffer or pick result")
            .defaultValue(std::uint32_t(-1));
        return builder.build();
    }
};

struct UserVolumetricModel
{
    VolumetricModelSettings settings;
    VolumetricModel value;
    Stored<UserVolume> volume;
    Stored<UserTransferFunction> transferFunction;
};

using CreateVolumetricModelParams = CreateParamsOf<VolumetricModelParams>;
using GetVolumetricModelResult = GetResultOf<VolumetricModelParams>;
using UpdateVolumetricModelParams = UpdateParamsOf<VolumetricModelParams>;

CreateObjectResult createVolumetricModel(ObjectManager &objects, Device &device, const CreateVolumetricModelParams &params);
GetVolumetricModelResult getVolumetricModel(ObjectManager &objects, const GetObjectParams &params);
void updateVolumetricModel(ObjectManager &objects, Device &device, const UpdateVolumetricModelParams &params);

template<ReflectedJsonObject T>
struct BoundedResultOf
{
    T value;
    Box3 bounds;
};

template<ReflectedJsonObject T>
struct JsonObjectReflector<BoundedResultOf<T>>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<BoundedResultOf<T>>();
        builder.extend([](auto &object) { return &object.value; });
        builder.field("bounds", [](auto &object) { return &object.bounds; }).description("Bounding box of the scene object min/max XYZ");
        return builder.build();
    }
};

struct GroupParams
{
    std::vector<ObjectId> geometries;
    std::vector<ObjectId> clippingGeometries;
    std::vector<ObjectId> volumes;
    std::vector<ObjectId> lights;
};

template<>
struct JsonObjectReflector<GroupParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<GroupParams>();
        builder.field("geometries", [](auto &object) { return &object.geometries; })
            .description("IDs of the geometric models to put in the group")
            .defaultValue(std::vector<ObjectId>());
        builder.field("clippingGeometries", [](auto &object) { return &object.clippingGeometries; })
            .description("IDs of the clipping geometric models to put in the group")
            .defaultValue(std::vector<ObjectId>());
        builder.field("volumes", [](auto &object) { return &object.volumes; })
            .description("IDs of the volumetric models to put in the group")
            .defaultValue(std::vector<ObjectId>());
        builder.field("lights", [](auto &object) { return &object.lights; })
            .description("IDs of the lights to put in the group")
            .defaultValue(std::vector<ObjectId>());
        return builder.build();
    }
};

struct GroupStorage
{
    std::vector<Stored<UserGeometricModel>> geometries;
    std::vector<Stored<UserGeometricModel>> clippingGeometries;
    std::vector<Stored<UserVolumetricModel>> volumes;
    std::vector<Stored<UserLight>> lights;
};

struct UserGroup
{
    GroupSettings settings;
    GroupStorage storage;
    Group value;
};

using CreateGroupParams = CreateParamsOf<GroupParams>;
using GroupResult = BoundedResultOf<GroupParams>;
using GetGroupResult = GetResultOf<GroupResult>;
using UpdateGroupParams = UpdateParamsOf<GroupParams>;

CreateObjectResult createGroup(ObjectManager &objects, Device &device, const CreateGroupParams &params);
GetGroupResult getGroup(ObjectManager &objects, const GetObjectParams &params);
void updateGroup(ObjectManager &objects, Device &device, const UpdateGroupParams &params);

struct InstanceParams
{
    InstanceSettings value;
    ObjectId group;
};

template<>
struct JsonObjectReflector<InstanceParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<InstanceParams>();
        builder.field("group", [](auto &object) { return &object.group; }).description("ID of group to instanciate");
        builder.field("transform", [](auto &object) { return &object.value.transform; }).description("Group transform").defaultValue(Transform());
        builder.field("id", [](auto &object) { return &object.value.id; })
            .description("Optional ID to retreive the model in framebuffer or pick result")
            .defaultValue(std::uint32_t(-1));
        return builder.build();
    }
};

struct UserInstance
{
    InstanceSettings settings;
    Instance value;
    Stored<UserGroup> group;
};

using CreateInstanceParams = CreateParamsOf<InstanceParams>;
using InstanceResult = BoundedResultOf<InstanceParams>;
using GetInstanceResult = GetResultOf<InstanceResult>;
using UpdateInstanceParams = UpdateParamsOf<InstanceParams>;

CreateObjectResult createInstance(ObjectManager &objects, Device &device, const CreateInstanceParams &params);
GetInstanceResult getInstance(ObjectManager &objects, const GetObjectParams &params);
void updateInstance(ObjectManager &objects, Device &device, const UpdateInstanceParams &params);

struct WorldParams
{
    std::vector<ObjectId> instances;
};

template<>
struct JsonObjectReflector<WorldParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<WorldParams>();
        builder.field("instances", [](auto &object) { return &object.instances; }).description("List of instances to put in the world");
        return builder.build();
    }
};

struct UserWorld
{
    WorldSettings settings;
    World value;
    std::vector<Stored<UserInstance>> instances;
};

using CreateWorldParams = CreateParamsOf<WorldParams>;
using WorldResult = BoundedResultOf<WorldParams>;
using GetWorldResult = GetResultOf<WorldResult>;
using UpdateWorldParams = UpdateParamsOf<WorldParams>;

CreateObjectResult createWorld(ObjectManager &objects, Device &device, const CreateWorldParams &params);
GetWorldResult getWorld(ObjectManager &objects, const GetObjectParams &params);
void updateWorld(ObjectManager &objects, Device &device, const UpdateWorldParams &params);
}

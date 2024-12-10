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

#include "SceneObjects.h"

#include "common/Objects.h"

namespace
{
using namespace brayns;

GroupParams getGroupParams(const GroupStorage &storage)
{
    return {
        .geometries = getObjectIds(storage.geometries),
        .clippingGeometries = getObjectIds(storage.clippingGeometries),
        .volumes = getObjectIds(storage.volumes),
        .lights = getObjectIds(storage.lights),
    };
}

GroupStorage getGroupStorage(ObjectManager &objects, const GroupParams &params)
{
    return {
        .geometries = getStoredObjects<UserGeometricModel>(objects, params.geometries),
        .clippingGeometries = getStoredObjects<UserGeometricModel>(objects, params.clippingGeometries),
        .volumes = getStoredObjects<UserVolumetricModel>(objects, params.volumes),
        .lights = getStoredObjects<UserLight>(objects, params.lights),
    };
}

GroupSettings getGroupSettings(const GroupStorage &storage)
{
    return {
        .geometries = mapObjects(storage.geometries, [](const auto &object) { return object.get().value; }),
        .clippingGeometries = mapObjects(storage.clippingGeometries, [](const auto &object) { return object.get().value; }),
        .volumes = mapObjects(storage.volumes, [](const auto &object) { return object.get().value; }),
        .lights = mapObjects(storage.lights, [](const auto &object) { return object.get().get(); }),
    };
}
}

namespace brayns
{
CreateObjectResult createGeometricModel(ObjectManager &objects, Device &device, CreateGeometricModelParams params)
{
    auto &[base, derived] = params;

    auto geometry = objects.getAsStored<UserGeometry>(derived.geometry);

    auto model = createGeometricModel(device, geometry.get().get(), derived.value);

    auto object = UserGeometricModel{std::move(geometry), std::move(derived.value), std::move(model)};
    auto stored = objects.add(std::move(object), {"GeometricModel"}, std::move(base));

    return getResult(stored);
}

GetGeometricModelResult getGeometricModel(ObjectManager &objects, const GetObjectParams &params)
{
    auto &model = objects.getAs<UserGeometricModel>(params.id);
    auto result = GeometricModelParams{model.settings, model.geometry.getId()};
    return getResult(std::move(result));
}

void updateGeometricModel(ObjectManager &objects, Device &device, const UpdateGeometricModelParams &params)
{
    auto &model = objects.getAs<UserGeometricModel>(params.id);

    auto current = GeometricModelParams{model.settings, model.geometry.getId()};
    auto updated = getUpdatedParams(params, std::move(current));

    auto geometry = objects.getAsStored<UserGeometry>(updated.geometry);

    model.value.update(geometry.get().get(), updated.value);
    device.throwIfError();

    model.settings = std::move(updated.value);
    model.geometry = std::move(geometry);
}

CreateObjectResult createVolumetricModel(ObjectManager &objects, Device &device, const CreateVolumetricModelParams &params)
{
    auto &[base, derived] = params;

    auto volume = objects.getAsStored<UserVolume>(derived.volume);
    auto transferFunction = objects.getAsStored<UserTransferFunction>(derived.transferFunction);

    auto model = createVolumetricModel(device, volume.get().get(), transferFunction.get().get(), derived.value);

    auto object = UserVolumetricModel{std::move(volume), std::move(transferFunction), std::move(derived.value), std::move(model)};
    auto stored = objects.add(std::move(object), {"VolumetricModel"}, std::move(base));

    return getResult(stored);
}

GetVolumetricModelResult getVolumetricModel(ObjectManager &objects, const GetObjectParams &params)
{
    auto &model = objects.getAs<UserVolumetricModel>(params.id);
    auto result = VolumetricModelParams{model.settings, model.volume.getId(), model.transferFunction.getId()};
    return getResult(std::move(result));
}

void updateVolumetricModel(ObjectManager &objects, Device &device, const UpdateVolumetricModelParams &params)
{
    auto &model = objects.getAs<UserVolumetricModel>(params.id);

    auto current = VolumetricModelParams{model.settings, model.volume.getId(), model.transferFunction.getId()};
    auto updated = getUpdatedParams(params, std::move(current));

    auto volume = objects.getAsStored<UserVolume>(updated.volume);
    auto transferFunction = objects.getAsStored<UserTransferFunction>(updated.transferFunction);

    model.value.update(volume.get().get(), transferFunction.get().get(), updated.value);
    device.throwIfError();

    model.settings = std::move(updated.value);
    model.volume = std::move(volume);
    model.transferFunction = std::move(transferFunction);
}

CreateObjectResult createGroup(ObjectManager &objects, Device &device, const CreateGroupParams &params)
{
    auto &[base, derived] = params;

    auto storage = getGroupStorage(objects, derived);
    auto settings = getGroupSettings(storage);

    auto group = createGroup(device, settings);

    auto object = UserGroup{std::move(storage), std::move(settings), std::move(group)};
    auto stored = objects.add(std::move(object), {"Group"}, std::move(base));

    return getResult(stored);
}

GetGroupResult getGroup(ObjectManager &objects, const GetObjectParams &params)
{
    auto &group = objects.getAs<UserGroup>(params.id);
    auto result = GroupResult{getGroupParams(group.storage), group.value.getBounds()};
    return getResult(std::move(result));
}

void updateGroup(ObjectManager &objects, Device &device, const UpdateGroupParams &params)
{
    auto &group = objects.getAs<UserGroup>(params.id);

    auto updated = getUpdatedParams(params, getGroupParams(group.storage));
    auto storage = getGroupStorage(objects, updated);
    auto settings = getGroupSettings(storage);

    group.value.update(settings);
    device.throwIfError();

    group.settings = std::move(settings);
    group.storage = std::move(storage);
}

CreateObjectResult createInstance(ObjectManager &objects, Device &device, const CreateInstanceParams &params)
{
    auto &[base, derived] = params;

    auto group = objects.getAsStored<UserGroup>(derived.group);

    auto instance = createInstance(device, group.get().value, derived.value);

    auto object = UserInstance{std::move(group), derived.value, std::move(instance)};
    auto stored = objects.add(std::move(object), {"Instance"}, std::move(base));

    return getResult(stored);
}

GetInstanceResult getInstance(ObjectManager &objects, const GetObjectParams &params)
{
    auto &instance = objects.getAs<UserInstance>(params.id);
    auto result = InstanceResult{{instance.settings, instance.group.getId()}, instance.value.getBounds()};
    return getResult(std::move(result));
}

void updateInstance(ObjectManager &objects, Device &device, const UpdateInstanceParams &params)
{
    auto &instance = objects.getAs<UserInstance>(params.id);

    auto current = InstanceParams{instance.settings, instance.group.getId()};
    auto updated = getUpdatedParams(params, std::move(current));

    auto group = objects.getAsStored<UserGroup>(updated.group);

    instance.value.update(group.get().value, updated.value);
    device.throwIfError();

    instance.settings = std::move(updated.value);
    instance.group = std::move(group);
}

CreateObjectResult createWorld(ObjectManager &objects, Device &device, const CreateWorldParams &params)
{
    auto &[base, derived] = params;

    auto instances = getStoredObjects<UserInstance>(objects, derived.instances);
    auto settings = WorldSettings{mapObjects(instances, [](const auto &object) { return object.get().value; })};

    auto world = createWorld(device, settings);

    auto object = UserWorld{std::move(instances), std::move(settings), std::move(world)};
    auto stored = objects.add(std::move(object), {"World"}, std::move(base));

    return getResult(stored);
}

GetWorldResult getWorld(ObjectManager &objects, const GetObjectParams &params)
{
    auto &world = objects.getAs<UserWorld>(params.id);
    auto instances = getObjectIds(world.instances);
    auto result = WorldResult{{std::move(instances)}, world.value.getBounds()};
    return getResult(std::move(result));
}

void updateWorld(ObjectManager &objects, Device &device, const UpdateWorldParams &params)
{
    auto &world = objects.getAs<UserWorld>(params.id);

    auto current = WorldParams{getObjectIds(world.instances)};
    auto updated = getUpdatedParams(params, std::move(current));

    auto instances = getStoredObjects<UserInstance>(objects, updated.instances);
    auto settings = WorldSettings{mapObjects(instances, [](const auto &object) { return object.get().value; })};

    world.value.update(settings);
    device.throwIfError();

    world.settings = std::move(settings);
    world.instances = std::move(instances);
}
}

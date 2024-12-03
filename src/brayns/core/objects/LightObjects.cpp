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

#include "LightObjects.h"

namespace
{
using namespace brayns;

template<typename T>
auto createLightAs(ObjectManager &objects, const auto &params, std::string type, auto &&create)
{
    const auto &[objectParams, lightParams] = params;
    const auto &[base, derived] = lightParams;

    auto light = create(base, derived);

    auto ptr = toShared(T{derived, std::move(light)});

    auto object = UserLight{
        .settings = base,
        .value = ptr,
        .get = [=] { return ptr->value; },
    };

    auto stored = objects.add(std::move(object), {std::move(type)}, objectParams);

    return getResult(stored);
}

template<typename T>
auto getLightAs(ObjectManager &objects, const GetObjectParams &params)
{
    auto object = objects.getAsStored<UserLight>(params.id);
    auto &light = *castAsShared<T>(object.get().value, object);
    return getResult(light.storage);
}

template<typename T, typename U>
void updateLightAs(ObjectManager &objects, Device &device, const UpdateParamsOf<U> &params)
{
    auto object = objects.getAsStored<UserLight>(params.id);
    auto &light = *castAsShared<T>(object.get().value, object);

    auto settings = getUpdatedParams(params, light.storage);

    light.value.update(settings);
    device.throwIfError();

    light.storage = settings;
}
}

namespace brayns
{
GetLightResult getLight(ObjectManager &objects, const GetObjectParams &params)
{
    auto &light = objects.getAs<UserLight>(params.id);
    return getResult(light.settings);
}

void updateLight(ObjectManager &objects, Device &device, const UpdateLightParams &params)
{
    auto &light = objects.getAs<UserLight>(params.id);

    auto settings = getUpdatedParams(params, light.settings);

    light.get().update(settings);
    device.throwIfError();

    light.settings = settings;
}

CreateObjectResult createDistantLight(ObjectManager &objects, Device &device, const CreateDistantLightParams &params)
{
    auto create = [&](const auto &base, const auto &derived) { return createDistantLight(device, base, derived); };
    return createLightAs<UserDistantLight>(objects, params, "DistantLight", create);
}

GetDistantLightResult getDistantLight(ObjectManager &objects, const GetObjectParams &params)
{
    return getLightAs<UserDistantLight>(objects, params);
}

void updateDistantLight(ObjectManager &objects, Device &device, const UpdateDistantLightParams &params)
{
    updateLightAs<UserDistantLight>(objects, device, params);
}

CreateObjectResult createSphereLight(ObjectManager &objects, Device &device, const CreateSphereLightParams &params)
{
    auto create = [&](const auto &base, const auto &derived) { return createSphereLight(device, base, derived); };
    return createLightAs<UserSphereLight>(objects, params, "SphereLight", create);
}

GetSphereLightResult getSphereLight(ObjectManager &objects, const GetObjectParams &params)
{
    return getLightAs<UserSphereLight>(objects, params);
}

void updateSphereLight(ObjectManager &objects, Device &device, const UpdateSphereLightParams &params)
{
    updateLightAs<UserSphereLight>(objects, device, params);
}

CreateObjectResult createSpotLight(ObjectManager &objects, Device &device, const CreateSpotLightParams &params)
{
    auto create = [&](const auto &base, const auto &derived) { return createSpotLight(device, base, derived); };
    return createLightAs<UserSpotLight>(objects, params, "SpotLight", create);
}

GetSpotLightResult getSpotLight(ObjectManager &objects, const GetObjectParams &params)
{
    return getLightAs<UserSpotLight>(objects, params);
}

void updateSpotLight(ObjectManager &objects, Device &device, const UpdateSpotLightParams &params)
{
    updateLightAs<UserSpotLight>(objects, device, params);
}

CreateObjectResult createQuadLight(ObjectManager &objects, Device &device, const CreateQuadLightParams &params)
{
    auto create = [&](const auto &base, const auto &derived) { return createQuadLight(device, base, derived); };
    return createLightAs<UserQuadLight>(objects, params, "QuadLight", create);
}

GetQuadLightResult getQuadLight(ObjectManager &objects, const GetObjectParams &params)
{
    return getLightAs<UserQuadLight>(objects, params);
}

void updateQuadLight(ObjectManager &objects, Device &device, const UpdateQuadLightParams &params)
{
    updateLightAs<UserQuadLight>(objects, device, params);
}

CreateObjectResult createCylinderLight(ObjectManager &objects, Device &device, const CreateCylinderLightParams &params)
{
    auto create = [&](const auto &base, const auto &derived) { return createCylinderLight(device, base, derived); };
    return createLightAs<UserCylinderLight>(objects, params, "CylinderLight", create);
}

GetCylinderLightResult getCylinderLight(ObjectManager &objects, const GetObjectParams &params)
{
    return getLightAs<UserCylinderLight>(objects, params);
}

void updateCylinderLight(ObjectManager &objects, Device &device, const UpdateCylinderLightParams &params)
{
    updateLightAs<UserCylinderLight>(objects, device, params);
}

CreateObjectResult createHdriLight(ObjectManager &objects, Device &device, const CreateHdriLightParams &params)
{
    const auto &[objectParams, lightParams] = params;
    const auto &[base, derived] = lightParams;

    auto texture = objects.getAsStored<UserTexture>(derived.map);
    auto &map = castTextureAs<UserTexture2D>(texture);

    auto light = createHdriLight(device, map.value, base, derived.value);

    auto ptr = toShared(UserHdriLight{{std::move(derived.value), std::move(texture)}, std::move(light)});

    auto object = UserLight{
        .settings = base,
        .value = ptr,
        .get = [=] { return ptr->value; },
    };

    auto stored = objects.add(std::move(object), {std::move("HdriLight")}, objectParams);

    return getResult(stored);
}

GetHdriLightResult getHdriLight(ObjectManager &objects, const GetObjectParams &params)
{
    auto object = objects.getAsStored<UserLight>(params.id);
    auto &light = *castAsShared<UserHdriLight>(object.get().value, object);
    auto settings = HdriLightParams{light.storage.settings, light.storage.map.getId()};
    return getResult(std::move(settings));
}

void updateHdriLight(ObjectManager &objects, Device &device, const UpdateHdriLightParams &params)
{
    auto object = objects.getAsStored<UserLight>(params.id);
    auto &light = *castAsShared<UserHdriLight>(object.get().value, object);

    auto current = HdriLightParams{light.storage.settings, light.storage.map.getId()};

    auto updated = getUpdatedParams(params, std::move(current));

    auto texture = objects.getAsStored<UserTexture>(updated.map);
    auto &map = castTextureAs<UserTexture2D>(texture);

    light.value.update(map.value, updated.value);
    device.throwIfError();

    light.storage = {std::move(updated.value), std::move(texture)};
}

CreateObjectResult createAmbientLight(ObjectManager &objects, Device &device, const CreateAmbientLightParams &params)
{
    auto create = [&](const auto &base, const auto &derived) { return createAmbientLight(device, base, derived); };
    return createLightAs<UserAmbientLight>(objects, params, "AmbientLight", create);
}

GetAmbientLightResult getAmbientLight(ObjectManager &objects, const GetObjectParams &params)
{
    return getLightAs<UserAmbientLight>(objects, params);
}

void updateAmbientLight(ObjectManager &objects, Device &device, const UpdateAmbientLightParams &params)
{
    updateLightAs<UserAmbientLight>(objects, device, params);
}

CreateObjectResult createSunSkyLight(ObjectManager &objects, Device &device, const CreateSunSkyLightParams &params)
{
    auto create = [&](const auto &base, const auto &derived) { return createSunSkyLight(device, base, derived); };
    return createLightAs<UserSunSkyLight>(objects, params, "SunSkyLight", create);
}

GetSunSkyLightResult getSunSkyLight(ObjectManager &objects, const GetObjectParams &params)
{
    return getLightAs<UserSunSkyLight>(objects, params);
}

void updateSunSkyLight(ObjectManager &objects, Device &device, const UpdateSunSkyLightParams &params)
{
    updateLightAs<UserSunSkyLight>(objects, device, params);
}
}

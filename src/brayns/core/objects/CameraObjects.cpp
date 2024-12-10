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

#include "CameraObjects.h"

namespace
{
using namespace brayns;

template<typename T>
auto createCameraAs(ObjectManager &objects, const auto &params, std::string type, auto &&create)
{
    const auto &[objectParams, cameraParams] = params;
    const auto &[base, derived] = cameraParams;

    auto camera = create(base, derived);

    auto ptr = toShared(T{derived, std::move(camera)});

    auto object = UserCamera{
        .settings = base,
        .value = ptr,
        .get = [=] { return ptr->value; },
    };

    auto stored = objects.add(std::move(object), {std::move(type)}, objectParams);

    return getResult(stored);
}

template<typename T>
auto getCameraAs(ObjectManager &objects, const GetObjectParams &params)
{
    auto object = objects.getAsStored<UserCamera>(params.id);
    auto &camera = *castAsShared<T>(object.get().value, object);
    return getResult(camera.settings);
}

template<typename T, typename U>
void updateCameraAs(ObjectManager &objects, Device &device, const UpdateParamsOf<U> &params)
{
    auto object = objects.getAsStored<UserCamera>(params.id);
    auto &camera = *castAsShared<T>(object.get().value, object);

    auto settings = getUpdatedParams(params, camera.settings);

    camera.value.update(settings);
    device.throwIfError();

    camera.settings = settings;
}
}

namespace brayns
{
GetCameraResult getCamera(ObjectManager &objects, const GetObjectParams &params)
{
    auto &camera = objects.getAs<UserCamera>(params.id);
    return getResult(camera.settings);
}

void updateCamera(ObjectManager &objects, Device &device, const UpdateCameraParams &params)
{
    auto &camera = objects.getAs<UserCamera>(params.id);

    auto settings = getUpdatedParams(params, camera.settings);

    camera.get().update(settings);
    device.throwIfError();

    camera.settings = settings;
}

CreateObjectResult createPerspectiveCamera(ObjectManager &objects, Device &device, const CreatePerspectiveCameraParams &params)
{
    auto create = [&](const auto &base, const auto &derived) { return createPerspectiveCamera(device, base, derived); };
    return createCameraAs<UserPerspectiveCamera>(objects, params, "PerspectiveCamera", create);
}

GetPerspectiveCameraResult getPerspectiveCamera(ObjectManager &objects, const GetObjectParams &params)
{
    return getCameraAs<UserPerspectiveCamera>(objects, params);
}

void updatePerspectiveCamera(ObjectManager &objects, Device &device, const UpdatePerspectiveCameraParams &params)
{
    updateCameraAs<UserPerspectiveCamera>(objects, device, params);
}

CreateObjectResult createOrthographicCamera(ObjectManager &objects, Device &device, const CreateOrthographicCameraParams &params)
{
    auto create = [&](const auto &base, const auto &derived) { return createOrthographicCamera(device, base, derived); };
    return createCameraAs<UserOrthographicCamera>(objects, params, "OrthographicCamera", create);
}

GetOrthographicCameraResult getOrthographicCamera(ObjectManager &objects, const GetObjectParams &params)
{
    return getCameraAs<UserOrthographicCamera>(objects, params);
}

void updateOrthographicCamera(ObjectManager &objects, Device &device, const UpdateOrthographicCameraParams &params)
{
    updateCameraAs<UserOrthographicCamera>(objects, device, params);
}

CreateObjectResult createPanoramicCamera(ObjectManager &objects, Device &device, const CreatePanoramicCameraParams &params)
{
    auto create = [&](const auto &base, const auto &derived) { return createPanoramicCamera(device, base, derived); };
    return createCameraAs<UserPanoramicCamera>(objects, params, "PanoramicCamera", create);
}

GetPanoramicCameraResult getPanoramicCamera(ObjectManager &objects, const GetObjectParams &params)
{
    return getCameraAs<UserPanoramicCamera>(objects, params);
}

void updatePanoramicCamera(ObjectManager &objects, Device &device, const UpdatePanoramicCameraParams &params)
{
    updateCameraAs<UserPanoramicCamera>(objects, device, params);
}
}

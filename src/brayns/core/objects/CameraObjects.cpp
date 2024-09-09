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
auto getCameraAs(ObjectRegistry &objects, const ObjectParams &params)
{
    auto stored = objects.getAsStored<UserCamera>(params.id);
    auto &derived = castAs<T>(stored.get().value, stored.getInfo());
    return derived.settings;
}

template<typename T, typename U>
auto updateCameraAs(ObjectRegistry &objects, const UpdateParams<U> &params)
{
    auto stored = objects.getAsStored<UserCamera>(params.id);
    auto &base = stored.get();
    auto &derived = castAs<T>(base.value, stored.getInfo());
    auto &device = base.device.get();

    derived.value.update(params.settings);
    device.throwIfError();

    derived.settings = params.settings;
}
}

namespace brayns
{
CameraInfo getCamera(ObjectRegistry &objects, const ObjectParams &params)
{
    auto &camera = objects.getAs<UserCamera>(params.id);
    return camera.settings;
}

void updateCamera(ObjectRegistry &objects, const CameraUpdate &params)
{
    auto &object = objects.getAs<UserCamera>(params.id);
    auto &device = object.device.get();
    auto camera = object.get();

    camera.update(params.settings);
    device.throwIfError();

    object.settings = params.settings;
}

ObjectResult createPerspectiveCamera(ObjectRegistry &objects, Device &device, const PerspectiveCameraParams &params)
{
    auto camera = createPerspectiveCamera(device, params.base, params.derived);

    auto derived = UserPerspectiveCamera{params.derived, std::move(camera)};
    auto ptr = std::make_shared<decltype(derived)>(std::move(derived));

    auto object = UserCamera{
        .device = device,
        .settings = params.base,
        .value = ptr,
        .get = [=] { return ptr->value; },
        .setAspect = [=](auto value) { ptr->value.setAspect(value); },
    };

    auto stored = objects.add(std::move(object), "PerspectiveCamera");

    return {stored.getId()};
}

PerspectiveCameraInfo getPerspectiveCamera(ObjectRegistry &objects, const ObjectParams &params)
{
    return getCameraAs<UserPerspectiveCamera>(objects, params);
}

void updatePerspectiveCamera(ObjectRegistry &objects, const PerspectiveCameraUpdate &params)
{
    updateCameraAs<UserPerspectiveCamera>(objects, params);
}

ObjectResult createOrthographicCamera(ObjectRegistry &objects, Device &device, const OrthographicCameraParams &params)
{
    auto camera = createOrthographicCamera(device, params.base, params.derived);

    auto derived = UserOrthographicCamera{params.derived, std::move(camera)};
    auto ptr = std::make_shared<decltype(derived)>(std::move(derived));

    auto object = UserCamera{
        .device = device,
        .settings = params.base,
        .value = ptr,
        .get = [=] { return ptr->value; },
        .setAspect = [=](auto value) { ptr->value.setAspect(value); },
    };

    auto stored = objects.add(std::move(object), "OrthographicCamera");

    return {stored.getId()};
}

OrthographicCameraInfo getOrthographicCamera(ObjectRegistry &objects, const ObjectParams &params)
{
    return getCameraAs<UserOrthographicCamera>(objects, params);
}

void updateOrthographicCamera(ObjectRegistry &objects, const OrthographicCameraUpdate &params)
{
    updateCameraAs<UserOrthographicCamera>(objects, params);
}

ObjectResult createPanoramicCamera(ObjectRegistry &objects, Device &device, const PanoramicCameraParams &params)
{
    auto camera = createPanoramicCamera(device, params.base, params.derived);

    auto derived = UserPanoramicCamera{params.derived, std::move(camera)};
    auto ptr = std::make_shared<decltype(derived)>(std::move(derived));

    auto object = UserCamera{
        .device = device,
        .settings = params.base,
        .value = ptr,
        .get = [=] { return ptr->value; },
        .setAspect = [](auto) {},
    };

    auto stored = objects.add(std::move(object), "PanoramicCamera");

    return {stored.getId()};
}

PanoramicCameraInfo getPanoramicCamera(ObjectRegistry &objects, const ObjectParams &params)
{
    return getCameraAs<UserPanoramicCamera>(objects, params);
}

void updatePanoramicCamera(ObjectRegistry &objects, const PanoramicCameraUpdate &params)
{
    updateCameraAs<UserPanoramicCamera>(objects, params);
}
}

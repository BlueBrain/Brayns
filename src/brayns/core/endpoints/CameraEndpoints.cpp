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

#include "CameraEndpoints.h"

#include <any>
#include <functional>

namespace brayns
{
template<>
struct JsonObjectReflector<CameraSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<CameraSettings>();
        builder.field("position", [](auto &object) { return &object.position; }).description("Camera position XYZ");
        builder.field("direction", [](auto &object) { return &object.direction; })
            .description("Camera forward direction XYZ");
        builder.field("up", [](auto &object) { return &object.up; })
            .description("Camera up direction XYZ")
            .defaultValue(Vector3(0.0F, 1.0F, 0.0F));
        builder.field("near_clip", [](auto &object) { return &object.nearClip; })
            .description("Distance to clip objects that are too close to the camera")
            .defaultValue(0.0F);
        return builder.build();
    }
};

template<ReflectedJson T>
struct CameraParams
{
    CameraSettings base;
    T derived;
};

template<ReflectedJson T>
struct JsonObjectReflector<CameraParams<T>>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<CameraParams<T>>();
        builder.field("base", [](auto &object) { return &object.base; })
            .description("Camera settings common to all camera types");
        builder.field("derived", [](auto &object) { return &object.derived; })
            .description("Camera settings specific to the given camera type");
        return builder.build();
    }
};

template<typename T>
struct CameraReflector;

template<std::derived_from<Camera> T>
using GetCameraSettings = typename CameraReflector<T>::Settings;

template<std::derived_from<Camera> T>
using CameraParamsOf = CameraParams<GetCameraSettings<T>>;

template<std::derived_from<Camera> T>
using CameraUpdateOf = UpdateParams<GetCameraSettings<T>>;

using CameraUpdate = UpdateParams<CameraSettings>;

template<typename T>
concept ReflectedCamera =
    ReflectedJson<GetCameraSettings<T>> && std::same_as<std::string, decltype(CameraReflector<T>::getType())>
    && std::same_as<T, decltype(CameraReflector<T>::create(std::declval<Device &>(), CameraParamsOf<T>()))>
    && std::is_void_v<decltype(CameraReflector<T>::setAspect(std::declval<T &>(), 0.0F))>;

template<ReflectedCamera T>
std::string getCameraType()
{
    return CameraReflector<T>::getType();
}

template<ReflectedCamera T>
T createCamera(Device &device, const CameraParamsOf<T> &params)
{
    return CameraReflector<T>::create(device, params);
}

template<ReflectedCamera T>
void setCameraAspect(T &camera, float aspect)
{
    CameraReflector<T>::setAspect(camera, aspect);
}

template<ReflectedCamera T>
struct UserCamera
{
    T deviceObject;
    CameraParamsOf<T> params;
};

struct CameraInterface
{
    std::any value;
    std::function<std::string()> getType;
    std::function<Camera()> getDeviceObject;
    std::function<CameraSettings()> getSettings;
    std::function<void(const CameraSettings &)> update;
    std::function<void(float)> setAspect;
};

template<>
struct ObjectReflector<CameraInterface>
{
    static std::string getType(const CameraInterface &camera)
    {
        return camera.getType();
    }
};

template<ReflectedCamera T>
CameraInterface createCameraInterface(const std::shared_ptr<UserCamera<T>> &camera)
{
    return {
        .value = camera,
        .getType = [] { return getCameraType<T>(); },
        .getDeviceObject = [=] { return camera->deviceObject; },
        .getSettings = [=] { return camera->params.base; },
        .update = [=](const auto &settings) { camera->params.base = settings; },
        .setAspect = [=](auto aspect) { setCameraAspect(camera->deviceObject, aspect); },
    };
}

template<ReflectedCamera T>
UserCamera<T> &castCamera(CameraInterface &camera)
{
    auto ptr = std::any_cast<std::shared_ptr<UserCamera<T>>>(&camera.value);

    if (ptr != nullptr)
    {
        return **ptr;
    }

    auto expected = getCameraType<T>();
    auto got = camera.getType();

    throw InvalidParams(fmt::format("Invalid camera type: expected {}, got {}", expected, got));
}

template<ReflectedCamera T>
ObjectResult createUserCamera(LockedObjects &locked, Device &device, const CameraParamsOf<T> &params)
{
    return locked.visit(
        [&](ObjectManager &objects)
        {
            auto camera = createCamera<T>(device, params);
            auto object = UserCamera<T>{camera, params};
            auto ptr = std::make_shared<decltype(object)>(std::move(object));

            auto interface = createCameraInterface(ptr);

            auto stored = objects.add(std::move(interface));

            return stored.getResult();
        });
}

CameraSettings getCameraSettings(LockedObjects &locked, const ObjectParams &params)
{
    return locked.visit(
        [&](ObjectManager &objects)
        {
            auto &camera = objects.get<CameraInterface>(params.id);

            return camera.getSettings();
        });
}

template<ReflectedCamera T>
GetCameraSettings<T> getDerivedCamera(LockedObjects &locked, const ObjectParams &params)
{
    return locked.visit(
        [&](ObjectManager &objects)
        {
            auto &interface = objects.get<CameraInterface>(params.id);
            auto &camera = castCamera<T>(interface);

            return camera.params.derived;
        });
}

void updateCamera(LockedObjects &locked, const CameraUpdate &params)
{
    locked.visit(
        [&](ObjectManager &objects)
        {
            auto &camera = objects.get<CameraInterface>(params.id);

            camera.update(params.properties);
        });
}

template<ReflectedCamera T>
void updateDerivedCamera(LockedObjects &locked, const CameraUpdateOf<T> &params)
{
    locked.visit(
        [&](ObjectManager &objects)
        {
            auto &interface = objects.get<CameraInterface>(params.id);
            auto &camera = castCamera<T>(interface);

            camera.params.derived = params.properties;
        });
}

template<ReflectedCamera T>
void addCameraType(ApiBuilder &builder, LockedObjects &objects, Device &device)
{
    auto type = getCameraType<T>();

    builder
        .endpoint(
            "create-" + type,
            [&](CameraParamsOf<T> params) { return createUserCamera<T>(objects, device, params); })
        .description("Create a camera of type " + type);

    builder.endpoint("get-" + type, [&](ObjectParams params) { return getDerivedCamera<T>(objects, params); })
        .description("Get derived properties of a " + type);

    builder.endpoint("update-" + type, [&](CameraUpdateOf<T> params) { updateDerivedCamera<T>(objects, params); })
        .description("Update derived properties of a " + type);
}

template<>
struct JsonObjectReflector<PerspectiveSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<PerspectiveSettings>();
        builder.field("fovy", [](auto &object) { return &object.fovy; })
            .description("Camera vertical field of view in degrees (horizontal is deduced from framebuffer aspect)")
            .defaultValue(45.0F);
        return builder.build();
    }
};

template<>
struct CameraReflector<PerspectiveCamera>
{
    using Settings = PerspectiveSettings;

    static std::string getType()
    {
        return "perspective-camera";
    }

    static PerspectiveCamera create(Device &device, const CameraParamsOf<PerspectiveCamera> &params)
    {
        return createPerspectiveCamera(device, params.base, params.derived);
    }

    static void setAspect(PerspectiveCamera &camera, float aspect)
    {
        camera.setAspect(aspect);
    }
};

template<>
struct JsonObjectReflector<OrthographicSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<OrthographicSettings>();
        builder.field("height", [](auto &object) { return &object.height; })
            .description("Camera viewport height in world coordinates (horizontal is deduced from framebuffer aspect)")
            .defaultValue(1.0F);
        return builder.build();
    }
};

template<>
struct CameraReflector<OrthographicCamera>
{
    using Settings = OrthographicSettings;

    static std::string getType()
    {
        return "orthographic-camera";
    }

    static OrthographicCamera create(Device &device, const CameraParamsOf<OrthographicCamera> &params)
    {
        return createOrthographicCamera(device, params.base, params.derived);
    }

    static void setAspect(OrthographicCamera &camera, float aspect)
    {
        camera.setAspect(aspect);
    }
};

void addCameraEndpoints(ApiBuilder &builder, LockedObjects &objects, Device &device)
{
    addCameraType<PerspectiveCamera>(builder, objects, device);
    addCameraType<OrthographicCamera>(builder, objects, device);

    builder.endpoint("get-camera", [&](ObjectParams params) { return getCameraSettings(objects, params); })
        .description("Get the base properties of a camera of any type");

    builder.endpoint("update-camera", [&](CameraUpdate params) { return updateCamera(objects, params); })
        .description("Update the base properties of a camera of any type");
}
}

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
        builder.field("image_region", [](auto &object) { return &object.imageRegion; })
            .description("Normalized region of the camera to be rendered (does not affect framebuffer resolution)")
            .defaultValue(Box2{{0.0F, 0.0F}, {1.0F, 1.0F}});
        return builder.build();
    }
};

void validateCameraSettings(const CameraSettings &settings)
{
    auto right = cross(settings.direction, settings.up);

    if (right.x == 0.0F && right.y == 0.0F && right.z == 0.0F)
    {
        throw InvalidParams("Camera up and direction are colinear");
    }

    if (settings.nearClip < 0.0F)
    {
        throw InvalidParams("Camera near clip must be positive");
    }
}

template<typename T>
struct CameraReflector;

template<std::derived_from<Camera> T>
using GetCameraSettings = typename CameraReflector<T>::Settings;

template<std::derived_from<Camera> T>
using CameraParams = ComposedParams<CameraSettings, GetCameraSettings<T>>;

template<std::derived_from<Camera> T>
using CameraUpdateOf = UpdateParams<GetCameraSettings<T>>;

using CameraUpdate = UpdateParams<CameraSettings>;

template<typename T>
concept ReflectedCamera =
    ReflectedJson<GetCameraSettings<T>> && std::same_as<std::string, decltype(CameraReflector<T>::getType())>
    && std::same_as<T, decltype(CameraReflector<T>::create(std::declval<Device &>(), CameraParams<T>()))>
    && std::is_void_v<decltype(CameraReflector<T>::update(std::declval<T &>(), GetCameraSettings<T>()))>
    && std::is_void_v<decltype(CameraReflector<T>::setAspect(std::declval<T &>(), 0.0F))>;

template<ReflectedCamera T>
std::string getCameraType()
{
    return CameraReflector<T>::getType();
}

template<ReflectedCamera T>
T createCamera(Device &device, const CameraParams<T> &params)
{
    return CameraReflector<T>::create(device, params);
}

template<ReflectedCamera T>
void updateCamera(T &camera, const GetCameraSettings<T> &settings)
{
    CameraReflector<T>::update(camera, settings);
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
    CameraParams<T> params;
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
UserCamera<T> &getCamera(ObjectManager &objects, ObjectId id)
{
    auto &interface = objects.get<CameraInterface>(id);
    auto *ptr = std::any_cast<std::shared_ptr<UserCamera<T>>>(&interface.value);

    if (ptr != nullptr)
    {
        return **ptr;
    }

    auto type = interface.getType();
    throw InvalidParams(fmt::format("Invalid camera type for object {}: {}", id, type));
}

template<ReflectedCamera T>
ObjectResult addCamera(LockedObjects &locked, Device &device, const CameraParams<T> &params)
{
    validateCameraSettings(params.base);

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
GetCameraSettings<T> getCameraAs(LockedObjects &locked, const ObjectParams &params)
{
    return locked.visit(
        [&](ObjectManager &objects)
        {
            auto &camera = getCamera<T>(objects, params.id);

            return camera.params.derived;
        });
}

void updateCameraSettings(LockedObjects &locked, Device &device, const CameraUpdate &params)
{
    validateCameraSettings(params.properties);

    locked.visit(
        [&](ObjectManager &objects)
        {
            auto &camera = objects.get<CameraInterface>(params.id);
            auto deviceObject = camera.getDeviceObject();

            deviceObject.update(params.properties);
            device.throwIfError();

            camera.update(params.properties);
        });
}

template<ReflectedCamera T>
void updateCameraAs(LockedObjects &locked, Device &device, const CameraUpdateOf<T> &params)
{
    locked.visit(
        [&](ObjectManager &objects)
        {
            auto &camera = getCamera<T>(objects, params.id);

            updateCamera(camera.deviceObject, params.properties);
            device.throwIfError();

            camera.params.derived = params.properties;
        });
}

template<ReflectedCamera T>
void addCameraType(ApiBuilder &builder, LockedObjects &objects, Device &device)
{
    auto type = getCameraType<T>();

    builder.endpoint("create-" + type, [&](CameraParams<T> params) { return addCamera<T>(objects, device, params); })
        .description("Create a camera of type " + type);

    builder.endpoint("get-" + type, [&](ObjectParams params) { return getCameraAs<T>(objects, params); })
        .description("Get derived properties of a camera of type " + type);

    builder.endpoint("update-" + type, [&](CameraUpdateOf<T> params) { updateCameraAs<T>(objects, device, params); })
        .description("Update derived properties of a camera of type " + type);
}

template<>
struct JsonObjectReflector<DepthOfField>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<DepthOfField>();
        builder.field("aperture_radius", [](auto &object) { return &object.apertureRadius; })
            .description("Size of the aperture radius (0 is no depth of field)")
            .defaultValue(0.0F);
        builder.field("focus_distance", [](auto &object) { return &object.focusDistance; })
            .description("Distance at which the image is the sharpest")
            .defaultValue(1.0F);
        return builder.build();
    }
};

template<>
struct EnumReflector<StereoMode>
{
    static auto reflect()
    {
        auto builder = EnumBuilder<StereoMode>();
        builder.field("none", StereoMode::None).description("Disable stereo");
        builder.field("left", StereoMode::Left).description("Render left eye");
        builder.field("right", StereoMode::Right).description("Render right eye");
        builder.field("side_by_side", StereoMode::SideBySide).description("Render both eyes side by side");
        builder.field("top_bottom", StereoMode::TopBottom).description("Render left eye above right eye");
        return builder.build();
    }
};

template<>
struct JsonObjectReflector<Stereo>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<Stereo>();
        builder.field("mode", [](auto &object) { return &object.mode; })
            .description("Size of the aperture radius (0 is no depth of field)")
            .defaultValue(StereoMode::None);
        builder.field("interpupillary_distance", [](auto &object) { return &object.interpupillaryDistance; })
            .description("Distance between observer eyes")
            .defaultValue(0.0635F);
        return builder.build();
    }
};

template<>
struct JsonObjectReflector<PerspectiveSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<PerspectiveSettings>();
        builder.field("fovy", [](auto &object) { return &object.fovy; })
            .description("Camera vertical field of view in degrees (horizontal is deduced from framebuffer aspect)")
            .defaultValue(45.0F);
        builder.field("depth_of_field", [](auto &object) { return &object.depthOfField; })
            .description("Depth of field settings, set to null to disable it");
        builder.field("architectural", [](auto &object) { return &object.architectural; })
            .description("Vertical edges are projected to be parallel")
            .defaultValue(false);
        builder.field("stereo", [](auto &object) { return &object.stereo; })
            .description("Stereo settings, set to null to disable it");
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

    static PerspectiveCamera create(Device &device, const CameraParams<PerspectiveCamera> &params)
    {
        return createPerspectiveCamera(device, params.base, params.derived);
    }

    static void update(PerspectiveCamera &camera, const PerspectiveSettings &settings)
    {
        camera.update(settings);
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

    static OrthographicCamera create(Device &device, const CameraParams<OrthographicCamera> &params)
    {
        return createOrthographicCamera(device, params.base, params.derived);
    }

    static void update(OrthographicCamera &camera, const OrthographicSettings &settings)
    {
        camera.update(settings);
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

    builder
        .endpoint("update-camera", [&](CameraUpdate params) { return updateCameraSettings(objects, device, params); })
        .description("Update the base properties of a camera of any type");
}
}

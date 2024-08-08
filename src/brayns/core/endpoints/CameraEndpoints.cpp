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
struct JsonObjectReflector<CameraView>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<CameraView>();
        builder.field("position", [](auto &object) { return &object.position; }).description("Camera position XYZ");
        builder.field("direction", [](auto &object) { return &object.direction; })
            .description("Camera forward direction XYZ");
        builder.field("up", [](auto &object) { return &object.direction; })
            .description("Camera up direction XYZ")
            .defaultValue(Vector3(0.0F, 1.0F, 0.0F));
        builder.field("near_clipping_distance", [](auto &object) { return &object.nearClippingDistance; })
            .description("Distance to clip objects that are too close to the camera")
            .defaultValue(0.0F);
        return builder.build();
    }
};

template<ReflectedJson T>
struct CameraParams
{
    CameraView view;
    T projection;
};

template<ReflectedJson T>
struct JsonObjectReflector<CameraParams<T>>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<CameraParams<T>>();
        builder.field("view", [](auto &object) { return &object.view; })
            .description("Camera view (common to all camera types)");
        builder.field("projection", [](auto &object) { return &object.projection; })
            .description("Camera projection (specific to each camera type)");
        return builder.build();
    }
};

template<typename T>
struct CameraReflector;

template<std::derived_from<Camera> T>
using GetProjection = typename CameraReflector<T>::Projection;

template<std::derived_from<Camera> T>
using CameraParamsOf = CameraParams<GetProjection<T>>;

template<std::derived_from<Camera> T>
using ProjectionUpdate = UpdateParams<GetProjection<T>>;

using ViewUpdate = UpdateParams<CameraView>;

template<typename T>
concept ReflectedCamera =
    ReflectedJson<GetProjection<T>> && std::same_as<std::string, decltype(CameraReflector<T>::getType())>
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
    std::function<CameraView()> getView;
    std::function<void(const CameraView &)> setView;
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
        .getView = [=] { return camera->params.view; },
        .setView = [=](const auto &view) { camera->params.view = view; },
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

CameraView getCameraView(LockedObjects &locked, const ObjectParams &params)
{
    return locked.visit(
        [&](ObjectManager &objects)
        {
            auto &camera = objects.get<CameraInterface>(params.id);

            return camera.getView();
        });
}

template<ReflectedCamera T>
GetProjection<T> getCameraProjection(LockedObjects &locked, const ObjectParams &params)
{
    return locked.visit(
        [&](ObjectManager &objects)
        {
            auto &interface = objects.get<CameraInterface>(params.id);
            auto &camera = castCamera<T>(interface);

            return camera.params.projection;
        });
}

void updateCameraView(LockedObjects &locked, const ViewUpdate &params)
{
    locked.visit(
        [&](ObjectManager &objects)
        {
            auto &camera = objects.get<CameraInterface>(params.id);

            camera.setView(params.properties);
        });
}

template<ReflectedCamera T>
void updateCameraProjection(LockedObjects &locked, const ProjectionUpdate<T> &params)
{
    locked.visit(
        [&](ObjectManager &objects)
        {
            auto &interface = objects.get<CameraInterface>(params.id);
            auto &camera = castCamera<T>(interface);

            camera.params.projection = params.properties;
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

    builder.endpoint("get-" + type, [&](ObjectParams params) { return getCameraProjection<T>(objects, params); })
        .description("Get projection part of a camera of type " + type);

    builder.endpoint("update-" + type, [&](ProjectionUpdate<T> params) { updateCameraProjection<T>(objects, params); })
        .description("Update projection part of a camera of type " + type);
}

template<>
struct JsonObjectReflector<Perspective>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<Perspective>();
        builder.field("fovy", [](auto &object) { return &object.fovy; })
            .description("Camera vertical field of view (horizontal is deduced from framebuffer aspect)")
            .defaultValue(45.0F);
        return builder.build();
    }
};

template<>
struct CameraReflector<PerspectiveCamera>
{
    using Projection = Perspective;

    static std::string getType()
    {
        return "perspective-camera";
    }

    static PerspectiveCamera create(Device &device, const CameraParamsOf<PerspectiveCamera> &params)
    {
        return createPerspectiveCamera(device, params.view, params.projection);
    }

    static void setAspect(PerspectiveCamera &camera, float aspect)
    {
        camera.setAspect(aspect);
    }
};

template<>
struct JsonObjectReflector<Orthographic>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<Orthographic>();
        builder.field("height", [](auto &object) { return &object.height; })
            .description("Camera viewport height in world coordinates (horizontal is deduced from framebuffer aspect)");
        return builder.build();
    }
};

template<>
struct CameraReflector<OrthographicCamera>
{
    using Projection = Orthographic;

    static std::string getType()
    {
        return "orthographic-camera";
    }

    static OrthographicCamera create(Device &device, const CameraParamsOf<OrthographicCamera> &params)
    {
        return createOrthographicCamera(device, params.view, params.projection);
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

    builder.endpoint("get-camera", [&](ObjectParams params) { return getCameraView(objects, params); })
        .description("Get the view of a camera of any type");

    builder.endpoint("update-camera", [&](ViewUpdate params) { return updateCameraView(objects, params); })
        .description("Update the view of a camera of any type");
}
}

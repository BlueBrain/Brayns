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

template<>
struct JsonObjectReflector<Perspective>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<Perspective>();
        builder.field("fovy", [](auto &object) { return &object.fovy; })
            .description("Camera vertical field of view (horizontal is deduced from framebuffer resolution)")
            .defaultValue(45.0F);
        return builder.build();
    }
};

template<>
struct JsonObjectReflector<Viewport>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<Viewport>();
        builder.field("height", [](auto &object) { return &object.height; })
            .description("Camera viewport height (horizontal is deduced from framebuffer resolution)");
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

/*struct UserCamera
{
    std::any params;
    std::function<void(float)> setAspectRatio;
};

using PerspectiveParams = CameraParams<Perspective>;
using PerspectiveUserCamera = UserCamera<Perspective>;

template<typename T>
struct ObjectReflector<UserCamera<T>>
{
    using Settings = PerspectiveParams;

    static std::string getType()
    {
        return "perspective-camera";
    }

    static PerspectiveParams getProperties(const PerspectiveUserCamera &camera)
    {
        return camera.value.params;
    }
};

using OrthographicParams = CameraParams<Viewport>;
using OrthographicUserCamera = UserCamera<Viewport>;

template<>
struct ObjectReflector<OrthographicUserCamera>
{
    using Settings = OrthographicParams;

    static std::string getType()
    {
        return "orthographic-camera";
    }

    static OrthographicParams getProperties(const OrthographicUserCamera &camera)
    {
        return camera.value.params;
    }
};*/

void addCameraEndpoints(ApiBuilder &builder, LockedObjects &objects, Device &device)
{
    (void)builder;
    (void)objects;
    (void)device;
}
}

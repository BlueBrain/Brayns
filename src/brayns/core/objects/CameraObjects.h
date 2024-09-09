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

#include <brayns/core/engine/Camera.h>
#include <brayns/core/manager/ObjectRegistry.h>

namespace brayns
{
template<>
struct JsonObjectReflector<View>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<View>();
        builder.field("position", [](auto &object) { return &object.position; }).description("Position XYZ");
        builder.field("direction", [](auto &object) { return &object.direction; }).description("Forward direction XYZ");
        builder.field("up", [](auto &object) { return &object.up; })
            .description("Up direction XYZ")
            .defaultValue(Vector3(0.0F, 1.0F, 0.0F));
        return builder.build();
    }
};

template<>
struct JsonObjectReflector<CameraSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<CameraSettings>();
        builder.field("view", [](auto &object) { return &object.view; }).description("Camera view in 3D space");
        builder.field("nearClip", [](auto &object) { return &object.nearClip; })
            .description("Distance to clip objects that are too close to the camera")
            .defaultValue(0.0F)
            .minimum(0.0F);
        builder.field("imageRegion", [](auto &object) { return &object.imageRegion; })
            .description("Normalized region of the camera viewport to be rendered (does not affect resolution)")
            .defaultValue(Box2{{0.0F, 0.0F}, {1.0F, 1.0F}});
        return builder.build();
    }
};

template<ReflectedJson T>
using CameraParams = ComposedParams<CameraSettings, T>;

using CameraInfo = CameraSettings;
using CameraUpdate = UpdateParams<CameraSettings>;

struct UserCamera
{
    std::reference_wrapper<Device> device;
    CameraSettings settings;
    std::any value;
    std::function<Camera()> get;
    std::function<void(float)> setAspect;
};

template<ReflectedJson Settings, std::derived_from<Camera> T>
struct DerivedCamera
{
    Settings settings;
    T value;
};

CameraInfo getCamera(ObjectRegistry &objects, const ObjectParams &params);
void updateCamera(ObjectRegistry &objects, const CameraUpdate &params);

template<>
struct JsonObjectReflector<DepthOfField>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<DepthOfField>();
        builder.field("apertureRadius", [](auto &object) { return &object.apertureRadius; })
            .description("Size of the aperture radius (0 is no depth of field)")
            .defaultValue(0.1F);
        builder.field("focusDistance", [](auto &object) { return &object.focusDistance; })
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
        builder.field("Left", StereoMode::Left).description("Render left eye");
        builder.field("Right", StereoMode::Right).description("Render right eye");
        builder.field("SideBySide", StereoMode::SideBySide).description("Render both eyes side by side");
        builder.field("TopBottom", StereoMode::TopBottom).description("Render left eye above right eye");
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
            .description("How to render images for each eye");
        builder.field("interpupillaryDistance", [](auto &object) { return &object.interpupillaryDistance; })
            .description("Distance between observer eyes")
            .defaultValue(0.0635F);
        return builder.build();
    }
};

template<>
struct JsonObjectReflector<PerspectiveCameraSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<PerspectiveCameraSettings>();
        builder.field("fovy", [](auto &object) { return &object.fovy; })
            .description("Camera vertical field of view in radians (horizontal is deduced from framebuffer aspect)")
            .defaultValue(radians(45.0F));
        builder.field("depthOfField", [](auto &object) { return &object.depthOfField; })
            .description("Depth of field settings, set to null to disable it");
        builder.field("architectural", [](auto &object) { return &object.architectural; })
            .description("Vertical edges are projected to be parallel")
            .defaultValue(false);
        builder.field("stereo", [](auto &object) { return &object.stereo; })
            .description("Stereo settings, set to null to disable it");
        return builder.build();
    }
};

using PerspectiveCameraParams = CameraParams<PerspectiveCameraSettings>;
using PerspectiveCameraInfo = PerspectiveCameraSettings;
using PerspectiveCameraUpdate = UpdateParams<PerspectiveCameraSettings>;
using UserPerspectiveCamera = DerivedCamera<PerspectiveCameraSettings, PerspectiveCamera>;

ObjectResult createPerspectiveCamera(ObjectRegistry &objects, Device &device, const PerspectiveCameraParams &params);
PerspectiveCameraInfo getPerspectiveCamera(ObjectRegistry &objects, const ObjectParams &params);
void updatePerspectiveCamera(ObjectRegistry &objects, const PerspectiveCameraUpdate &params);

template<>
struct JsonObjectReflector<OrthographicCameraSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<OrthographicCameraSettings>();
        builder.field("height", [](auto &object) { return &object.height; })
            .description("Camera viewport height in world coordinates (horizontal is deduced from framebuffer aspect)")
            .defaultValue(1.0F);
        return builder.build();
    }
};

using OrthographicCameraParams = CameraParams<OrthographicCameraSettings>;
using OrthographicCameraInfo = OrthographicCameraSettings;
using OrthographicCameraUpdate = UpdateParams<OrthographicCameraSettings>;
using UserOrthographicCamera = DerivedCamera<OrthographicCameraSettings, OrthographicCamera>;

ObjectResult createOrthographicCamera(ObjectRegistry &objects, Device &device, const OrthographicCameraParams &params);
OrthographicCameraInfo getOrthographicCamera(ObjectRegistry &objects, const ObjectParams &params);
void updateOrthographicCamera(ObjectRegistry &objects, const OrthographicCameraUpdate &params);

template<>
struct JsonObjectReflector<PanoramicCameraSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<PanoramicCameraSettings>();
        builder.field("stereo", [](auto &object) { return &object.stereo; })
            .description("Stereo settings, set to null to disable it");
        return builder.build();
    }
};

using PanoramicCameraParams = CameraParams<PanoramicCameraSettings>;
using PanoramicCameraInfo = PanoramicCameraSettings;
using PanoramicCameraUpdate = UpdateParams<PanoramicCameraSettings>;
using UserPanoramicCamera = DerivedCamera<PanoramicCameraSettings, PanoramicCamera>;

ObjectResult createPanoramicCamera(ObjectRegistry &objects, Device &device, const PanoramicCameraParams &params);
PanoramicCameraInfo getPanoramicCamera(ObjectRegistry &objects, const ObjectParams &params);
void updatePanoramicCamera(ObjectRegistry &objects, const PanoramicCameraUpdate &params);
}

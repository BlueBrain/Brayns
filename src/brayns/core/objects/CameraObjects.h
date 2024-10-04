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
#include <brayns/core/manager/ObjectManager.h>

namespace brayns
{
struct UserCamera
{
    CameraSettings settings;
    std::any value;
    std::function<Camera()> get;
};

template<>
struct JsonObjectReflector<CameraSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<CameraSettings>();
        builder.field("position", [](auto &object) { return &object.position; })
            .description("Camera position XYZ")
            .defaultValue(Vector3(0.0F, 0.0F, 0.0F));
        builder.field("direction", [](auto &object) { return &object.direction; })
            .description("Camera forward direction XYZ")
            .defaultValue(Vector3(0.0F, 0.0F, 1.0F));
        builder.field("up", [](auto &object) { return &object.up; }).description("Camera up direction XYZ").defaultValue(Vector3(0.0F, 1.0F, 0.0F));
        builder.field("nearClip", [](auto &object) { return &object.nearClip; })
            .description("Distance to clip objects that are too close to the camera")
            .defaultValue(0.0F)
            .minimum(0.0F);
        builder.field("region", [](auto &object) { return &object.region; })
            .description("Normalized region of the camera to be rendered")
            .defaultValue(Box2{{0.0F, 0.0F}, {1.0F, 1.0F}});
        return builder.build();
    }
};

using GetCameraResult = GetResultOf<CameraSettings>;
using UpdateCameraParams = UpdateParamsOf<CameraSettings>;

GetCameraResult getCamera(ObjectManager &objects, const GetObjectParams &params);
void updateCamera(ObjectManager &objects, Device &device, const UpdateCameraParams &params);

template<ReflectedJsonObject Settings, std::derived_from<Camera> T>
struct UserCameraOf
{
    Settings settings;
    T value;
};

template<ReflectedJsonObject T>
using CreateCameraParamsOf = CreateParamsOf<ComposedParamsOf<CameraSettings, T>>;

template<>
struct EnumReflector<Stereo>
{
    static auto reflect()
    {
        auto builder = EnumBuilder<Stereo>();
        builder.field("None", Stereo::None).description("Disable stereo");
        builder.field("Left", Stereo::Left).description("Render left eye");
        builder.field("Right", Stereo::Right).description("Render right eye");
        builder.field("SideBySide", Stereo::SideBySide).description("Render both eyes side by side");
        builder.field("TopBottom", Stereo::TopBottom).description("Render left eye above right eye");
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
            .description("Camera vertical field of view in radians")
            .defaultValue(radians(45.0F));
        builder.field("aspect", [](auto &object) { return &object.aspect; })
            .description("Camera image aspect ratio (width / height)")
            .defaultValue(1.0F);
        builder.field("apertureRadius", [](auto &object) { return &object.apertureRadius; })
            .description("Size of the aperture radius (0 is no depth of field)")
            .defaultValue(0.0F);
        builder.field("focusDistance", [](auto &object) { return &object.focusDistance; })
            .description("Distance at which the image is the sharpest")
            .defaultValue(1.0F);
        builder.field("architectural", [](auto &object) { return &object.architectural; })
            .description("Vertical edges are projected to be parallel")
            .defaultValue(false);
        builder.field("stereo", [](auto &object) { return &object.stereo; }).description("Stereo mode").defaultValue(Stereo::None);
        builder.field("interpupillaryDistance", [](auto &object) { return &object.interpupillaryDistance; })
            .description("Distance between observer eyes when stereo is enabled")
            .defaultValue(0.0635F);
        return builder.build();
    }
};

using CreatePerspectiveCameraParams = CreateCameraParamsOf<PerspectiveCameraSettings>;
using GetPerspectiveCameraResult = GetResultOf<PerspectiveCameraSettings>;
using UpdatePerspectiveCameraParams = UpdateParamsOf<PerspectiveCameraSettings>;
using UserPerspectiveCamera = UserCameraOf<PerspectiveCameraSettings, PerspectiveCamera>;

CreateObjectResult createPerspectiveCamera(ObjectManager &objects, Device &device, const CreatePerspectiveCameraParams &params);
GetPerspectiveCameraResult getPerspectiveCamera(ObjectManager &objects, const GetObjectParams &params);
void updatePerspectiveCamera(ObjectManager &objects, Device &device, const UpdatePerspectiveCameraParams &params);

template<>
struct JsonObjectReflector<OrthographicCameraSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<OrthographicCameraSettings>();
        builder.field("height", [](auto &object) { return &object.height; })
            .description("Camera viewport height in world coordinates (horizontal is deduced from framebuffer aspect)")
            .defaultValue(1.0F);
        builder.field("aspect", [](auto &object) { return &object.aspect; })
            .description("Camera image aspect ratio (width / height)")
            .defaultValue(1.0F);
        return builder.build();
    }
};

using CreateOrthographicCameraParams = CreateCameraParamsOf<OrthographicCameraSettings>;
using GetOrthographicCameraResult = GetResultOf<OrthographicCameraSettings>;
using UpdateOrthographicCameraParams = UpdateParamsOf<OrthographicCameraSettings>;
using UserOrthographicCamera = UserCameraOf<OrthographicCameraSettings, OrthographicCamera>;

CreateObjectResult createOrthographicCamera(ObjectManager &objects, Device &device, const CreateOrthographicCameraParams &params);
GetOrthographicCameraResult getOrthographicCamera(ObjectManager &objects, const GetObjectParams &params);
void updateOrthographicCamera(ObjectManager &objects, Device &device, const UpdateOrthographicCameraParams &params);

template<>
struct JsonObjectReflector<PanoramicCameraSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<PanoramicCameraSettings>();
        builder.field("stereo", [](auto &object) { return &object.stereo; }).description("Stereo mode").defaultValue(Stereo::None);
        builder.field("interpupillaryDistance", [](auto &object) { return &object.interpupillaryDistance; })
            .description("Distance between observer eyes when stereo is enabled")
            .defaultValue(0.0635F);
        return builder.build();
    }
};

using CreatePanoramicCameraParams = CreateCameraParamsOf<PanoramicCameraSettings>;
using GetPanoramicCameraResult = GetResultOf<PanoramicCameraSettings>;
using UpdatePanoramicCameraParams = UpdateParamsOf<PanoramicCameraSettings>;
using UserPanoramicCamera = UserCameraOf<PanoramicCameraSettings, PanoramicCamera>;

CreateObjectResult createPanoramicCamera(ObjectManager &objects, Device &device, const CreatePanoramicCameraParams &params);
GetPanoramicCameraResult getPanoramicCamera(ObjectManager &objects, const GetObjectParams &params);
void updatePanoramicCamera(ObjectManager &objects, Device &device, const UpdatePanoramicCameraParams &params);
}

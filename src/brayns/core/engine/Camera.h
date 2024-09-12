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

#include "Device.h"
#include "Object.h"

namespace brayns
{
struct View
{
    Vector3 position = {0.0F, 0.0F, 0.0F};
    Vector3 direction = {0.0F, 0.0F, 1.0F};
    Vector3 up = {0.0F, 1.0F, 0.0F};
};

struct CameraSettings
{
    View view = {};
    float nearClip = 1.0e-6F;
    Box2 imageRegion = {{0.0F, 0.0F}, {1.0F, 1.0F}};
};

class Camera : public Managed<OSPCamera>
{
public:
    using Managed::Managed;

    void update(const CameraSettings &settings);
};

struct DepthOfField
{
    float apertureRadius;
    float focusDistance = 1.0F;
};

enum class StereoMode
{
    Left = OSP_STEREO_LEFT,
    Right = OSP_STEREO_RIGHT,
    SideBySide = OSP_STEREO_SIDE_BY_SIDE,
    TopBottom = OSP_STEREO_TOP_BOTTOM,
};

struct Stereo
{
    StereoMode mode;
    float interpupillaryDistance = 0.0635F;
};

struct PerspectiveCameraSettings
{
    float fovy = radians(60.0F);
    float aspect = 1.0F;
    std::optional<DepthOfField> depthOfField = {};
    bool architectural = false;
    std::optional<Stereo> stereo = {};
};

class PerspectiveCamera : public Camera
{
public:
    using Camera::Camera;
    using Camera::update;

    void update(const PerspectiveCameraSettings &settings);
    void setAspect(float aspect);
};

PerspectiveCamera createPerspectiveCamera(
    Device &device,
    const CameraSettings &settings = {},
    const PerspectiveCameraSettings &perspective = {});

struct OrthographicCameraSettings
{
    float height = 1.0F;
    float aspect = 1.0F;
};

class OrthographicCamera : public Camera
{
public:
    using Camera::Camera;
    using Camera::update;

    void update(const OrthographicCameraSettings &settings);
    void setAspect(float aspect);
};

OrthographicCamera createOrthographicCamera(
    Device &device,
    const CameraSettings &settings = {},
    const OrthographicCameraSettings &orthographic = {});

struct PanoramicCameraSettings
{
    std::optional<Stereo> stereo;
};

class PanoramicCamera : public Camera
{
public:
    using Camera::Camera;
    using Camera::update;

    void update(const PanoramicCameraSettings &settings);
};

PanoramicCamera createPanoramicCamera(
    Device &device,
    const CameraSettings &settings = {},
    const PanoramicCameraSettings &panoramic = {});
}

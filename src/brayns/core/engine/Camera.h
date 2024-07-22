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
struct CameraSettings
{
    Vector3 position = {0.0F, 0.0F, 0.0F};
    Vector3 direction = {0.0F, 0.0F, 1.0F};
    Vector3 up = {0.0F, 1.0F, 0.0F};
    float nearClippingDistance = 1.0e-6F;
};

class Camera : public Managed<OSPCamera>
{
public:
    using Managed::Managed;
};

struct PerspectiveCameraSettings
{
    CameraSettings base = {};
    float fovy = 60.0F;
    float aspectRatio = 1.0F;
};

class PerspectiveCamera : public Camera
{
public:
    using Camera::Camera;
};

PerspectiveCamera createPerspectiveCamera(Device &device, const PerspectiveCameraSettings &settings);

struct OrthographicCameraSettings
{
    CameraSettings base = {};
    float height = 1.0F;
    float aspectRatio = 1.0F;
};

class OrthographicCamera : public Camera
{
public:
    using Camera::Camera;
};

OrthographicCamera createOrthographicCamera(Device &device, const OrthographicCameraSettings &settings);
}

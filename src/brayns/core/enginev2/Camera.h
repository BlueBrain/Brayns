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

#include "Object.h"

namespace brayns::experimental
{
struct CameraSettings
{
    Vector3 position = {0.0f, 0.0f, 0.0f};
    Vector3 direction = {0.0f, 0.0f, 1.0f};
    Vector3 up = {0.0f, 1.0f, 0.0f};
    float nearClippingDistance = 1.0e-6F;
};

class Camera : public Managed<OSPCamera>
{
public:
    using Managed::Managed;
};

struct PerspectiveCameraSettings : CameraSettings
{
    float fovy = 60.0f;
    float aspectRatio = 1.0f;
};

class PerspectiveCamera : public Camera
{
public:
    using Camera::Camera;
};

void loadPerspectiveCameraParams(OSPCamera handle, const PerspectiveCameraSettings &settings);

struct OrthographicCameraSettings : CameraSettings
{
    float height = 1.0f;
    float aspectRatio = 1.0f;
};

void loadOrthographicCameraParams(OSPCamera handle, const OrthographicCameraSettings &settings);

class OrthographicCamera : public Camera
{
public:
    using Camera::Camera;
};
}

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

#include "Managed.h"

namespace brayns::experimental
{
class Camera : public Managed<OSPCamera>
{
public:
    using Managed::Managed;

    void setTransform(const Affine3 &transform);
    void setNearClip(float distance);
};

class PerspectiveCamera : public Camera
{
public:
    static inline const std::string name = "perspective";

    using Camera::Camera;

    void setFovy(float degrees);
    void setAspectRatio(float aspect);
};

class OrthographicCamera : public Camera
{
public:
    static inline const std::string name = "orthographic";

    using Camera::Camera;

    void setHeight(float height);
    void setAspectRatio(float aspect);
};
}

namespace ospray
{
OSPTYPEFOR_SPECIALIZATION(brayns::experimental::Camera, OSP_CAMERA)
OSPTYPEFOR_SPECIALIZATION(brayns::experimental::PerspectiveCamera, OSP_CAMERA)
OSPTYPEFOR_SPECIALIZATION(brayns::experimental::OrthographicCamera, OSP_CAMERA)
}

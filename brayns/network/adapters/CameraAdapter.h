/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include <brayns/json/JsonAdapterMacro.h>

#include <brayns/engine/cameras/OrthographicCamera.h>
#include <brayns/engine/cameras/PerspectiveCamera.h>

namespace brayns
{
BRAYNS_JSON_ADAPTER_BEGIN(OrthographicCamera)
BRAYNS_JSON_ADAPTER_GETSET("height", getHeight, setHeight, "Camera orthographic projection height")
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(PerspectiveCamera)
BRAYNS_JSON_ADAPTER_GETSET(
    "fovy",
    getFOVY,
    setFOVY,
    "Camera vertical field of view (in degrees)",
    Default(45.f),
    Minimum(1.f))
BRAYNS_JSON_ADAPTER_GETSET(
    "aperture_radius",
    getApertureRadius,
    setApertureRadius,
    "Lens aperture radius (Use for depth of field effect. A value of 0.0 disables it",
    Default(0.f),
    Minimum(0.f))
BRAYNS_JSON_ADAPTER_GETSET(
    "focus_distance",
    getFocusDistance,
    setFocusDistance,
    "Distance at which to focus (for depth of field effect). A value of 1.0 disables it.",
    Default(1.f),
    Minimum(1.f))
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(LookAt)
BRAYNS_JSON_ADAPTER_ENTRY(position, "Position of the camera", Required(false))
BRAYNS_JSON_ADAPTER_ENTRY(target, "Target position at which the camera is looking", Required(false))
BRAYNS_JSON_ADAPTER_ENTRY(up, "Up vector to compute the camera orthonormal basis", Required(false))
BRAYNS_JSON_ADAPTER_END()

class GenericLookAt
{
public:
    GenericLookAt() = default;
    GenericLookAt(LookAt baseLookAt);

    const Vector3f &getPosition() const noexcept;

    void setPosition(const Vector3f &position) noexcept;

    const Vector3f &getTarget() const noexcept;

    void setTarget(const Vector3f &target) noexcept;

    const Vector3f &getUp() const noexcept;

    void setUp(const Vector3f &up) noexcept;

    const LookAt &getLookAt() const noexcept;

private:
    LookAt _lookAt;
};

BRAYNS_JSON_ADAPTER_BEGIN(GenericLookAt)
BRAYNS_JSON_ADAPTER_GETSET("position", getPosition, setPosition, "Position of the camera", Required(false))
BRAYNS_JSON_ADAPTER_GETSET(
    "target",
    getTarget,
    setTarget,
    "Target position at which the camera is looking",
    Required(false))
BRAYNS_JSON_ADAPTER_GETSET("up", getUp, setUp, "Up vector to compute the camera orthonormal basis", Required(false))
BRAYNS_JSON_ADAPTER_END()
} // namespace brayns

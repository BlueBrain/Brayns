/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/engine/Camera.h>
#include <brayns/json/JsonAdapterMacro.h>

namespace brayns
{
class PerspectiveCamera : public Camera
{
public:
    PerspectiveCamera();

    std::string_view getName() const noexcept final;

    void commitCameraSpecificParams() final;

    void setFOVY(const float fovy) noexcept;
    void setApertureRadius(const float aperture) noexcept;
    void setFocusDistance(const float distance) noexcept;

    float getFOVY() const noexcept;
    float getApertureRadius() const noexcept;
    float getFocusDistance() const noexcept;
private:
    float _fovy {45.f};
    float _apertureRadius {0.f};
    float _focusDistance {1.f};
};

BRAYNS_JSON_ADAPTER_BEGIN(PerspectiveCamera)
BRAYNS_JSON_ADAPTER_GETSET("position", getPosition, setPosition, "Camera position in world space coordinates")
BRAYNS_JSON_ADAPTER_GETSET("target", getTarget, setTarget, "Camera target in world space coordinates")
BRAYNS_JSON_ADAPTER_GETSET("up", getUp, setUp, "Camera UP vector")
BRAYNS_JSON_ADAPTER_GETSET("fovy", getFOVY, setFOVY, "Camera vertical field of view (in degrees)")
BRAYNS_JSON_ADAPTER_GETSET("aperture_radius", getApertureRadius, setApertureRadius,
                           "Lens aperture radius (Use for depth of field effect. A value of 0.0 disables it. "
                           "Will be clamped to the range [0.0, +infinity)")
BRAYNS_JSON_ADAPTER_GETSET("focus_distance", getFocusDistance, setFocusDistance,
                           "Distance at which to focus (for depth of field effect). A value of 1.0 disables it. "
                           "Will be clamped to the range [1.0, +infinity]")
BRAYNS_JSON_ADAPTER_END()
}

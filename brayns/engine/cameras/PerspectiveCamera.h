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
    Ptr clone() const noexcept final;

    /**
     * @brief Sets the perspective viertical field of view (in degrees)
     */
    void setFOVY(const float fovy) noexcept;

    /**
     * @brief Sets the camera lens aperture radius. Used for depth of field effect. A value of 0.0 disables
     * the effect. Must be >= 0.0
     */
    void setApertureRadius(const float aperture) noexcept;

    /**
     * @brief Sets the distance at which the camera lens is focusing. Used for depth of field effect. A value of 1.0
     * disables the effect. Must be >= 1.0
     */
    void setFocusDistance(const float distance) noexcept;

    /**
     * @brief Returns the perspective vertical field of view (in degrees)
     */
    float getFOVY() const noexcept;

    /**
     * @brief Returns the camera lens aperture radius.
     */
    float getApertureRadius() const noexcept;

    /**
     * @brief Returns the distance at which the camera lens is focusing.
     */
    float getFocusDistance() const noexcept;

protected:
    std::string_view getOSPHandleName() const noexcept final;
    void commitCameraSpecificParams() final;

private:
    float _fovy {45.f};
    float _apertureRadius {0.f};
    float _focusDistance {1.f};
};

BRAYNS_JSON_ADAPTER_BEGIN(PerspectiveCamera)
BRAYNS_JSON_ADAPTER_GETSET("fovy", getFOVY, setFOVY, "Camera vertical field of view (in degrees)")
BRAYNS_JSON_ADAPTER_GETSET("aperture_radius", getApertureRadius, setApertureRadius,
                           "Lens aperture radius (Use for depth of field effect. A value of 0.0 disables it. "
                           "Will be clamped to the range [0.0, +infinity)")
BRAYNS_JSON_ADAPTER_GETSET("focus_distance", getFocusDistance, setFocusDistance,
                           "Distance at which to focus (for depth of field effect). A value of 1.0 disables it. "
                           "Will be clamped to the range [1.0, +infinity]")
BRAYNS_JSON_ADAPTER_END()
}

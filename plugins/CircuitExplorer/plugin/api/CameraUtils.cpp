/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman <nadir.romanguerrero@epfl.ch>
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

#include "CameraUtils.h"

void CameraUtils::updateCamera(brayns::Camera& camera,
                               const ODUCameraInformation& cinfo)
{
    auto& position = cinfo.origin;
    auto& direction = cinfo.direction;
    auto& up = cinfo.up;
    auto target = position + direction;
    auto orientation = glm::lookAt(position, target, up);
    auto apertureRadius = cinfo.apertureRadius;
    auto focusDistance = cinfo.focusDistance;
    camera.setPosition(position);
    camera.setTarget(target);
    camera.setOrientation(glm::inverse(glm::quat_cast(orientation)));
    camera.updateProperty("apertureRadius", apertureRadius);
    camera.updateProperty("focusDistance", focusDistance);
    camera.commit();
}

ODUCameraInformation CameraUtils::getCameraAsODU(const brayns::Camera& camera)
{
    ODUCameraInformation result;
    auto& position = camera.getPosition();
    result.origin = position;
    auto& orientation = camera.getOrientation();
    result.direction = glm::rotate(orientation, {0.0, 0.0, -1.0});
    result.up = glm::rotate(orientation, {0.0, 1.0, 0.0});
    auto apertureRadius = camera.getPropertyOrValue("apertureRadius", 0.0);
    result.apertureRadius = apertureRadius;
    auto focusDistance = camera.getPropertyOrValue("focusDistance", 0.0);
    result.focusDistance = focusDistance;
    return result;
}

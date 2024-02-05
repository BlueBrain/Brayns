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

#pragma once

#include <brayns/engine/Camera.h>

// TODO: Setting up the camera in this manner is kind of common in 3D
// applications. To be more precise, a "look at" camera is widely used and
// shoulbe supported in brayns (The only difference with this definition is a
// "view target" instead of a "view direction". Tasks to-do:
// - Replace ODU definition for look at (replace direction for target).
// - Drop aperture radius and focus distance
// - Move to core
// - Add native support in the camera class to set up through this definition

/**
 * @brief The ODUCameraInformation struct holds the necessary data to set up the
 * camera in a origin-direction-up fashion.
 */
struct ODUCameraInformation
{
    brayns::Vector3f origin;
    brayns::Vector3f direction;
    brayns::Vector3f up;
    // Params used by DOFPerspectiveCamera
    float apertureRadius;
    float focusDistance;
};

/**
 * @brief The CameraUtils class performs the basic operations to trasnform to
 * and from the native camera definition and the ODU camera definition
 *
 *        To be removed
 */
class CameraUtils
{
public:
    /**
     * @brief updates the renderer camera with the given ODU definition.
     */
    static void updateCamera(brayns::Camera& camera,
                             const ODUCameraInformation& cinfo);

    /**
     * @brief returns an ODU definition of the current camera state.
     */
    static ODUCameraInformation getCameraAsODU(const brayns::Camera& camera);
};

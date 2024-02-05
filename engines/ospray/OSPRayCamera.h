/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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
#include <ospray.h>

namespace brayns
{
/**
   OPSRAY specific camera

   This object is the OSPRay specific implementation of a Camera
*/
class OSPRayCamera : public Camera
{
public:
    OSPRayCamera() = default;
    ~OSPRayCamera();

    /**
       Commits the changes held by the camera object so that
       attributes become available to the OSPRay rendering engine
    */
    void commit() final;

    /**
       Set the clipping planes to use in this camera.
       Only implemented in the perspective and orthographic cameras.
    */
    void setClipPlanes(const std::vector<Plane>& planes);

    /**
       Gets the OSPRay implementation of the camera object
       @return OSPRay implementation of the camera object
    */
    OSPCamera impl() { return _camera; }

private:
    OSPCamera _camera{nullptr};
    std::string _currentOSPCamera;
    std::vector<Plane> _clipPlanes;

    void _createOSPCamera();
};
} // namespace brayns

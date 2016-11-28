/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
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

#ifndef OPTIXCAMERA_H
#define OPTIXCAMERA_H

#include <brayns/common/camera/Camera.h>
#include <optixu/optixpp_namespace.h>

namespace brayns
{

/**
   OptiX specific camera

   This object is the OptiX specific implementation of a Camera
*/
class OptiXCamera : public brayns::Camera
{

public:

    OptiXCamera(
        const CameraType cameraType,
        optix::Context& context,
        const bool environmentMap );

    /**
       Commits the changes held by the camera object so that
       attributes become available to the OptiX rendering engine
    */
    void commit() final;

    /**
       Gets the OptiX implementation of the camera object
       @return OptiX implementation of the camera object
    */
    optix::Program& impl() { return _camera; }

private:

    void _calculateCameraVariables( Vector3f& U, Vector3f& V, Vector3f& W );

    optix::Program _camera;
    optix::Program _exceptionProgram;
    optix::Context& _context;

};

}
#endif // OptiXCAMERA_H

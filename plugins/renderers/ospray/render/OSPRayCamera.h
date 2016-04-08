/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#ifndef OSPRAYCAMERA_H
#define OSPRAYCAMERA_H

#include <brayns/common/camera/Camera.h>
#include <ospray.h>

namespace brayns
{

/**
   OPSRAY specific camera

   This object is the OSPRay specific implementation of a Camera
*/
class OSPRayCamera : public brayns::Camera
{
public:
    OSPRayCamera( const CameraType cameraType );

    /**
       Commits the changes held by the camera object so that
       attributes become available to the OSPRay rendering engine
    */
    void commit() final;

    /** @copydoc Camera::setAspectRatio */
    void setAspectRatio( float aspectRatio ) final;

    /**
       Gets the OSPRay implementation of the camera object
       @return OSPRay implementation of the camera object
    */
    OSPCamera ospImpl() { return _camera; }

private:
    OSPCamera _camera;
};

}
#endif // OSPRAYCAMERA_H

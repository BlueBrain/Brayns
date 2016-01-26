/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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

    /** @copydoc Camera::set */
    void set( const Vector3f& position, const Vector3f& target,
              const Vector3f& up ) final;

    /** @copydoc Camera::setPosition */
    void setPosition(const Vector3f&) final;

    /** @copydoc Camera::getPosition */
    const Vector3f& getPosition() final { return _position; };

    /** @copydoc Camera::setTarget */
    void setTarget(const Vector3f&) final;

    /** @copydoc Camera::getTarget */
    const Vector3f& getTarget() final { return _target; };

    /** @copydoc Camera::setUp */
    void setUp(const Vector3f&) final;

    /** @copydoc Camera::getUp */
    const Vector3f& getUp() final { return _up; };

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
    OSPCamera impl() { return _camera; }

private:
    OSPCamera _camera;
};

}
#endif // OSPRAYCAMERA_H

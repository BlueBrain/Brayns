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

#ifndef CAMERA_H
#define CAMERA_H

#include <brayns/api.h>
#include <brayns/common/types.h>

namespace brayns
{

enum CameraType
{
    CT_UNDEFINED = 0,
    CT_PERSPECTIVE,
    CT_STEREO,
    CT_ORTHOGRAPHIC
};

/**
   Camera object

   This object in an abstract interface to a camera which is defined by a position, target and
   up vector
*/
class Camera
{
public:
    BRAYNS_API Camera(const CameraType cameraType);
    BRAYNS_API virtual ~Camera() {}

    /**
       Sets position, target and up vector
       @param position The x, y, z coordinates of the camera position
       @param target The x, y, z coordinates of the camera target: the point the camera is "looking
              at" or focused on
       @param up the x, y, z coordinates of the up vector's end point
    */
    BRAYNS_API virtual void set(
        const Vector3f& position, const Vector3f& target, const Vector3f& up) = 0;

    /**
       Sets camera position
       @param position The x, y, z coordinates of the camera position
    */
    BRAYNS_API virtual void setPosition(const Vector3f&) = 0;

    /**
       Gets camera position
       @return The x, y, z coordinates of the camera position
    */
    BRAYNS_API virtual const Vector3f& getPosition() = 0;

    /**
       Sets camera target
       @param target The x, y, z coordinates of the camera target: the point the camera is "looking
              at" or focused on
    */
    BRAYNS_API virtual void setTarget(const Vector3f&) = 0;

    /**
       Gets camera target
       @return The x, y, z coordinates of the camera target: the point the camera is "looking at" or
               focused on
    */
    BRAYNS_API virtual const Vector3f& getTarget() = 0;

    /**
       Sets camera up vector
       @param up the x, y, z coordinates of the up vector's end point
    */
    BRAYNS_API virtual void setUp(const Vector3f&) = 0;

    /**
       Gets camera up vector
       @return the x, y, z coordinates of the up vector's end point
    */
    BRAYNS_API virtual const Vector3f& getUp() = 0;

    /**
       Commits the changes held by the camera object so that
       attributes become available to the underlying rendering engine
    */
    BRAYNS_API virtual void commit() =  0;

    /**
       Set the aspec ratio of the camera
       @param aspectRatio The new aspect ratio
    */
    BRAYNS_API virtual void setAspectRatio(float aspectRatio) = 0;

protected:
    Vector3f _position;
    Vector3f _target;
    Vector3f _up;
    CameraType _cameraType;
    float _aspectRatio;
};

}

#endif // CAMERA_H

/* Copyright (c) 2011-2015, EPFL/Blue Brain Project
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

class Camera
{
public:
    BRAYNS_API Camera(const CameraType cameraType, const Vector2i& frameSize);
    BRAYNS_API virtual ~Camera() {}

    BRAYNS_API virtual void set(
        const Vector3f& position, const Vector3f& target, const Vector3f& up) = 0;
    BRAYNS_API virtual const Vector3f& getPosition() = 0;
    BRAYNS_API virtual const Vector3f& getTarget() = 0;
    BRAYNS_API virtual const Vector3f& getUp() = 0;
    BRAYNS_API virtual void commit() =  0;
    BRAYNS_API virtual void resize(const Vector2i& frameSize) = 0;

protected:
    Vector3f _position;
    Vector3f _target;
    Vector3f _up;
    CameraType _cameraType;
    Vector2ui _frameSize;
};

}

#endif // CAMERA_H

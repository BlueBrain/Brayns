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

#ifndef OSPRAYCAMERA_H
#define OSPRAYCAMERA_H

#include <brayns/common/camera/Camera.h>
#include <ospray.h>

namespace brayns
{

class OSPRayCamera : public brayns::Camera
{
public:
    OSPRayCamera( const CameraType cameraType, const Vector2i& frameSize );

    void set( const Vector3f& position, const Vector3f& target, const Vector3f& up ) final;
    const Vector3f& getPosition() final;
    const Vector3f& getTarget() final;
    const Vector3f& getUp() final;

    void commit() final;
    void resize( const Vector2i& frameSize ) final;

    OSPCamera impl() {return _camera;}

private:
    OSPCamera _camera;
};

}
#endif // OSPRAYCAMERA_H

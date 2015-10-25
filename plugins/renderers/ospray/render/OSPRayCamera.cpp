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

#include "OSPRayCamera.h"

#include <brayns/common/log.h>
#include <ospray/common/OSPCommon.h>

namespace brayns
{

OSPRayCamera::OSPRayCamera( const CameraType cameraType, const Vector2i& frameSize )
   : Camera(cameraType, frameSize)
{
    std::string cameraAsString;
    switch( _cameraType )
    {
    case CT_STEREO:
        cameraAsString = "stereo";
        break;
    default:
        cameraAsString = "perspective";
        break;
    }

    _camera = ospNewCamera( cameraAsString.c_str());
    float aspect=static_cast<float>(_frameSize[0])/static_cast<float>(_frameSize[1]);
    ospSetf(_camera, "aspect", aspect);
    commit();
}

void OSPRayCamera::set( const Vector3f& position, const Vector3f& target, const Vector3f& up )
{
    ospSet3f(_camera,"pos", position.x(), position.y(), position.z());
    ospSet3f(_camera,"dir", target.x(), target.y(), target.z());
    ospSet3f(_camera,"up", up.x(), up.y(), up.z());
    commit();
}

void OSPRayCamera::commit()
{
    ospCommit(_camera);
}

void OSPRayCamera::resize( const Vector2i& frameSize )
{
    _frameSize = frameSize;
    float aspect=static_cast<float>(_frameSize[0])/static_cast<float>(_frameSize[1]);
    ospSetf(_camera, "aspect", aspect);
    commit();
}

const Vector3f& OSPRayCamera::getPosition()
{
    ospray::vec3f position;
    ospGetVec3f(_camera, "pos", &position);
    _position = Vector3f(position.x, position.y, position.z);
    return _position;
}

const Vector3f& OSPRayCamera::getTarget()
{
    ospray::vec3f target;
    ospGetVec3f(_camera, "dir", &target);
    _target = Vector3f(target.x, target.y, target.z);
    return _target;
}

const Vector3f& OSPRayCamera::getUp()
{
    ospray::vec3f up;
    ospGetVec3f(_camera, "up", &up);
    _up = Vector3f(up.x, up.y, up.z);
    return _up;
}


}

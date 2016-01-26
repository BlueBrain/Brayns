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

OSPRayCamera::OSPRayCamera( const CameraType cameraType )
   : Camera( cameraType )
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
    _camera = ospNewCamera( cameraAsString.c_str( ));
    commit();
}

void OSPRayCamera::set( const Vector3f& position, const Vector3f& target,
                        const Vector3f& up )
{
    _position = position;
    _target = target;
    _up = up;
    commit();
}

void OSPRayCamera::commit()
{
    ospSet3f( _camera,"pos", _position.x(), _position.y(), _position.z( ));
    ospSet3f( _camera,"dir", _target.x(), _target.y(), _target.z( ));
    ospSet3f( _camera,"up", _up.x(), _up.y(), _up.z( ));
    ospCommit( _camera );
}

void OSPRayCamera::setAspectRatio( float aspectRatio )
{
    ospSetf( _camera, "aspect", aspectRatio );
    commit();
}

void OSPRayCamera::setPosition( const Vector3f& position )
{
    _position = position;
    commit();
}

void OSPRayCamera::setTarget( const Vector3f& target )
{
    _target = target;
    commit();
}

void OSPRayCamera::setUp( const Vector3f& up )
{
    _up = up;
    commit();
}

}

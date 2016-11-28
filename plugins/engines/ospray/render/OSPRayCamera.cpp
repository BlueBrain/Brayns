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

#include "OSPRayCamera.h"

#include <brayns/common/log.h>
#include <ospray/common/OSPCommon.h>

namespace brayns
{

OSPRayCamera::OSPRayCamera( const CameraType cameraType )
   : Camera( cameraType )
{
    std::string cameraAsString;
    switch( getType( ))
    {
    case CameraType::stereo:
        cameraAsString = "stereo";
        break;
    case CameraType::orthographic:
        cameraAsString = "orthographic";
        break;
    case CameraType::panoramic:
        cameraAsString = "panoramic";
        break;
    default:
        cameraAsString = "perspective";
        break;
    }
    _camera = ospNewCamera( cameraAsString.c_str( ));
}

void OSPRayCamera::commit()
{
    const Vector3f& position = getPosition();
    const Vector3f& target = getTarget();
    const Vector3f dir = normalize( target - position );
    const Vector3f& up = getUp();

    ospSet3f( _camera,"pos", position.x(), position.y(), position.z( ));
    ospSet3f( _camera,"dir", dir.x(), dir.y(), dir.z( ));
    ospSet3f( _camera,"up", up.x(), up.y(), up.z( ));
    ospSetf( _camera, "aspect", getAspectRatio( ));
    ospSetf( _camera, "apertureRadius", getAperture( ));
    ospSetf( _camera, "focusDistance", getFocalLength( ));
    ospCommit( _camera );
}

void OSPRayCamera::setEnvironmentMap( const bool )
{
}

}

/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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
    case CT_STEREO:
        cameraAsString = "stereo";
        break;
    case CT_ORTHOGRAPHIC:
        cameraAsString = "orthographic";
        break;
    case CT_PANORAMIC:
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
    const Vector3f& upVector = getUpVector();

    ospSet3f( _camera,"pos", position.x(), position.y(), position.z( ));
    ospSet3f( _camera,"dir", dir.x(), dir.y(), dir.z( ));
    ospSet3f( _camera,"up", upVector.x(), upVector.y(), upVector.z( ));
    ospSetf( _camera, "aspect", getAspectRatio( ));
    ospSetf( _camera, "apertureRadius", getAperture( ));
    ospSetf( _camera, "focusDistance", getFocalLength( ));
    ospCommit( _camera );
}

}

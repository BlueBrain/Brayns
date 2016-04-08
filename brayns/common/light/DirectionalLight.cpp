/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#include "DirectionalLight.h"

namespace brayns
{

DirectionalLight::DirectionalLight(
    const Vector3f& direction,
    const Vector3f& color,
    const float intensity )
    : Light( color, intensity )
    , _direction( direction )
{
}

}

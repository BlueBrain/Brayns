/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#include "PointLight.h"

namespace brayns
{

PointLight::PointLight(
    const Vector3f& color,
    const Vector3f& position,
    const float intensity,
    const float cutoffDistance )
    : Light( color, intensity )
    , _position( position )
    , _cutoffDistance( cutoffDistance )
{
}

}

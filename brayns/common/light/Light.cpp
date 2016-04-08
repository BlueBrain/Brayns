/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#include "Light.h"

namespace brayns
{

Light::~Light()
{
}

Light::Light( const Vector3f& color, const float intensity )
    : _color( color )
    , _intensity( intensity )
{
}

}

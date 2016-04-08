/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#include "Material.h"

namespace brayns
{

Material::Material()
    : _color(0.f,0.f,0.f)
    , _specularColor(0.f,0.f,0.f)
    , _specularExponent(0.f)
    , _reflectionIndex(0.f)
    , _opacity(1.f)
    , _refractionIndex(1.f)
    , _emission(0.f)
{
}

}

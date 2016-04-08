/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#include "Texture2D.h"

namespace brayns
{

Texture2D::Texture2D()
    : _nbChannels(0)
    , _depth(0)
    , _width(0)
    , _height(0)
{
}

void Texture2D::setRawData(unsigned char* data, size_t size)
{
    _rawData.clear();
    _rawData.assign(data, data+size);
}

}

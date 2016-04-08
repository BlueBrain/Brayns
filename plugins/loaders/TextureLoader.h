/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include <brayns/common/types.h>
#include <brayns/common/material/Texture2D.h>

namespace brayns
{

class TextureLoader
{
public:
    TextureLoader();

    bool loadTexture(
        TexturesMap& textures,
        TextureType textureType,
        const std::string& filename);
};

}

#endif // TEXTURELOADER_H

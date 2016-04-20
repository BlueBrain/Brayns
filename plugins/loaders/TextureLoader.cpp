/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#include "TextureLoader.h"

#include <brayns/common/log.h>

#ifdef BRAYNS_USE_MAGICKPP
#  define MAGICKCORE_HDRI_ENABLE true
#  define MAGICKCORE_QUANTUM_DEPTH 32
#  include <Magick++.h>
#endif

namespace brayns
{

TextureLoader::TextureLoader()
{
}

#ifdef BRAYNS_USE_MAGICKPP
bool TextureLoader::loadTexture(
    TexturesMap& textures,
    const TextureType textureType,
    const std::string& filename)
{
    if (textures.find(filename) != textures.end())
        return true;

    try
    {
        Magick::Image image(filename);
        Magick::Blob blob;
        image.magick("RGB"); // Set JPEG output format
        image.write(&blob);
        size_t totalSize = blob.length();

        Texture2DPtr texture(new Texture2D);
        texture->setType(textureType);
        texture->setWidth(image.columns());
        texture->setHeight(image.rows());
        texture->setNbChannels(image.matte() ? 4 : 3);
        texture->setDepth(1);
        texture->setRawData((unsigned char*)blob.data(), totalSize);

        BRAYNS_INFO << filename << ": " <<
            texture->getWidth() << "x" << texture->getHeight() << "x" <<
            texture->getNbChannels() << "x" << texture->getDepth() <<
            " added to the texture cache" << std::endl;
        textures[filename] = texture;
    }
    catch( Magick::Warning &warning )
    {
        // Handle any other Magick++ warning.
        BRAYNS_WARN << warning.what() << std::endl;
        return false;
    }
    catch( Magick::ErrorFileOpen &error )
    {
        // Process Magick++ file open error
        BRAYNS_ERROR << error.what() << std::endl;
        return false;
    }
    return true;
}
#else
bool TextureLoader::loadTexture(
    TexturesMap&, const TextureType, const std::string& filename )
{
    BRAYNS_ERROR << "ImageMagick is required to load " << filename << std::endl;
    return false;
}
#endif

}

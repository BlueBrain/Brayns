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

#include "TextureLoader.h"

#include <brayns/common/log.h>

#ifdef BRAYNS_USE_MAGICKPP
#include <Magick++.h>
#endif

namespace brayns
{
TextureLoader::TextureLoader()
{
}

#ifdef BRAYNS_USE_MAGICKPP
bool TextureLoader::loadTexture(TexturesMap& textures,
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

        BRAYNS_INFO << filename << ": " << texture->getWidth() << "x"
                    << texture->getHeight() << "x" << texture->getNbChannels()
                    << "x" << texture->getDepth()
                    << " added to the texture cache" << std::endl;
        textures[filename] = texture;
    }
    catch (Magick::Warning& warning)
    {
        // Handle any other Magick++ warning.
        BRAYNS_WARN << warning.what() << std::endl;
        return false;
    }
    catch (Magick::ErrorFileOpen& error)
    {
        // Process Magick++ file open error
        BRAYNS_ERROR << error.what() << std::endl;
        return false;
    }
    return true;
}
#else
bool TextureLoader::loadTexture(TexturesMap&, const TextureType,
                                const std::string& filename)
{
    BRAYNS_ERROR << "ImageMagick is required to load " << filename << std::endl;
    return false;
}
#endif
}

/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
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

#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include <brayns/common/material/Texture2D.h>
#include <brayns/common/types.h>

namespace brayns
{
/**
 * @brief The ImageManager class uses the ImageMagick++ libray to manipulate
 * images. This class provide an API for encoding into a specific format (PNG,
 * JPEG, etc), and exporting frame buffers to the file system
 */
class ImageManager
{
public:
    ImageManager();

    /**
     * @brief Exports the contents of a frame buffer to a file. The format of
     * the file (PNG, JPG, etc) is determined by the extension of the specified
     * file name
     * @param frameBuffer Frame buffer to export
     * @param filename Full name of the file, with appropriate extension
     * @return True is export was successful, false otherwise
     */
    static bool exportFrameBufferToFile(FrameBuffer& frameBuffer,
                                        const std::string& filename);

    /**
     * @brief Import a Texture from file
     * @param textures A map of textures handled internally by Brayns
     * @param id of the texture
     * @param filename Full name of the texture file
     * @return True is import was successful, false otherwise
     */
    static bool importTextureFromFile(TexturesMap& textures, const size_t id,
                                      const std::string& filename);
};
}
#endif // IMAGEMANAGER_H

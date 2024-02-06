/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#pragma once

#include <string>

namespace brayns
{
/**
 * @brief Helper class to get the ID of an image encoding.
 *
 * The image format ID is the file extension without the dot.
 *
 */
class ImageFormat
{
public:
    /**
     * @brief Extract the image format from the given file path.
     *
     * Basically take the extension without the dot.
     *
     * @param filename Path containing info about image format.
     * @return std::string Image format ID.
     */
    static std::string fromFilename(const std::string &filename);

    /**
     * @brief Extract the image format from the given file extension.
     *
     * Basically remove the dot if present.
     *
     * @param filename Extension with or without the dot.
     * @return std::string Image format ID.
     */
    static std::string fromExtension(const std::string &extension);
};
} // namespace brayns

/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <stdexcept>
#include <string>

#include "Image.h"

namespace brayns
{
/**
 * @brief Used to decode images from files or memory.
 *
 */
class ImageDecoder
{
public:
    /**
     * @brief Load an image from given file.
     *
     * @param filename Image file path.
     * @return Image Decoded image.
     * @throw std::runtime_error Format not supported or corrupted.
     */
    static Image load(const std::string &filename);

    /**
     * @brief Decode the raw file data encoded with given format.
     *
     * @param data Image encoded data.
     * @param format Image encoding format.
     * @return Image Decoded image.
     * @throw std::runtime_error Format not supported or corrupted.
     */
    static Image decode(const std::string &data, const std::string &format);
};
} // namespace brayns

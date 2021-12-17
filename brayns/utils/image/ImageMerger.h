/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
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
#include <vector>

#include "Image.h"

namespace brayns
{
/**
 * @brief Class to merge a list of images.
 *
 */
class ImageMerger
{
public:
    /**
     * @brief Merge images horizontally.
     *
     * Images should have the same height, channel count and channel size.
     *
     * @param images Images to merge.
     * @return Image Image containing all images side-by-side.
     * @throw std::runtime_error Incompatible images.
     */
    static Image merge(const std::vector<Image> &images);
};
} // namespace brayns
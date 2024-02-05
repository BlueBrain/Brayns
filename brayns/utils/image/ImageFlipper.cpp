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

#include "ImageFlipper.h"

#include <cstring>
#include <utility>

namespace brayns
{
void ImageFlipper::flipVertically(Image &image)
{
    auto height = image.getHeight();
    auto rowSize = image.getRowSize();
    auto data = static_cast<uint8_t *>(image.getData());
    uint8_t buffer[2048];
    for (size_t i = 0; i < height / 2; ++i)
    {
        auto row0 = data + i * rowSize;
        auto row1 = data + (height - i - 1) * rowSize;
        auto remainder = rowSize;
        while (remainder)
        {
            auto size = std::min(remainder, sizeof(buffer));
            std::memcpy(buffer, row0, size);
            std::memcpy(row0, row1, size);
            std::memcpy(row1, buffer, size);
            row0 += size;
            row1 += size;
            remainder -= size;
        }
    }
}
} // namespace brayns

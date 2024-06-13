/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <doctest.h>

#include <fstream>

#include <brayns/core/codecs/JpegCodec.h>
#include <brayns/core/utils/Filesystem.h>

using namespace brayns::experimental;
using brayns::Size2;

struct TestImage
{
    std::string data;
    Size2 size;
    ImageFormat format;
};

TestImage createTestImage(ImageFormat format)
{
    auto width = std::size_t(50);
    auto height = std::size_t(50);
    auto pixelSize = getPixelSize(format);
    auto rowSize = width * pixelSize;
    auto size = rowSize * height;

    auto data = std::string(size, '\0');

    for (auto i = std::size_t(20); i < 40; ++i)
    {
        for (auto j = std::size_t(10); j < 30; ++j)
        {
            auto redIndex = i * rowSize + j * pixelSize;

            data[redIndex] = -1;

            if (format == ImageFormat::Rgba)
            {
                data[redIndex + 3] = -1;
            }
        }
    }

    return {std::move(data), {width, height}, format};
}

TEST_CASE("JpegCodec")
{
    auto image = createTestImage(ImageFormat::Rgb);

    auto data = encodeJpeg({image.data.data(), image.size, image.format});
    writeFile(data, "test1.jpg");

    image = createTestImage(ImageFormat::Rgba);

    data = encodeJpeg({image.data.data(), image.size, image.format});
    writeFile(data, "test2.jpg");
}

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

#include <brayns/core/codecs/ExrCodec.h>
#include <brayns/core/codecs/JpegCodec.h>
#include <brayns/core/codecs/PngCodec.h>
#include <brayns/core/utils/Filesystem.h>

using namespace brayns::experimental;

struct TestImage
{
    std::string data;
    Size2 size;
    ImageFormat format;
    RowOrder rowOrder;
};

TestImage createTestImage(ImageFormat format)
{
    auto width = std::size_t(200);
    auto height = std::size_t(100);
    auto pixelSize = format == ImageFormat::Rgb8 ? 3 : 4;
    auto rowSize = width * pixelSize;
    auto size = rowSize * height;
    auto rowOrder = RowOrder::BottomUp;

    auto data = std::string(size, '\0');

    for (auto i = std::size_t(0); i < 20; ++i)
    {
        for (auto j = std::size_t(0); j < 30; ++j)
        {
            auto redIndex = i * rowSize + j * pixelSize;

            data[redIndex] = -1;

            if (format == ImageFormat::Rgba8)
            {
                data[redIndex + 3] = -1;
            }
        }
    }

    return {std::move(data), {width, height}, format, rowOrder};
}

ImageView view(const TestImage &image)
{
    return {image.data.data(), image.size, image.format, image.rowOrder};
}

TEST_CASE("JpegCodec")
{
    auto image = createTestImage(ImageFormat::Rgb8);

    auto data = encodeJpeg(view(image));
    // writeFile(data, "test1.jpg");

    image = createTestImage(ImageFormat::Rgba8);

    data = encodeJpeg(view(image));
    // writeFile(data, "test2.jpg");
    (void)data;
}

TEST_CASE("PngCodec")
{
    auto image = createTestImage(ImageFormat::Rgb8);

    auto data = encodePng(view(image));
    // writeFile(data, "test1.png");

    image = createTestImage(ImageFormat::Rgba8);

    data = encodePng(view(image));
    // writeFile(data, "test2.png");
    (void)data;
}

TEST_CASE("ExrCodec")
{
    auto width = std::size_t(200);
    auto height = std::size_t(100);
    auto size = width * height;

    auto colors = std::vector<Color4>(size);
    auto depths = std::vector<float>(size);
    auto ids = std::vector<std::uint32_t>(size);
    auto idfs = std::vector<float>(size);

    for (auto i = std::size_t(0); i < 20; ++i)
    {
        for (auto j = std::size_t(0); j < 30; ++j)
        {
            auto index = i * width + j;
            colors[index] = {1, 0, 0, 1};
            depths[index] = 1.0F;
            ids[index] = 1;
            idfs[index] = 1.0F;
        }
    }

    auto image = ExrImage{
        .size = {width, height},
        .channels = {
            ExrChannel{
                .name = "R",
                .data = &colors[0][0],
                .dataType = ExrDataType::F32,
                .stride = sizeof(Color4),
            },
            ExrChannel{
                .name = "G",
                .data = &colors[0][1],
                .dataType = ExrDataType::F32,
                .stride = sizeof(Color4),
            },
            ExrChannel{
                .name = "B",
                .data = &colors[0][2],
                .dataType = ExrDataType::F32,
                .stride = sizeof(Color4),
            },
            ExrChannel{
                .name = "A",
                .data = &colors[0][3],
                .dataType = ExrDataType::F32,
                .stride = sizeof(Color4),
            },
            ExrChannel{
                .name = "Z",
                .data = &depths[0],
                .dataType = ExrDataType::F32,
            },
            ExrChannel{
                .name = "ID",
                .data = &ids[0],
                .dataType = ExrDataType::U32,
            },
            ExrChannel{
                .name = "IDF",
                .data = &idfs[0],
                .dataType = ExrDataType::F32,
            },
        }};

    auto data = encodeExr(image);
    // writeFile(data, "test.exr");
    (void)data;
}

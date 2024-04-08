/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include <brayns/core/utils/image/Image.h>
#include <brayns/core/utils/image/ImageCodecRegistry.h>
#include <brayns/core/utils/image/ImageDecoder.h>
#include <brayns/core/utils/image/ImageEncoder.h>
#include <brayns/core/utils/image/ImageFormat.h>

#include <doctest/doctest.h>

#include <filesystem>

namespace
{
class ImageFactory
{
public:
    static brayns::Image createImage(size_t width, size_t height, std::string pixels)
    {
        auto info = brayns::ImageInfo();
        info.width = width;
        info.height = height;
        info.channelCount = 1;
        info.channelSize = 1;
        return brayns::Image(info, std::move(pixels));
    }
};
}

TEST_CASE("Image info")
{
    auto info = brayns::ImageInfo();
    info.channelCount = 2;
    info.channelSize = 3;
    info.width = 16;
    info.height = 16;

    auto pixelSize = info.channelCount * info.channelSize;
    auto rowSize = info.width * pixelSize;
    auto size = rowSize * info.height;
    auto pixelIndex56 = (5 + info.width * 6);

    CHECK(info.getPixelCount() == info.width * info.height);
    CHECK(info.getPixelIndex(5, 6) == pixelIndex56);
    CHECK(info.getPixelOffset(5, 6) == pixelIndex56 * pixelSize);
    CHECK(info.getPixelSize() == pixelSize);
    CHECK(info.getRowSize() == rowSize);
    CHECK(info.getSize() == size);
}

TEST_CASE("Image")
{
    SUBCASE("Properties")
    {
        auto info = brayns::ImageInfo();
        info.channelCount = 1;
        info.channelSize = 1;
        info.width = 16;
        info.height = 16;
        auto randomPixels = std::string(info.getSize(), 0);
        auto image = brayns::Image(info, std::move(randomPixels));

        CHECK(image.getChannelCount() == info.channelCount);
        CHECK(image.getChannelSize() == info.channelSize);
        CHECK(image.getHeight() == info.height);
        CHECK(image.getWidth() == info.width);
        CHECK(image.getData());
        CHECK(image.getPixelSize() == info.getPixelSize());
        CHECK(image.getRowSize() == info.getRowSize());
        CHECK(!image.isEmpty());
    }
    SUBCASE("Empty")
    {
        auto emptyInfo = brayns::ImageInfo();
        emptyInfo.channelCount = 0;
        emptyInfo.channelSize = 0;
        emptyInfo.height = 0;
        emptyInfo.width = 0;
        auto emptyImage = brayns::Image(emptyInfo);
        CHECK(emptyImage.isEmpty());
        CHECK_THROWS_WITH(emptyImage.getData(), "Image coordinates out of bounds");

        int randomData = 0;
        CHECK_THROWS_WITH(emptyImage.write(&randomData, sizeof(int)), "Image coordinates out of bounds");
    }
    SUBCASE("Read data")
    {
        auto image = ImageFactory::createImage(5, 5, "aaaaabbbbbcccccdddddeeeee");

        CHECK(*static_cast<char *>(image.getData()) == 'a');
        CHECK(*static_cast<char *>(image.getData(0, 2)) == 'c');
        CHECK(*static_cast<char *>(image.getData(4, 4)) == 'e');
        CHECK_THROWS_WITH(image.getData(5, 0), "Image coordinates out of bounds");
        CHECK_THROWS_WITH(image.getData(0, 5), "Image coordinates out of bounds");
    }
    SUBCASE("Write data")
    {
        auto image = ImageFactory::createImage(5, 5, "aaaaaaaaaaaaaaaaaaaaaaaaa");
        auto b = 'b';

        CHECK(*static_cast<char *>(image.getData(2, 1)) == 'a');
        CHECK_NOTHROW(image.write(&b, sizeof(char), 2, 1));
        CHECK(*static_cast<char *>(image.getData(2, 1)) == 'b');
        CHECK_THROWS_WITH(image.write(&b, sizeof(char), 0, 5), "Image coordinates out of bounds");
        CHECK_THROWS_WITH(image.write(&b, sizeof(char), 5, 0), "Image coordinates out of bounds");
    }
    SUBCASE("Write image")
    {
        auto image = ImageFactory::createImage(5, 5, "aaaaaaaaaaaaaaaaaaaaaaaaa");
        auto subImage = ImageFactory::createImage(3, 2, "cccccc");
        CHECK_NOTHROW(image.write(subImage, 1, 3));

        auto expectedImage = ImageFactory::createImage(5, 5, "aaaaaaaaaaaaaaaacccaaccca");
        CHECK(image == expectedImage);
        CHECK_THROWS_WITH(image.write(subImage, 3, 1), "Image write overflow");

        auto info = brayns::ImageInfo();
        info.width = 5;
        info.height = 5;
        info.channelCount = 2;
        info.channelSize = 1;
        auto differentShapeImage = brayns::Image(info, std::string(info.getSize(), 'a'));
        CHECK_THROWS_WITH(image.write(differentShapeImage, 0, 0), "Images have different shape");
    }
    SUBCASE("Comparsion")
    {
        auto base = ImageFactory::createImage(5, 5, "aaaaaaaaaaaaaaaaaaaaaaaaa");
        auto image1 = ImageFactory::createImage(5, 5, "aaaaaaaaaaaaaaaaaaaaaaaaa");
        auto image2 = ImageFactory::createImage(5, 5, "aaaaaaaaaaaabaaaaaaaaaaaa");
        auto image3 = ImageFactory::createImage(4, 1, "aaaa");

        CHECK(base == image1);
        CHECK(base != image2);
        CHECK(base != image3);
    }
}

TEST_CASE("Image codec registry")
{
    CHECK_NOTHROW(brayns::ImageCodecRegistry::getCodec("jpg"));
    CHECK_NOTHROW(brayns::ImageCodecRegistry::getCodec("JPG"));
    CHECK_NOTHROW(brayns::ImageCodecRegistry::getCodec("png"));
    CHECK_NOTHROW(brayns::ImageCodecRegistry::getCodec("PNG"));

    CHECK_THROWS_WITH(brayns::ImageCodecRegistry::getCodec(""), doctest::Contains("Format not supported"));
    CHECK_THROWS_WITH(brayns::ImageCodecRegistry::getCodec("tif"), doctest::Contains("Format not supported"));
    CHECK_THROWS_WITH(brayns::ImageCodecRegistry::getCodec("GIF"), doctest::Contains("Format not supported"));
}

TEST_CASE("Image format")
{
    auto fullPath = "/a/path/to/an/image.jpg";
    auto relativePath = "path/to/an/image.png";
    auto nonSupportedFormat = "image.bmp";
    auto invalidFormat = "myfolder/myimage.jpg.exe";
    auto upperCaseFormat = "PNG";
    auto dotFormat = ".jpg";

    CHECK(brayns::ImageFormat::fromFilename(fullPath) == "jpg");
    CHECK(brayns::ImageFormat::fromFilename(relativePath) == "png");
    CHECK(brayns::ImageFormat::fromFilename(nonSupportedFormat) == "bmp");
    CHECK(brayns::ImageFormat::fromFilename(invalidFormat) == "exe");
    CHECK(brayns::ImageFormat::fromExtension(upperCaseFormat) == "PNG");
    CHECK(brayns::ImageFormat::fromExtension(dotFormat) == "jpg");
}

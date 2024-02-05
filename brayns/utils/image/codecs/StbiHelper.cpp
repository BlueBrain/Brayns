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

#include "StbiHelper.h"

#include <cassert>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#pragma GCC diagnostic pop

namespace
{
using namespace brayns;

class StbiDecoder
{
public:
    static Image decode(const void *data, size_t size)
    {
        return decode(static_cast<const unsigned char *>(data), int(size));
    }

    static Image decode(const unsigned char *data, int size)
    {
        int width, height, channelCount;
        auto pixels = stbi_load_from_memory(data, size, &width, &height,
                                            &channelCount, 0);
        auto info = _getImageInfo(width, height, channelCount);
        auto copy = _copyPixels(pixels, info.getSize());
        stbi_image_free(pixels);
        return {info, copy};
    }

private:
    static ImageInfo _getImageInfo(int width, int height, int channelCount)
    {
        ImageInfo info;
        info.width = size_t(width);
        info.height = size_t(height);
        info.channelCount = size_t(channelCount);
        info.channelSize = 1;
        return info;
    }

    static std::string _copyPixels(unsigned char *pixels, size_t count)
    {
        auto data = reinterpret_cast<const char *>(pixels);
        return {data, count};
    }
};

class StringBuilder
{
public:
    static void append(void *context, void *data, int size)
    {
        auto &buffer = *static_cast<std::string *>(context);
        auto str = static_cast<const char *>(data);
        buffer.append(str, size_t(size));
    }
};

class StbiPngEncoder
{
public:
    static std::string encode(const Image &image)
    {
        std::string context;
        auto width = int(image.getWidth());
        auto height = int(image.getHeight());
        auto channelCount = int(image.getChannelCount());
        assert(image.getChannelSize() == 1);
        auto data = image.getData();
        auto success =
            stbi_write_png_to_func(&StringBuilder::append, &context, width,
                                   height, channelCount, data, 0);
        if (!success)
        {
            return {};
        }
        return context;
    }
};

class StbiJpegEncoder
{
public:
    static std::string encode(const Image &image, int quality)
    {
        std::string context;
        auto width = int(image.getWidth());
        auto height = int(image.getHeight());
        auto channelCount = int(image.getChannelCount());
        assert(image.getChannelSize() == 1);
        auto data = image.getData();
        auto success =
            stbi_write_jpg_to_func(&StringBuilder::append, &context, width,
                                   height, channelCount, data, quality);
        if (!success)
        {
            return {};
        }
        return context;
    }
};
} // namespace

namespace brayns
{
Image StbiHelper::decode(const void *data, size_t size)
{
    return StbiDecoder::decode(data, size);
}

std::string StbiHelper::encodePng(const Image &image)
{
    return StbiPngEncoder::encode(image);
}

std::string StbiHelper::encodeJpeg(const Image &image, int quality)
{
    return StbiJpegEncoder::encode(image, quality);
}
} // namespace brayns

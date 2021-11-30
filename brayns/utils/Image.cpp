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

#include "Image.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <utility>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

#define STB_IMAGE_IMPLEMENTATION
#include <deps/stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <deps/stb/stb_image_write.h>

#pragma GCC diagnostic pop

#include "Filesystem.h"

namespace
{
using namespace brayns;

class ImageLoader
{
public:
    static Image load(const std::string &filename)
    {
        auto image = _loadImage(filename);
        auto data = image.data;
        if (!data)
        {
            throw std::runtime_error("Cannot load image '" + filename + "'");
        }
        ImageInfo info;
        info.width = size_t(image.width);
        info.height = size_t(image.height);
        info.channelCount = size_t(image.channelCount);
        info.channelType = image.channelType;
        return {info, data};
    }

private:
    struct StbiImage
    {
        int width = 0;
        int height = 0;
        int channelCount = 0;
        ChannelType channelType = ChannelType::Unknown;
        void *data = nullptr;

        void load(const char *path)
        {
            data = stbi_load(path, &width, &height, &channelCount, 0);
            channelType = ChannelType::Uint8;
        }

        void load16(const char *path)
        {
            data = stbi_load_16(path, &width, &height, &channelCount, 0);
            channelType = ChannelType::Uint16;
        }

        void loadf(const char *path)
        {
            data = stbi_loadf(path, &width, &height, &channelCount, 0);
            channelType = ChannelType::Float;
        }
    };

    static StbiImage _loadImage(const std::string &filename)
    {
        StbiImage image;
        auto path = filename.c_str();
        if (stbi_is_hdr(path))
        {
            image.loadf(path);
            return image;
        }
        if (stbi_is_16_bit(path))
        {
            image.load16(path);
            return image;
        }
        image.load(path);
        return image;
    }
};

class ImageAllocator
{
public:
    static void *allocate(const ImageInfo &info)
    {
        auto size = info.getSize();
        if (size == 0)
        {
            return nullptr;
        }
        return allocate(size);
    }

    static void *allocate(size_t size)
    {
        auto data = malloc(size);
        if (!data)
        {
            throw std::runtime_error("Failed to allocate image memory");
        }
        return data;
    }
};

class ImageCopy
{
public:
    static void *of(const ImageInfo &info, const void *data)
    {
        if (!data)
        {
            return nullptr;
        }
        auto size = info.getSize();
        auto copy = ImageAllocator::allocate(size);
        std::memcpy(copy, data, size);
        return copy;
    }
};

class JpegWriter
{
public:
    static void save(const Image &image, const std::string &filename)
    {
        auto path = filename.c_str();
        auto &info = image.getInfo();
        auto width = int(info.width);
        auto height = int(info.height);
        auto channelCount = int(info.channelCount);
        auto data = image.getData();
        if (!stbi_write_jpg(path, width, height, channelCount, data, 100))
        {
            throw std::runtime_error("Cannot save JPEG as '" + filename + "'");
        }
    }
};

class PngWriter
{
public:
    static void save(const Image &image, const std::string &filename)
    {
        auto path = filename.c_str();
        auto &info = image.getInfo();
        auto width = int(info.width);
        auto height = int(info.height);
        auto channelCount = int(info.channelCount);
        auto data = image.getData();
        if (!stbi_write_png(path, width, height, channelCount, data, 0))
        {
            throw std::runtime_error("Cannot save PNG as '" + filename + "'");
        }
    }
};

class HdrWriter
{
public:
    static void save(const Image &image, const std::string &filename)
    {
        auto path = filename.c_str();
        auto &info = image.getInfo();
        auto width = int(info.width);
        auto height = int(info.height);
        auto channelCount = int(info.channelCount);
        auto data = image.getFloats();
        if (!stbi_write_hdr(path, width, height, channelCount, data))
        {
            throw std::runtime_error("Cannot save HDR as '" + filename + "'");
        }
    }
};

class ImageWriter
{
public:
    static void save(const Image &image, const std::string &filename)
    {
        fs::path path(filename);
        auto extension = path.extension().string();
        if (extension == ".jpg")
        {
            JpegWriter::save(image, filename);
            return;
        }
        if (extension == ".png")
        {
            PngWriter::save(image, filename);
            return;
        }
        if (extension == ".hdr")
        {
            HdrWriter::save(image, filename);
            return;
        }
        throw std::runtime_error("Format not supported '" + filename + "'");
    }
};

class ImageFlipper
{
public:
    static void flipVertically(const Image &image)
    {
        auto &info = image.getInfo();
        auto height = info.height;
        auto rowSize = info.getRowSize();
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
                memcpy(buffer, row0, size);
                memcpy(row0, row1, size);
                memcpy(row1, buffer, size);
                row0 += size;
                row1 += size;
                remainder -= size;
            }
        }
    }
};
} // namespace

namespace brayns
{
Image Image::load(const std::string &filename)
{
    return ImageLoader::load(filename);
}

Image::Image(const ImageInfo &info)
    : _info(info)
    , _data(ImageAllocator::allocate(info))
{
}

Image::Image(const ImageInfo &info, void *data)
    : _info(info)
    , _data(data)
{
}

Image::~Image()
{
    stbi_image_free(_data);
}

Image::Image(const Image &other)
    : _info(other._info)
    , _data(ImageCopy::of(other._info, other._data))
{
}

Image::Image(Image &&other)
    : _info(other._info)
    , _data(std::exchange(other._data, nullptr))
{
}

Image &Image::operator=(const Image &other)
{
    _info = other._info;
    stbi_image_free(_data);
    _data = ImageCopy::of(other._info, _data);
    return *this;
}

Image &Image::operator=(Image &&other)
{
    std::swap(_info, other._info);
    std::swap(_data, other._data);
    return *this;
}

void Image::save(const std::string &filename) const
{
    ImageWriter::save(*this, filename);
}

void Image::flipVertically()
{
    return ImageFlipper::flipVertically(*this);
}
} // namespace brayns

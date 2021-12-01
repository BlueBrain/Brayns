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
#include "base64/base64.h"

namespace
{
using namespace brayns;

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
        info.channelSize = image.channelSize;
        return {info, data};
    }

private:
    struct StbiImage
    {
        int width = 0;
        int height = 0;
        int channelCount = 0;
        size_t channelSize = 0;
        void *data = nullptr;

        void load(const char *path)
        {
            data = stbi_load(path, &width, &height, &channelCount, 0);
            channelSize = 1;
        }

        void load16(const char *path)
        {
            data = stbi_load_16(path, &width, &height, &channelCount, 0);
            channelSize = 2;
        }

        void loadf(const char *path)
        {
            data = stbi_loadf(path, &width, &height, &channelCount, 0);
            channelSize = sizeof(float);
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

class ImageCopy
{
public:
    static void *of(const Image &image)
    {
        return of(image.getData(), image.getSize());
    }

    static void *of(const void *data, size_t size)
    {
        if (!data)
        {
            return nullptr;
        }
        auto copy = ImageAllocator::allocate(size);
        std::memcpy(copy, data, size);
        return copy;
    }
};

class PngWriter
{
public:
    static void save(const Image &image, const std::string &filename)
    {
        auto path = filename.c_str();
        auto width = int(image.getWidth());
        auto height = int(image.getHeight());
        auto channelCount = int(image.getChannelCount());
        auto data = image.getData();
        if (!stbi_write_png(path, width, height, channelCount, data, 0))
        {
            throw std::runtime_error("Cannot save PNG as '" + filename + "'");
        }
    }
};

class BitmapWriter
{
public:
    static void save(const Image &image, const std::string &filename)
    {
        auto path = filename.c_str();
        auto width = int(image.getWidth());
        auto height = int(image.getHeight());
        auto channelCount = int(image.getChannelCount());
        auto data = image.getData();
        if (!stbi_write_bmp(path, width, height, channelCount, data))
        {
            throw std::runtime_error("Cannot save BMP as '" + filename + "'");
        }
    }
};

class JpegWriter
{
public:
    static void save(const Image &image, const std::string &filename,
                     int quality)
    {
        auto path = filename.c_str();
        auto width = int(image.getWidth());
        auto height = int(image.getHeight());
        auto channelCount = int(image.getChannelCount());
        auto data = image.getData();
        if (!stbi_write_jpg(path, width, height, channelCount, data, quality))
        {
            throw std::runtime_error("Cannot save JPEG as '" + filename + "'");
        }
    }
};

class HdrWriter
{
public:
    static void save(const Image &image, const std::string &filename)
    {
        auto path = filename.c_str();
        auto width = int(image.getWidth());
        auto height = int(image.getHeight());
        auto channelCount = int(image.getChannelCount());
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
    static bool canSaveImageAs(const std::string &extension)
    {
        static const std::vector<std::string> extensions = {"png", "bmp", "jpg",
                                                            "hdr"};
        auto first = extensions.begin();
        auto last = extensions.end();
        return std::find(first, last, extension) != last;
    }

    static void save(const Image &image, const std::string &filename,
                     int quality)
    {
        fs::path path(filename);
        auto extension = path.extension().string();
        if (extension == ".png")
        {
            PngWriter::save(image, filename);
            return;
        }
        if (extension == ".bmp")
        {
            BitmapWriter::save(image, filename);
            return;
        }
        if (extension == ".jpg")
        {
            JpegWriter::save(image, filename, quality);
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
};

class ImageMerger
{
public:
    static Image merge(const std::vector<Image> &images)
    {
        auto info = _getInfo(images);
        auto result = Image::allocate(info);
        size_t offset = 0;
        for (const auto &image : images)
        {
            result.paste(image, offset);
            offset += image.getWidth();
        }
        return result;
    }

private:
    static ImageInfo _getInfo(const std::vector<Image> &images)
    {
        if (images.empty())
        {
            return {};
        }
        auto info = images[0].getInfo();
        for (size_t i = 1; i < images.size(); ++i)
        {
            _add(images[i], info);
        }
        return info;
    }

    static void _add(const Image &image, ImageInfo &info)
    {
        info.width += image.getWidth();
        if (image.getHeight() != info.height)
        {
            throw std::runtime_error("All images must have the same height");
        }
        if (image.getChannelCount() != info.channelCount)
        {
            throw std::runtime_error(
                "All images must have the same channel count");
        }
        if (image.getChannelSize() != info.channelSize)
        {
            throw std::runtime_error(
                "All images must have the same channel size");
        }
    }
};
} // namespace

namespace brayns
{
Image Image::allocate(const ImageInfo &info)
{
    return {info, ImageAllocator::allocate(info)};
}

Image Image::load(const std::string &filename)
{
    return ImageLoader::load(filename);
}

Image Image::merge(const std::vector<Image> &images)
{
    return ImageMerger::merge(images);
}

bool Image::canBeSavedAs(const std::string &extension)
{
    return ImageWriter::canSaveImageAs(extension);
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
    , _data(ImageCopy::of(other))
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
    _data = ImageCopy::of(other);
    return *this;
}

Image &Image::operator=(Image &&other)
{
    std::swap(_info, other._info);
    std::swap(_data, other._data);
    return *this;
}

void Image::save(const std::string &filename, int quality) const
{
    ImageWriter::save(*this, filename, quality);
}

std::string Image::toBase64() const
{
    auto data = static_cast<const unsigned char *>(_data);
    auto size = getSize();
    return base64_encode(data, size);
}

void Image::flipVertically()
{
    return ImageFlipper::flipVertically(*this);
}

void Image::paste(const Image &image, size_t x, size_t y)
{
    auto source = image.getData();
    auto size = image.getSize();
    auto bytes = getBytes();
    auto destination = bytes + getPixelOffset(x, y);
    assert(destination + size <= bytes + getSize());
    std::memcpy(destination, source, size);
}

void Image::assign(const void *data, size_t size, size_t offset)
{
    assert(offset + size < getSize());
    auto destination = getBytes() + offset;
    std::memcpy(destination, data, size);
}
} // namespace brayns

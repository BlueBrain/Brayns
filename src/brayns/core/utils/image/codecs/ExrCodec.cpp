/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "ExrCodec.h"

#include <zlib.h>

#define TINYEXR_IMPLEMENTATION
#define TINYEXR_USE_MINIZ 0
#include "tinyexr.h"

#include <cstring>

namespace
{
template<typename T>
struct TinyExrPixelType;

template<>
struct TinyExrPixelType<unsigned int>
{
    static constexpr auto type = TINYEXR_PIXELTYPE_UINT;
};

template<>
struct TinyExrPixelType<float>
{
    static constexpr auto type = TINYEXR_PIXELTYPE_FLOAT;
};

struct TinyExrBuffer
{
    std::vector<std::vector<unsigned char>> channels;
    std::vector<unsigned char *> flippedChannels;
    std::vector<EXRChannelInfo> channelInfos;
    std::vector<int> pixelTypes;
};

struct TinyExrFrame
{
    EXRImage image;
    EXRHeader header;
    TinyExrBuffer buffer;
};

template<typename T>
class TinyExrFrameFactory
{
public:
    static TinyExrFrame create(const std::string &name, const brayns::Image &input)
    {
        auto channelCount = input.getChannelCount();

        auto frame = TinyExrFrame();
        auto &buffer = frame.buffer;

        buffer.channels = _splitImageChannels(input);
        buffer.flippedChannels = _flipChannels(buffer.channels);
        buffer.channelInfos = _createChannelInfos(channelCount);
        buffer.pixelTypes = _createPixelTypes(channelCount);

        frame.image = _initializeExrImage(buffer.flippedChannels, input.getWidth(), input.getHeight());
        frame.header = _initializeExrHeader(frame.image, name, buffer.channelInfos, buffer.pixelTypes);

        return frame;
    }

private:
    static inline const std::vector<std::string> channelNames = {"R", "G", "B", "A"};

private:
    static std::vector<std::vector<unsigned char>> _allocateChannels(size_t count, size_t resolution)
    {
        auto imageChannels = std::vector<std::vector<unsigned char>>(count);
        for (auto &channel : imageChannels)
        {
            channel.resize(resolution * sizeof(T));
        }
        return imageChannels;
    }

    static std::vector<std::vector<unsigned char>> _splitImageChannels(const brayns::Image &image)
    {
        auto channelCount = image.getChannelCount();
        auto channelSize = image.getChannelSize();
        auto width = image.getWidth();
        auto resolution = width * image.getHeight();

        auto imageChannels = _allocateChannels(channelCount, resolution);

        for (auto i = 0ul; i < resolution; i++)
        {
            auto x = i % width;
            auto y = i / width;
            auto pixel = image.getData(x, y);
            auto bytes = reinterpret_cast<const unsigned char *>(pixel);

            for (size_t j = 0; j < channelCount; ++j)
            {
                auto &imageChannel = imageChannels[j];
                auto &exrChannelValue = imageChannel[i * sizeof(T)];
                std::memcpy(&exrChannelValue, bytes + j * channelSize, channelSize);
            }
        }

        return imageChannels;
    }

    static std::vector<unsigned char *> _flipChannels(std::vector<std::vector<unsigned char>> &channels)
    {
        auto channelCount = channels.size();

        auto flippedChannels = std::vector<unsigned char *>();
        flippedChannels.reserve(channelCount);

        for (size_t i = 0; i < channelCount; ++i)
        {
            auto index = channelCount - i - 1;
            flippedChannels.push_back(channels[index].data());
        }

        return flippedChannels;
    }

    static std::vector<EXRChannelInfo> _createChannelInfos(size_t channelCount)
    {
        auto infos = std::vector<EXRChannelInfo>();
        infos.reserve(channelCount);

        for (size_t i = 0; i < channelCount; ++i)
        {
            auto &info = infos.emplace_back();
            auto index = channelCount - 1 - i;
            strncpy(info.name, channelNames[index].data(), 255);
        }

        return infos;
    }

    static std::vector<int> _createPixelTypes(size_t channelCount)
    {
        auto types = std::vector<int>();
        types.reserve(channelCount);

        for (size_t i = 0; i < channelCount; ++i)
        {
            types.emplace_back(TinyExrPixelType<T>::type);
        }

        return types;
    }

    static EXRImage _initializeExrImage(std::vector<unsigned char *> &channels, size_t width, size_t height)
    {
        EXRImage image;
        InitEXRImage(&image);

        image.num_channels = static_cast<int>(channels.size());
        image.images = channels.data();
        image.width = static_cast<int>(width);
        image.height = static_cast<int>(height);

        return image;
    }

    static EXRHeader _initializeExrHeader(
        const EXRImage &image,
        const std::string &name,
        std::vector<EXRChannelInfo> &infos,
        std::vector<int> &pixelTypes)
    {
        EXRHeader header;
        InitEXRHeader(&header);

        strncpy(header.name, name.data(), 255);
        header.num_channels = image.num_channels;
        header.channels = infos.data();
        header.pixel_types = pixelTypes.data();
        header.requested_pixel_types = pixelTypes.data();

        return header;
    }
};

class TinyExrTypeDispatcher
{
public:
    static TinyExrFrame dispatch(const brayns::ExrFrame &frame)
    {
        return dispatch(frame.name, frame.image);
    }

    static TinyExrFrame dispatch(const std::string &name, const brayns::Image &input)
    {
        auto type = input.getDataType();
        switch (type)
        {
        case brayns::ImageDataType::Float:
            return TinyExrFrameFactory<float>::create(name, input);
        case brayns::ImageDataType::UnsignedInt:
            return TinyExrFrameFactory<unsigned int>::create(name, input);
        }

        throw std::runtime_error("Unhandled image data type");
    }
};

class TinyExrEconderHelper
{
public:
    static std::string encode(const brayns::Image &input)
    {
        auto frame = TinyExrTypeDispatcher::dispatch("deafult", input);
        return _encode(frame);
    }

    static std::string encode(const std::vector<brayns::ExrFrame> &frames)
    {
        auto processedFrames = std::vector<TinyExrFrame>();
        processedFrames.reserve(frames.size());

        for (auto &frame : frames)
        {
            processedFrames.push_back(TinyExrTypeDispatcher::dispatch(frame.name, frame.image));
        }

        return _encode(processedFrames);
    }

private:
    static std::string _encode(TinyExrFrame &frame)
    {
        unsigned char *memory = nullptr;
        const char *err = nullptr;
        auto writtenByteCount = SaveEXRImageToMemory(&frame.image, &frame.header, &memory, &err);
        return _buildString(memory, writtenByteCount, err);
    }

    static std::string _encode(std::vector<TinyExrFrame> &frames)
    {
        auto count = static_cast<unsigned int>(frames.size());
        auto images = _buildImageList(frames);
        auto headers = _buildHeaderList(frames);
        unsigned char *memory = nullptr;
        const char *err = nullptr;
        auto writtenByteCount = SaveEXRMultipartImageToMemory(images.data(), headers.data(), count, &memory, &err);
        return _buildString(memory, writtenByteCount, err);
    }

    static std::string _buildString(unsigned char *memory, size_t size, const char *error)
    {
        if (size == 0)
        {
            auto errorMessage = std::string(error);
            FreeEXRErrorMessage(error);
            throw std::runtime_error("Error encoding EXR image: " + errorMessage);
        }

        auto encodedImage = std::string(reinterpret_cast<const char *>(memory), size);
        free(memory);

        return encodedImage;
    }

    static std::vector<EXRImage> _buildImageList(const std::vector<TinyExrFrame> &frames)
    {
        auto imageList = std::vector<EXRImage>();
        imageList.reserve(frames.size());

        for (auto &frame : frames)
        {
            imageList.push_back(frame.image);
        }

        return imageList;
    }

    static std::vector<const EXRHeader *> _buildHeaderList(const std::vector<TinyExrFrame> &frames)
    {
        auto headerList = std::vector<const EXRHeader *>();
        headerList.reserve(frames.size());

        for (auto &frame : frames)
        {
            headerList.push_back(&frame.header);
        }

        return headerList;
    }
};

class TinyExrDecoderHelper
{
public:
    static brayns::Image decode(const void *data, size_t size)
    {
        float *rgba = nullptr;
        int width;
        int height;
        auto memory = reinterpret_cast<const unsigned char *>(data);
        const char *error;
        auto result = LoadEXRFromMemory(&rgba, &width, &height, memory, size, &error);

        if (result != TINYEXR_SUCCESS)
        {
            auto message = std::string(error);
            FreeEXRErrorMessage(error);
            throw std::runtime_error("Error decoding EXR image: " + message);
        }

        auto imageInfo = brayns::ImageInfo();
        imageInfo.channelCount = 4;
        imageInfo.channelSize = sizeof(float);
        imageInfo.dataType = brayns::ImageDataType::Float;
        imageInfo.width = static_cast<size_t>(width);
        imageInfo.height = static_cast<size_t>(height);

        auto imageData = std::string(reinterpret_cast<const char *>(rgba), imageInfo.getSize());
        free(rgba);

        return brayns::Image(imageInfo, std::move(imageData));
    }
};
}

namespace brayns
{
std::string ExrCodec::getFormat() const
{
    return "exr";
}

std::string ExrCodec::encode(const Image &image, int quality, const std::optional<ImageMetadata> &metadata) const
{
    (void)quality;
    (void)metadata;
    return TinyExrEconderHelper::encode(image);
}

std::string ExrCodec::encode(const std::vector<ExrFrame> &frames) const
{
    return TinyExrEconderHelper::encode(frames);
}

Image ExrCodec::decode(const void *data, size_t size) const
{
    return TinyExrDecoderHelper::decode(data, size);
}
}

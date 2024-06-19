/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include "ExrCodec.h"

#include <ImathVec.h>
#include <ImfChannelList.h>
#include <ImfFrameBuffer.h>
#include <ImfHeader.h>
#include <ImfOutputFile.h>
#include <ImfPixelType.h>
#include <ImfStdIO.h>

namespace
{
using namespace brayns::experimental;

Imf::PixelType getPixelType(ExrDataType type)
{
    switch (type)
    {
    case ExrDataType::U32:
        return Imf::UINT;
    case ExrDataType::F32:
        return Imf::FLOAT;
    default:
        throw std::runtime_error("Invalid EXR pixel type");
    }
}

std::size_t getPixelSize(ExrDataType type)
{
    switch (type)
    {
    case ExrDataType::U32:
    case ExrDataType::F32:
        return 4;
    default:
        throw std::runtime_error("Invalid EXR pixel type");
    }
}

std::vector<std::string> packChannels(const ExrImage &image)
{
    const auto &channels = image.channels;

    auto packedChannels = std::vector<std::string>();
    packedChannels.reserve(channels.size());

    auto [width, height] = image.size;
    auto pixelCount = width * height;

    for (const auto &channel : channels)
    {
        auto pixelSize = getPixelSize(channel.dataType);
        auto channelSize = pixelCount * pixelSize;

        auto packedChannel = std::string();
        packedChannel.reserve(channelSize);

        auto bytes = static_cast<const char *>(channel.data);
        auto strideX = channel.stride == 0 ? pixelSize : channel.stride;
        auto strideY = width * strideX;

        auto topDown = image.rowOrder == RowOrder::TopDown;

        for (auto y = std::size_t(0); y < height; ++y)
        {
            auto rowIndex = topDown ? y : height - y - 1;

            auto offsetY = rowIndex * strideY;

            for (auto x = std::size_t(0); x < width; ++x)
            {
                auto offsetX = offsetY + x * strideX;

                packedChannel.append(bytes + offsetX, pixelSize);
            }
        }

        packedChannels.push_back(std::move(packedChannel));
    }

    return packedChannels;
}
}

namespace brayns::experimental
{
std::string encodeExr(const ExrImage &image)
{
    auto width = static_cast<int>(image.size[0]);
    auto height = static_cast<int>(image.size[1]);
    auto origin = Imath::V2i(0, 0);

    auto header = Imf::Header(width, height);
    auto &channels = header.channels();

    auto framebuffer = Imf::FrameBuffer();

    auto packedChannels = packChannels(image);

    for (auto i = std::size_t(0); i < packedChannels.size(); ++i)
    {
        auto &channel = image.channels[i];
        auto &packedChannel = packedChannels[i];

        const auto &name = channel.name;
        auto type = getPixelType(channel.dataType);
        auto *data = packedChannel.data();

        channels.insert(name, Imf::Channel(type));
        framebuffer.insert(name, Imf::Slice::Make(type, data, origin, width, height));
    }

    auto stream = Imf::StdOSStream();

    {
        auto output = Imf::OutputFile(stream, header);
        output.setFrameBuffer(framebuffer);
        output.writePixels(height);
    }

    return stream.str();
}
}

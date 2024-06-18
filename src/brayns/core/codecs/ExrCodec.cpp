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
}

namespace brayns::experimental
{
std::string encodeExr(const ExrImage &image)
{
    auto width = static_cast<int>(image.size[0]);
    auto height = static_cast<int>(image.size[1]);
    auto lineOrder = image.rowOrder == RowOrder::TopDown ? Imf::INCREASING_Y : Imf::DECREASING_Y;

    auto header = Imf::Header(width, height);
    header.lineOrder() = lineOrder;

    auto &channels = header.channels();
    auto framebuffer = Imf::FrameBuffer();

    for (const auto &channel : image.channels)
    {
        auto name = channel.name;
        auto type = getPixelType(channel.dataType);
        auto strideX = channel.stride;
        auto data = channel.data;
        auto origin = Imath::V2f(0.0F, 0.0F);

        channels.insert(name, Imf::Channel(type));
        framebuffer.insert(name, Imf::Slice::Make(type, data, origin, width, height, strideX));
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

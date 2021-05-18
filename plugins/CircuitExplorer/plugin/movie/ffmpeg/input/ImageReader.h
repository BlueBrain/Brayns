/* Copyright (c) 2021, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Adrien Fleury <adrien.fleury@epfl.ch>
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

#pragma once

#include <string>

#include "../common/Frame.h"
#include "../common/Packet.h"
#include "Decoder.h"
#include "ImageDecoder.h"
#include "InputContext.h"
#include "InputVideoStream.h"

namespace ffmpeg
{
class ImageReader
{
public:
    static FramePtr read(const std::string& filename)
    {
        auto context = InputContext::create(filename);
        auto codec = _createCodec(context.get());
        auto packet = _readRawImage(context.get());
        auto frame = _decodeImage(codec.get(), packet.get());
        return frame;
    }

private:
    static CodecContextPtr _createCodec(AVFormatContext* context)
    {
        auto codec = InputVideoStream::findCodec(context);
        return Decoder::create(codec);
    }

    static PacketPtr _readRawImage(AVFormatContext* context)
    {
        auto packet = Packet::create();
        _readPacket(context, packet.get());
        return packet;
    }

    static void _readPacket(AVFormatContext* context, AVPacket* packet)
    {
        Status status = av_read_frame(context, packet);
        if (!status.isSuccess())
        {
            throw Exception("Cannot read input packet", status);
        }
    }

    static FramePtr _decodeImage(AVCodecContext* codec, AVPacket* packet)
    {
        ImageDecoder decoder(codec);
        decoder.sendPacket(packet);
        decoder.flush();
        auto frame = Frame::create();
        decoder.receiveFrame(frame.get());
        return frame;
    }
};
} // namespace ffmpeg
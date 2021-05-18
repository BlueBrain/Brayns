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

#include "../Exception.h"
#include "../Status.h"
#include "../VideoInfo.h"
#include "../common/Frame.h"
#include "../common/Packet.h"
#include "Encoder.h"
#include "OutputContext.h"
#include "OutputVideoStream.h"
#include "VideoEncoder.h"
#include "VideoTimer.h"
#include "VideoWriter.h"

namespace ffmpeg
{
class VideoStream
{
public:
    VideoStream(const VideoInfo& info)
        : _info(info)
    {
        _timer.setFramerate(_info.framerate);
    }

    void open(const std::string& filename)
    {
        _context = OutputContext::create(filename);
        _stream = OutputVideoStream::add(_context.get(), _info);
        _encoderContext = _createEncoder();
        _encoder.open(_encoderContext.get());
        _writer.open(_context.get());
        _timer.setTimebase(_stream->time_base);
    }

    void write(AVFrame* frame)
    {
        _encoder.sendFrame(frame);
        auto packet = Packet::create();
        if (!_encoder.receivePacket(packet.get()))
        {
            return;
        }
        _writePacket(packet.get());
    }

    void close()
    {
        _flushEncoder();
        _writer.close();
    }

private:
    CodecContextPtr _createEncoder()
    {
        EncoderInfo encoderInfo;
        encoderInfo.context = _context.get();
        encoderInfo.codec = _info.codec;
        encoderInfo.stream = _stream;
        encoderInfo.framerate = _info.framerate;
        return Encoder::create(encoderInfo);
    }

    void _writePacket(AVPacket* packet)
    {
        packet->stream_index = _stream->index;
        _timer.setupPacket(packet);
        _writer.write(packet);
    }

    void _flushEncoder()
    {
        _encoder.flush();
        auto packet = Packet::create();
        while (_encoder.receivePacket(packet.get()))
        {
            _writePacket(packet.get());
            packet = Packet::create();
        }
    }

    VideoInfo _info;
    FormatContextPtr _context;
    VideoWriter _writer;
    AVStream* _stream;
    CodecContextPtr _encoderContext;
    VideoEncoder _encoder;
    VideoTimer _timer;
};
} // namespace ffmpeg
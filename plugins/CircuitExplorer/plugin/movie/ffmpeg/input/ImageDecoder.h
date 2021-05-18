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

extern "C"
{
#include <libavcodec/avcodec.h>
}

#include "../Exception.h"
#include "../Status.h"

namespace ffmpeg
{
class ImageDecoder
{
public:
    ImageDecoder(AVCodecContext* context)
        : _context(context)
    {
    }

    void sendPacket(AVPacket* packet)
    {
        Status status = avcodec_send_packet(_context, packet);
        if (!status.isSuccess())
        {
            throw Exception("Cannot send input packet", status);
        }
    }

    void flush()
    {
        Status status = avcodec_send_packet(_context, nullptr);
        if (!status.isSuccess())
        {
            throw Exception("Cannot flush input decoder", status);
        }
    }

    void receiveFrame(AVFrame* frame)
    {
        Status status = avcodec_receive_frame(_context, frame);
        if (!status.isSuccess())
        {
            throw Exception("Cannot receive decoded frame", status);
        }
    }

private:
    AVCodecContext* _context;
};
} // namespace ffmpeg
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
#include "../common/CodecContext.h"

namespace ffmpeg
{
class Decoder
{
public:
    static CodecContextPtr create(AVCodecParameters* parameters)
    {
        auto codec = _findDecoder(parameters);
        auto context = CodecContext::create(codec);
        _setupContext(context.get(), parameters);
        _openContext(context.get(), codec);
        return context;
    }

private:
    static AVCodec* _findDecoder(AVCodecParameters* parameters)
    {
        auto codec = avcodec_find_decoder(parameters->codec_id);
        if (!codec)
        {
            throw Exception("Cannot find decoder");
        }
        return codec;
    }

    static void _setupContext(AVCodecContext* context,
                              AVCodecParameters* parameters)
    {
        Status status = avcodec_parameters_to_context(context, parameters);
        if (!status.isSuccess())
        {
            throw Exception("Cannot fill decoder context", status);
        }
    }

    static void _openContext(AVCodecContext* context, AVCodec* codec)
    {
        Status status = avcodec_open2(context, codec, nullptr);
        if (!status.isSuccess())
        {
            throw Exception("Cannot open decoder", status);
        }
    }
};
} // namespace ffmpeg
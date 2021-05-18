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
#include <libavutil/opt.h>
}

#include "../Exception.h"
#include "../common/CodecContext.h"
#include "EncoderInfo.h"

namespace ffmpeg
{
class Encoder
{
public:
    static CodecContextPtr create(const EncoderInfo& info)
    {
        auto context = CodecContext::create(info.codec);
        _setup(context.get(), info.stream->codecpar);
        _loadInfo(context.get(), info);
        _openContext(context.get(), info.codec);
        _updateStream(context.get(), info.stream);
        return context;
    }

private:
    static void _setup(AVCodecContext* context, AVCodecParameters* parameters)
    {
        Status status = avcodec_parameters_to_context(context, parameters);
        if (!status.isSuccess())
        {
            throw Exception("Cannot setup encoder context", status);
        }
    }

    static void _loadInfo(AVCodecContext* context, const EncoderInfo& info)
    {
        context->time_base = {1, info.framerate};
        if (info.stream->codecpar->codec_id == AV_CODEC_ID_H264)
        {
            av_opt_set(context->priv_data, "preset", "ultrafast", 0);
        }
        if (info.context->oformat->flags & AVFMT_GLOBALHEADER)
        {
            context->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        }
    }

    static void _openContext(AVCodecContext* context, AVCodec* codec)
    {
        Status status = avcodec_open2(context, codec, nullptr);
        if (!status.isSuccess())
        {
            throw Exception("Cannot open encoder", status);
        }
    }

    static void _updateStream(AVCodecContext* context, AVStream* stream)
    {
        Status status =
            avcodec_parameters_from_context(stream->codecpar, context);
        if (!status.isSuccess())
        {
            throw Exception("Cannot setup video stream", status);
        }
    }
};
} // namespace ffmpeg
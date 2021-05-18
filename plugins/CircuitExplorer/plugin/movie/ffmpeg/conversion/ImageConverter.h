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
#include <libswscale/swscale.h>
}

#include "../Exception.h"
#include "../Status.h"
#include "../common/Frame.h"
#include "ConversionContext.h"
#include "ConversionInfo.h"

namespace ffmpeg
{
class ImageConverter
{
public:
    static FramePtr convert(const AVFrame* frame, const ConversionInfo& info)
    {
        auto context = _createContext(frame, info);
        auto newFrame = Frame::create();
        _setupNewFrame(newFrame.get(), info);
        _convert(context.get(), frame, newFrame.get());
        return newFrame;
    }

private:
    static ConversionContextPtr _createContext(const AVFrame* frame,
                                               const ConversionInfo& info)
    {
        auto frameInfo = ConversionInfo::fromFrame(frame);
        return ConversionContext::create(frameInfo, info);
    }

    static void _setupNewFrame(AVFrame* frame, const ConversionInfo& info)
    {
        info.toFrame(frame);
        Status status = av_frame_get_buffer(frame, 0);
        if (!status.isSuccess())
        {
            throw Exception("Cannot setup new frame", status);
        }
    }

    static void _convert(SwsContext* context, const AVFrame* source,
                         AVFrame* destination)
    {
        sws_scale(context, source->data, source->linesize, 0, source->height,
                  destination->data, destination->linesize);
    }
};
} // namespace ffmpeg
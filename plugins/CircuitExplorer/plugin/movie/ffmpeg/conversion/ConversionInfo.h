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
#include <libavutil/frame.h>
}

namespace ffmpeg
{
struct ConversionInfo
{
    int width = 0;
    int height = 0;
    AVPixelFormat format = AV_PIX_FMT_NONE;

    static ConversionInfo fromFrame(const AVFrame* frame)
    {
        ConversionInfo info;
        info.width = frame->width;
        info.height = frame->height;
        info.format = AVPixelFormat(frame->format);
        return info;
    }

    void toFrame(AVFrame* frame) const
    {
        frame->width = width;
        frame->height = height;
        frame->format = format;
    }
};
} // namespace ffmpeg
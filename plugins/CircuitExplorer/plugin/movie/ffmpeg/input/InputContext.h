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

extern "C"
{
#include <libavformat/avformat.h>
}

#include "../Exception.h"
#include "../Status.h"
#include "../common/FormatContext.h"

namespace ffmpeg
{
class InputContext
{
public:
    static FormatContextPtr create(const std::string& filename)
    {
        auto context = _openInput(filename);
        _findStreamInfo(context.get());
        return context;
    }

private:
    static FormatContextPtr _openInput(const std::string& filename)
    {
        AVFormatContext* context = nullptr;
        Status status =
            avformat_open_input(&context, filename.c_str(), nullptr, nullptr);
        if (!status.isSuccess())
        {
            throw Exception("Cannot open input context", status);
        }
        return FormatContextPtr(context);
    }

    static void _findStreamInfo(AVFormatContext* context)
    {
        Status status = avformat_find_stream_info(context, nullptr);
        if (!status.isSuccess())
        {
            throw Exception("Cannot find input stream info", status);
        }
    }
};
} // namespace ffmpeg
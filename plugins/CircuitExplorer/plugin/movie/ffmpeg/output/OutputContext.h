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
class OutputContext
{
public:
    static FormatContextPtr create(const std::string& filename)
    {
        AVFormatContext* context = nullptr;
        Status status =
            avformat_alloc_output_context2(&context, nullptr, nullptr,
                                           filename.c_str());
        if (!status.isSuccess())
        {
            throw Exception("Cannot allocate output context", status);
        }
        return FormatContextPtr(context);
    }
};
} // namespace ffmpeg
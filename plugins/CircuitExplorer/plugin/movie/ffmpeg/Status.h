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
#include <libavutil/common.h>
#include <libavutil/error.h>
}

namespace ffmpeg
{
class Status
{
public:
    Status() = default;

    Status(int code)
        : _code(code)
    {
    }

    int getCode() const { return _code; }

    bool isError() const { return _code < 0; }

    bool isSuccess() const { return _code == 0; }

    bool isRetry() const
    {
        return _code == AVERROR(EAGAIN) || _code == AVERROR_EOF;
    }

    std::string toString() const
    {
        char buffer[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(_code, buffer, sizeof(buffer));
        return buffer;
    }

    operator int() const { return _code; }

private:
    int _code = 0;
};
} // namespace ffmpeg
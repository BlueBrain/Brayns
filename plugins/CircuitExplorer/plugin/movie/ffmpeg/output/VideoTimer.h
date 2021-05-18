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

namespace ffmpeg
{
class VideoTimer
{
public:
    void setFramerate(int framerate) { _framerate = framerate; }

    void setTimebase(AVRational timebase)
    {
        _frameDuration = timebase.den / (_framerate * timebase.num);
    }

    void setupPacket(AVPacket* packet)
    {
        packet->pts = getTimestamp();
        packet->dts = packet->pts;
        packet->duration = _frameDuration;
        ++_frameCount;
    }

    int64_t getTimestamp() const { return _frameCount * _frameDuration; }

private:
    int64_t _framerate = 0;
    int64_t _frameCount = 0;
    int64_t _frameDuration = 0;
};
} // namespace ffmpeg
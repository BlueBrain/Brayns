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

#include <memory>
#include <utility>

extern "C"
{
#include <libavcodec/avcodec.h>
}

#include "../Exception.h"

namespace ffmpeg
{
class Packet
{
public:
    struct Deleter
    {
        void operator()(AVPacket* packet) const { av_packet_free(&packet); }
    };

    using Ptr = std::unique_ptr<AVPacket, Deleter>;

    static Ptr create()
    {
        auto packet = av_packet_alloc();
        if (!packet)
        {
            throw Exception("Cannot allocate packet");
        }
        return Ptr(packet);
    }
};

using PacketPtr = Packet::Ptr;
} // namespace ffmpeg
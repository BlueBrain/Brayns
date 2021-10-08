/* Copyright (c) 2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel Nachbaur <daniel.nachbaur@epfl.ch>
 *
 * This file is part of https://github.com/BlueBrain/Brayns
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

#include "DeflectParameters.h"
#include <brayns/common/utils/enumUtils.h>
#include <deflect/deflect.h>

#define deflectPixelOp deflectPixelOp
#define TEXTIFY(A) #A

namespace brayns
{
template <>
inline std::vector<std::pair<std::string, deflect::ChromaSubsampling>> enumMap()
{
    return {{"yuv444", deflect::ChromaSubsampling::YUV444},
            {"yuv422", deflect::ChromaSubsampling::YUV422},
            {"yuv420", deflect::ChromaSubsampling::YUV420}};
}
namespace utils
{
/**
 * Decode the framebuffer name to know the eye pass. Defined by plugins that
 * follow our convention, and by default in Brayns.cpp
 */
inline deflect::View getView(const std::string& name)
{
    if (name.length() == 2)
    {
        if (name.at(1) == 'L')
            return deflect::View::left_eye;
        if (name.at(1) == 'R')
            return deflect::View::right_eye;
        return deflect::View::mono;
    }
    return deflect::View::mono;
}

/**
 * Decode the framebuffer name to know the surface. Defined by plugins that
 * follow our convention, and by default in Brayns.cpp
 */
inline uint8_t getChannel(const std::string& name)
{
    if (name.length() == 2)
        return std::stoi(&name.at(0));
    return 0;
}

inline bool needsReset(const deflect::Observer& stream,
                       const DeflectParameters& params)
{
    const bool changed = stream.getId() != params.getId() ||
                         stream.getPort() != params.getPort() ||
                         stream.getHost() != params.getHostname();

    return changed || !stream.isConnected() || !params.getEnabled();
}
} // namespace utils
} // namespace brayns

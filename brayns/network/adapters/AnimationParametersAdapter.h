/* Copyright (c) 2021 EPFL/Blue Brain Project
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include <brayns/network/json/MessageAdapter.h>

#include <brayns/parameters/AnimationParameters.h>

namespace brayns
{
BRAYNS_ADAPTER_BEGIN(AnimationParameters)
BRAYNS_ADAPTER_GETSET("frame_count", getNumFrames, setNumFrames,
                      "Animation frame count")
BRAYNS_ADAPTER_GETSET("current", getFrame, setFrame, "Current frame index")
BRAYNS_ADAPTER_GETSET("delta", getDelta, setDelta, "Frame delta")
BRAYNS_ADAPTER_GETSET("dt", getDt, setDt, "Frame time")
BRAYNS_ADAPTER_GETSET("playing", isPlaying, setPlaying, "Animation is playing")
BRAYNS_ADAPTER_GETSET("unit", getUnit, setUnit, "Time unit")
BRAYNS_ADAPTER_END()
} // namespace brayns
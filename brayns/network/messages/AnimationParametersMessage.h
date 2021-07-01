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

#include <brayns/network/message/Message.h>

#include <brayns/parameters/AnimationParameters.h>

namespace brayns
{
BRAYNS_MESSAGE_BEGIN(AnimationParametersParams)
BRAYNS_MESSAGE_ENTRY(uint32_t, frame_count, "Animation frame count");
BRAYNS_MESSAGE_ENTRY(uint32_t, current, "Current frame index");
BRAYNS_MESSAGE_ENTRY(int32_t, delta, "Frame delta");
BRAYNS_MESSAGE_ENTRY(double, dt, "Frame time");
BRAYNS_MESSAGE_ENTRY(bool, playing, "Animation is playing");
BRAYNS_MESSAGE_ENTRY(std::string, unit, "Time unit");
BRAYNS_MESSAGE_END()

using AnimationParametersResult = AnimationParametersParams;
} // namespace brayns
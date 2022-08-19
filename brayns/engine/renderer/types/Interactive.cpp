/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "Interactive.h"

#include <brayns/engine/common/MathTypesOsprayTraits.h>

namespace
{
struct InteractiveParameters
{
    inline static const std::string spp = "pixelSamples";
    inline static const std::string pathLength = "maxPathLength";
    inline static const std::string background = "backgroundColor";
    inline static const std::string shadow = "shadows";
    inline static const std::string aoSamples = "aoSamples";
};
}

namespace brayns
{
void RendererData<Interactive>::update(ospray::cpp::Renderer &handle, Interactive &data)
{
    handle.setParam(InteractiveParameters::spp, data.samplesPerPixel);
    handle.setParam(InteractiveParameters::pathLength, data.maxRayBounces);
    handle.setParam(InteractiveParameters::background, data.backgroundColor);
    handle.setParam(InteractiveParameters::shadow, data.shadowsEnabled);
    handle.setParam(InteractiveParameters::aoSamples, data.aoSamples);
}
}

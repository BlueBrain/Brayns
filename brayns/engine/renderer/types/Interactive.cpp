/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include <ospray/ospray_cpp/ext/rkcommon.h>

namespace
{
struct InteractiveParameters
{
    static inline const std::string spp = "pixelSamples";
    static inline const std::string pathLength = "maxPathLength";
    static inline const std::string background = "backgroundColor";
    static inline const std::string shadow = "shadows";
    static inline const std::string aoSamples = "aoSamples";
};
}

namespace brayns
{
void RendererTraits<Interactive>::checkParameters(const Interactive &data)
{
    if (data.aoSamples > static_cast<size_t>(std::numeric_limits<int>::max()))
    {
        throw std::invalid_argument("ao samples cannot be higher than 2^31");
    }
    if (data.maxRayBounces > static_cast<size_t>(std::numeric_limits<int>::max()))
    {
        throw std::invalid_argument("ray bounces cannot be higher than 2^31");
    }
    if (data.samplesPerPixel > static_cast<size_t>(std::numeric_limits<int>::max()))
    {
        throw std::invalid_argument("samples per pixel cannot be higher than 2^31");
    }
}

void RendererTraits<Interactive>::updateData(ospray::cpp::Renderer &handle, Interactive &data)
{
    handle.setParam(InteractiveParameters::spp, static_cast<int>(data.samplesPerPixel));
    handle.setParam(InteractiveParameters::pathLength, static_cast<int>(data.maxRayBounces));
    handle.setParam(InteractiveParameters::background, data.backgroundColor);
    handle.setParam(InteractiveParameters::shadow, data.shadowsEnabled);
    handle.setParam(InteractiveParameters::aoSamples, static_cast<int>(data.aoSamples));
}
}

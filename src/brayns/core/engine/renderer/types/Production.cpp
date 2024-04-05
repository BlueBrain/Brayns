/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include "Production.h"

#include <ospray/ospray_cpp/ext/rkcommon.h>

namespace
{
struct ProductionParameters
{
    static inline const std::string spp = "pixelSamples";
    static inline const std::string pathLength = "maxPathLength";
    static inline const std::string background = "backgroundColor";
};
}

namespace brayns
{
void RendererTraits<Production>::checkParameters(const Production &data)
{
    if (data.maxRayBounces > static_cast<size_t>(std::numeric_limits<int>::max()))
    {
        throw std::invalid_argument("ray bounces cannot be higher than 2^31");
    }
    if (data.samplesPerPixel > static_cast<size_t>(std::numeric_limits<int>::max()))
    {
        throw std::invalid_argument("samples per pixel cannot be higher than 2^31");
    }
}

void RendererTraits<Production>::updateData(ospray::cpp::Renderer &handle, Production &data)
{
    handle.setParam(ProductionParameters::spp, static_cast<int>(data.samplesPerPixel));
    handle.setParam(ProductionParameters::pathLength, static_cast<int>(data.maxRayBounces));
    handle.setParam(ProductionParameters::background, data.backgroundColor);
}
}

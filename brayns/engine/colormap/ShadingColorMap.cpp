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

#include "ShadingColorMap.h"

#include <string>

#include <ospray/SDK/common/OSPCommon.h>
#include <ospray/ospray_cpp/Data.h>

namespace
{
struct ColorMapParameters
{
    inline static const std::string name = "piecewiseLinear";
    inline static const std::string colors = "color";
    inline static const std::string opacity = "opacity";
    inline static const std::string range = "value";
};
}

namespace brayns
{
ShadingColorMap::ShadingColorMap(const ColorMap &colorMap)
{
    set(colorMap);
}

void ShadingColorMap::set(const ColorMap &colorMap)
{
    constexpr auto stride = 4 * sizeof(float);
    auto &colors = colorMap.getColors();
    auto &color = colors.front();
    auto colorSize = colors.size();
    auto colorData = ospray::cpp::SharedData(&color.x, colorSize, stride);
    auto opacityData = ospray::cpp::SharedData(&color.w, colorSize, stride);
    _handle.setParam(ColorMapParameters::colors, colorData);
    _handle.setParam(ColorMapParameters::opacity, opacityData);

    auto &range = colorMap.getValuesRange();
    auto ospRange = rkcommon::math::range1f(range.x, range.y);
    _handle.setParam(ColorMapParameters::range, ospRange);

    _handle.commit();
}

const ospray::cpp::TransferFunction &ShadingColorMap::getHandle() const noexcept
{
    return _handle;
}
}

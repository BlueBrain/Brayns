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

#include "ShadingColorRamp.h"

#include <string>

#include <ospray/SDK/common/OSPCommon.h>
#include <ospray/ospray_cpp/Data.h>

namespace
{
struct ColorMapParameters
{
    static inline const std::string name = "piecewiseLinear";
    static inline const std::string colors = "color";
    static inline const std::string opacity = "opacity";
    static inline const std::string range = "value";
};
}

namespace brayns
{
ShadingColorRamp::ShadingColorRamp():
    _handle(ColorMapParameters::name)
{
}

void ShadingColorRamp::set(const ColorRamp &colorMap)
{
    constexpr auto stride = 4 * sizeof(float);
    auto &colors = colorMap.getColors();
    auto &color = colors.front();
    auto colorSize = colors.size();
    auto colorData = ospray::cpp::SharedData(&color.x, OSPDataType::OSP_VEC3F, colorSize, stride);
    auto opacityData = ospray::cpp::SharedData(&color.w, colorSize, stride);
    _handle.setParam(ColorMapParameters::colors, colorData);
    _handle.setParam(ColorMapParameters::opacity, opacityData);

    auto &range = colorMap.getValuesRange();
    auto ospRange = rkcommon::math::range1f(range.x, range.y);
    _handle.setParam(ColorMapParameters::range, ospRange);

    _handle.commit();
}

const ospray::cpp::TransferFunction &ShadingColorRamp::getHandle() const noexcept
{
    return _handle;
}
}

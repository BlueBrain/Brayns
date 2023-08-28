/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Authors: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                      Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "ColorRamp.h"

#include <algorithm>
#include <cassert>

namespace brayns
{
ColorRamp::ColorRamp():
    _colors({{0, 0, 0, 0}, {1, 1, 1, 1}}),
    _valuesRange({0, 255})
{
}

const std::vector<brayns::Vector4f> &ColorRamp::getColors() const
{
    return _colors;
}

void ColorRamp::setColors(std::vector<Vector4f> colors)
{
    auto size = colors.size();
    if (size == 0)
    {
        throw std::invalid_argument("A color ramp must have at least one color");
    }
    if (size > 256)
    {
        throw std::invalid_argument("A color ramp cannot have more than 256 colors");
    }
    _flag.update(_colors, std::move(colors));
}

const Vector2f &ColorRamp::getValuesRange() const
{
    return _valuesRange;
}

void ColorRamp::setValuesRange(const Vector2f &valuesRange)
{
    if (valuesRange.x >= valuesRange.y)
    {
        throw std::invalid_argument("Color ramp range must define a non zero positive interval");
    }

    _flag.update(_valuesRange, valuesRange);
}

Vector4f ColorRamp::getColorForValue(const float value) const
{
    if (value <= _valuesRange.x)
    {
        return _colors.front();
    }

    if (value >= _valuesRange.y)
    {
        return _colors.back();
    }

    const auto normalizedValue = (value - _valuesRange.x) / (_valuesRange.y - _valuesRange.x);

    const auto colorCount = _colors.size();
    assert(colorCount <= 256);

    const auto position = normalizedValue * static_cast<float>(colorCount);
    const auto colorIndex = static_cast<size_t>(position);

    const auto nextColorIndex = std::min(colorIndex + 1, colorCount - 1);

    const auto remainder = normalizedValue - std::floor(normalizedValue);

    const auto &color1 = _colors[colorIndex];
    const auto &color2 = _colors[nextColorIndex];

    return math::lerp(remainder, color1, color2);
}

bool ColorRamp::isModified() const noexcept
{
    return static_cast<bool>(_flag);
}

void ColorRamp::resetModified() noexcept
{
    _flag = false;
}
} // namespace brayns

/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include "TransferFunction.h"

#include <brayns/common/Log.h>

#include <algorithm>

namespace brayns
{
TransferFunction::TransferFunction()
    : _colors({{0, 0, 0, 0}, {1, 1, 1, 1}})
    , _valuesRange({0, 255})
{
}

const std::vector<brayns::Vector4f> &TransferFunction::getColors() const
{
    return _colors;
}

void TransferFunction::setColors(const std::vector<Vector4f> &colors)
{
    _updateValue(_colors, colors);
}

const Vector2f &TransferFunction::getValuesRange() const
{
    return _valuesRange;
}

void TransferFunction::setValuesRange(const Vector2f &valuesRange)
{
    if (valuesRange.x >= valuesRange.y)
    {
        throw std::invalid_argument("Transfer function range must define a non zero positive interval");
    }

    _updateValue(_valuesRange, valuesRange);
}

Vector4f TransferFunction::getColorForValue(const float v) const
{
    if (v <= _valuesRange.x)
    {
        return _colors.front();
    }

    if (v >= _valuesRange.y)
    {
        return _colors.back();
    }

    const auto normValue = (v - _valuesRange.x) / (_valuesRange.y - _valuesRange.x);

    const auto numColors = _colors.size();
    const size_t colorIndex = static_cast<size_t>(floor(normValue * numColors));
    const size_t nextColorIndex = std::min(colorIndex + 1, numColors - 1);

    const float remainder = normValue - floor(normValue);

    const auto &color1 = _colors[colorIndex];
    const auto &color2 = _colors[nextColorIndex];

    return glm::mix(color1, color2, remainder);
}
} // namespace brayns

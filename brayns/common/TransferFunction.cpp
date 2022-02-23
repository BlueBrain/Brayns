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

namespace
{
double _interpolatedOpacity(const std::vector<brayns::Vector2f> &controlPointsSorted, const double x) noexcept
{
    const auto &firstPoint = controlPointsSorted.front();
    if (x <= firstPoint.x)
        return firstPoint.y;

    for (size_t i = 1; i < controlPointsSorted.size(); ++i)
    {
        const auto &current = controlPointsSorted[i];
        const auto &previous = controlPointsSorted[i - 1];
        if (x <= current.x)
        {
            const auto t = (x - previous.x) / (current.x - previous.x);
            return (1.0 - t) * previous.y + t * current.y;
        }
    }

    const auto &lastPoint = controlPointsSorted.back();
    return lastPoint.y;
}


std::vector<float> _calculateInterpolatedOpacities(std::vector<brayns::Vector2f> controlPoints) noexcept
{
    constexpr size_t numSamples = 256;
    constexpr double dx = 1. / (numSamples - 1);

    std::sort(controlPoints.begin(), controlPoints.end(), [](auto a, auto b) { return a.x < b.x; });

    std::vector<float> opacities;
    opacities.reserve(numSamples);
    for (size_t i = 0; i < numSamples; ++i)
        opacities.push_back(_interpolatedOpacity(controlPoints, i * dx));
    return opacities;
}
} // namespace

namespace brayns
{
TransferFunction::TransferFunction()
{
    // Default trasnfer function status:
    // - Range 0 - 1
    // - Colors: Grayscale from black to white
    _colors = {{0, 0, 0}, {1, 1, 1}};
    _valuesRange = {0, 255};
    setControlPoints({{0, 0}, {1, 1}});
}

const std::vector<Vector2f> &TransferFunction::getControlPoints() const
{
    return _controlPoints;
}

void TransferFunction::setControlPoints(const std::vector<Vector2f> &controlPoints)
{
    _updateValue(_controlPoints, controlPoints);
    if(isModified())
        _opacities = _calculateInterpolatedOpacities(_controlPoints);
}

const std::vector<brayns::Vector3f> &TransferFunction::getColors() const
{
    return _colors;
}

void TransferFunction::setColors(const std::vector<Vector3f> &colors)
{
    _updateValue(_colors, colors);
}

const Vector2f &TransferFunction::getValuesRange() const
{
    return _valuesRange;
}

void TransferFunction::setValuesRange(const Vector2f &valuesRange)
{
    _updateValue(_valuesRange, valuesRange);
}

Vector4f TransferFunction::getColorForValue(const float v) const
{
    if (v <= _valuesRange.x)
    {
        const auto& color = *_colors.begin();
        const auto opacity = *_opacities.begin();
        return Vector4f(color, opacity);
    }

    if (v >= _valuesRange.y)
    {
        const auto& color = _colors.back();
        const auto opacity = _opacities.back();
        return Vector4f(color, opacity);
    }

    const auto normValue = (v - _valuesRange.x) / (_valuesRange.y - _valuesRange.x);

    const auto numColors = _colors.size();
    const size_t colorIndex = static_cast<size_t>(floor(normValue * numColors));
    const size_t nextColorIndex = std::min(colorIndex + 1, numColors - 1);

    const auto numOpacities = _opacities.size();
    const size_t opacityIndex = static_cast<size_t>(floor(normValue * numOpacities));
    const size_t nextOpacityIndex = std::min(opacityIndex + 1, numOpacities - 1);

    const float remainder = normValue - floor(normValue);

    const auto &color1 = _colors[colorIndex];
    const auto &color2 = _colors[nextColorIndex];

    const auto opacity1 = _opacities[opacityIndex];
    const auto opacity2 = _opacities[nextOpacityIndex];

    const Vector4f beginResult (color1, opacity1);
    const Vector4f endResult(color2, opacity2);

    return glm::mix(beginResult, endResult, remainder);
}
} // namespace brayns

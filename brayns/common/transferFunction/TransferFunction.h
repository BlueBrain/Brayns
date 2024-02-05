/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#pragma once

#include <brayns/common/BaseObject.h>
#include <brayns/common/MathTypes.h>

#include <vector>

namespace brayns
{
struct ColorMap
{
    std::string name;
    std::vector<Vector3f> colors;

    bool operator==(const ColorMap& rhs) const;

    void clear();
};

class TransferFunction : public BaseObject
{
public:
    TransferFunction();

    /** Reset to gray-scale with opacity [0..1] and value range [0,255]. */
    void clear();

    const std::vector<Vector2d>& getControlPoints() const
    {
        return _controlPoints;
    }
    void setControlPoints(const std::vector<Vector2d>& controlPoints)
    {
        _updateValue(_controlPoints, controlPoints);
    }

    const ColorMap& getColorMap() const { return _colorMap; }
    void setColorMap(const ColorMap& colorMap)
    {
        _updateValue(_colorMap, colorMap);
    }

    const auto& getColors() const { return _colorMap.colors; }
    const Vector2d& getValuesRange() const { return _valuesRange; }
    void setValuesRange(const Vector2d& valuesRange)
    {
        _updateValue(_valuesRange, valuesRange);
    }

    std::vector<float> calculateInterpolatedOpacities() const;

    Vector3f getColorForValue(const double v) const;

private:
    ColorMap _colorMap;
    std::vector<Vector2d> _controlPoints;
    Vector2d _valuesRange;
};
} // namespace brayns

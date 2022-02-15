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

    bool operator==(const ColorMap &rhs) const;

    void clear();
};

class TransferFunction : public BaseObject
{
public:
    TransferFunction();

    /** Reset to gray-scale with opacity [0..1] and value range [0,255]. */
    void clear();

    const std::vector<Vector2f> &getControlPoints() const;

    void setControlPoints(const std::vector<Vector2f> &controlPoints);

    const ColorMap &getColorMap() const;

    void setColorMap(const ColorMap &colorMap);

    const std::vector<brayns::Vector3f> &getColors() const;

    const Vector2f &getValuesRange() const;

    void setValuesRange(const Vector2f &valuesRange);

    std::vector<float> calculateInterpolatedOpacities() const;

    Vector3f getColorForValue(const double v) const;

private:
    ColorMap _colorMap;
    std::vector<Vector2f> _controlPoints;
    Vector2f _valuesRange;
};
} // namespace brayns

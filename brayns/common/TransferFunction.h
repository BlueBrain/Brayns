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
/**
 * @brief The TransferFunction class is an utility object which transform maps a value within a range, and from that
 * range, maps it into a color map scale.
 */
class TransferFunction : public BaseObject
{
public:
    /**
     * @brief Initializes the tranfer function by default to a grayscale from 0 to 1
     */
    TransferFunction();

    /**
     * @brief Return the control points. Control points are used to control and compute the opacity
     * of the color map. Each Control point consist on 2 numbers:
     *  - The first one indicates in which normalized value (0 - 1) of the color map list this control point starts
     *  - The second one specifies the opacity at that point.
     */
    const std::vector<Vector2f> &getControlPoints() const;

    /**
     * @brief Sets the control points. Control points are used to control and compute the opacity
     * of the color map. Each Control point consist on 2 numbers:
     *  - The first one indicates in which normalized value (0 - 1) of the color map list this control point starts
     *  - The second one specifies the opacity at that point.
     */
    void setControlPoints(const std::vector<Vector2f> &controlPoints);

    /**
     * @brief Return the list of colors that make up this transfer function color map
     */
    const std::vector<Vector3f> &getColors() const;

    /**
     * @brief Sets the color that make up the color map of this transfer function. Colors must be sorted.
     */
    void setColors(const std::vector<Vector3f> &colorMap);

    /**
     * @brief Return the range of values in which this transfer function works. Any value outside the range
     * is clamped in order to compute its color/opacity.
     */
    const Vector2f &getValuesRange() const;

    /**
     * @brief Sets the range of values in which this transfer function works. Any value outside the range
     * is clamped in order to compute its color/opacity.
     */
    void setValuesRange(const Vector2f &valuesRange);

    /**
     * @brief Return the interpolated opacities computed based on the control points
     */
    const std::vector<float>& getOpacities() const noexcept;

    /**
     * @brief Computes the color + opacity that corresponds to a given value
     */
    Vector4f getColorForValue(const float v) const;

private:
    std::vector<Vector3f> _colors;
    std::vector<float> _opacities;
    std::vector<Vector2f> _controlPoints;
    Vector2f _valuesRange;
};
} // namespace brayns

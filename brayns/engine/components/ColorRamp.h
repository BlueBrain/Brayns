/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <brayns/utils/MathTypes.h>
#include <brayns/utils/ModifiedFlag.h>

#include <vector>

namespace brayns
{
/**
 * @brief The ColorRamp class is an utility object which transform maps a value within a range, and from that
 * range, maps it into a color map scale.
 */
class ColorRamp
{
public:
    /**
     * @brief Initializes the tranfer function by default to a grayscale from 0 to 1
     */
    ColorRamp();

    /**
     * @brief Return the list of colors that make up this color ramp color map
     */
    const std::vector<Vector4f> &getColors() const;

    /**
     * @brief Sets the color that make up the color map of this color ramp. Colors must be sorted.
     */
    void setColors(std::vector<Vector4f> colorMap);

    /**
     * @brief Return the range of values in which this color ramp works. Any value outside the range
     * is clamped in order to compute its color/opacity.
     */
    const Vector2f &getValuesRange() const;

    /**
     * @brief Sets the range of values in which this color ramp works. Any value outside the range
     * is clamped in order to compute its color/opacity.
     */
    void setValuesRange(const Vector2f &valuesRange);

    /**
     * @brief Computes the color + opacity that corresponds to a given value
     */
    Vector4f getColorForValue(const float v) const;

    /**
     * @brief Returns the modified flag status object
     */
    bool isModified() const noexcept;

    /**
     * @brief Sets the modified flag to false
     */
    void resetModified() noexcept;

private:
    std::vector<Vector4f> _colors;
    Vector2f _valuesRange;
    ModifiedFlag _flag;
};
} // namespace brayns

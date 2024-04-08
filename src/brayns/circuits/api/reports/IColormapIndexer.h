/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman <nadir.romanguerrero@epfl.ch>
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

#include <brayns/core/utils/MathTypes.h>

#include <vector>

/**
 * @brief Interface to implement the compuation of indices into the colormap given the color ramp range
 * and the report frame data
 */
class IColormapIndexer
{
public:
    virtual ~IColormapIndexer() = default;

    /**
     * @brief Updates the indices in the indices buffer
     *
     * @param data
     * @param range
     * @param indices
     */
    virtual std::vector<uint8_t> generate(const std::vector<float> &data, const brayns::Vector2f &range) noexcept = 0;
};

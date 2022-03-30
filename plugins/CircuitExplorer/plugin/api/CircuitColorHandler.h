/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <brayns/common/MathTypes.h>
#include <brayns/engine/common/DataHandler.h>

#include <map>
#include <vector>

/**
 * @brief The ColoringInformation matches a method variable with a color
 */
struct ColoringInformation
{
    std::string variable;
    brayns::Vector4f color;
};

/**
 * @brief The CircuitColorHandler class is the interface to implement coloring methods on different
 * types of geometry
 */
class CircuitColorHandler
{
public:
    virtual ~CircuitColorHandler() = default;

    /**
     * @brief updateColorById Updates color of the elements by the ID they are
     * identified by. Specific ids might be used to isolate the update
     */
    virtual void updateColorById(const std::map<uint64_t, brayns::Vector4f> &colorMap) = 0;

    /**
     * @brief updateColorById Updates colors of the elements by assigning one to each element
     * @param colors
     */
    virtual void updateColorById(const std::vector<brayns::Vector4f> &colors) = 0;

    /**
     * @brief updateSingleColor Updates the color of all the elements to the
     * given color
     */
    virtual void updateSingleColor(const brayns::Vector4f &color) = 0;

    /**
     * @brief updateColor Updates the circuit color according to the given
     * method. If one or more variables are specified, only these will be
     * updated. Otherwise, updates the whole circuit.
     */
    virtual void updateColor(const std::string &method, const std::vector<ColoringInformation> &vars) = 0;

    /**
     * @brief updateIndexedColor udpates the circuit color using an indexed colormap.
     * @param color the color map
     * @param indices the indices into the color map
     */
    virtual void updateIndexedColor(brayns::OSPBuffer &color,
                                    const std::vector<uint8_t> &indices) = 0;
};

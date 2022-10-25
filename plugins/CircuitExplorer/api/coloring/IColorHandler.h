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

#include <brayns/utils/MathTypes.h>

#include <api/coloring/IColorData.h>

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
 * @brief The IColorHandler class is the interface to implement coloring methods on different
 * types of geometry
 */
class IColorHandler
{
public:
    virtual ~IColorHandler() = default;

    /**
     * @brief Updates the color of all the elements to the given color
     * @param color the color to use for all elements
     */
    virtual void updateColor(const brayns::Vector4f &color) = 0;

    /**
     * @brief Updates color of the elements which ID is present in the given color map
     * @param colorMap a map with the ids to update, and the color to use for each one of them
     * @return std::vector<uint64_t> list of ids that were not affected
     */
    virtual std::vector<uint64_t> updateColorById(const std::map<uint64_t, brayns::Vector4f> &colorMap) = 0;

    /**
     * @brief Updates colors of the elements by assigning one to each element
     * @param colors A list of colors, on for each element. Must be sorted according to the same order as the
     * IDs returned by getHandledIDs()
     */
    virtual void updateColorById(std::vector<brayns::Vector4f> colors) = 0;

    /**
     * @brief Updates element colors according to the given method. If one or more variables are
     * specified, only these will be updated. Otherwise, updates all elements.
     * @param colorData elements color information implementation
     * @param method The method to update the element colors by
     * @param vars optional list of variables to update, and the color to use for each one of them
     */
    virtual void updateColorByMethod(
        const IColorData &colorData,
        const std::string &method,
        const std::vector<ColoringInformation> &vars) = 0;

    /**
     * @brief Udpates the element colors using an indexed colormap.
     * @param color the color map
     * @param indices the indices into the color map
     */
    virtual void updateIndexedColor(std::vector<brayns::Vector4f> color, std::vector<uint8_t> indices) = 0;
};

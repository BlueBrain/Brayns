/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <unordered_map>
#include <vector>

/**
 * @brief The ColorTable struct holds static read-only list of colors to
 *        be used by the ColorDeck and ColorRoulette classes
 */
struct ColorTable
{
    std::vector<brayns::Vector4f> VALUES;

    ColorTable();
};

/**
 * @brief The ColorDeck class its an utility that allows to associate
 *        std::string keys with colors, allowing the access to the laters
 *        in an optimized way
 */
class ColorDeck
{
public:
    /**
     * @brief getColorForKey returns a color (brayns::Vector4f) for the given
     *        key. If no color is associated with the key, a new one is cached
     *        and returned
     */
    const brayns::Vector4f& getColorForKey(const std::string& k) noexcept;

private:
    const brayns::Vector4f& _emplaceColor(const std::string& k) noexcept;

    static ColorTable _TABLE;

    std::unordered_map<std::string, size_t> _colorMap;
    // Holds the last index used to access the ColorTable::VALUES color list
    size_t _lastIndex{0};
};

/**
 * @brief The ColorRoulette its an utility that returns a different color from
 *        the ColorTable color list on each call. Allows access to the colors in
 *        an optimized way
 */
class ColorRoulette
{
public:
    /**
     * @brief getNextColor returns the next color in the list. When the list
     *        has been exhausted, it starts from the beginning.
     */
    const brayns::Vector4f& getNextColor() noexcept;

private:
    static ColorTable _TABLE;
    // Holds the last index used to access the ColorTable::VALUES color list
    size_t _lastIndex{0};
};

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

#include "MathTypes.h"

#include <unordered_map>

namespace brayns
{
/**
 * @brief The ColorDeck allows to associate std::string keys with colors.
 */
class ColorDeck
{
public:
    /**
     * @brief Returns the color associated with the given key. If the key does not exists, a new color is associated
     * with it.
     * @param key Color key
     * @return Vector4f The color associated with the given key
     */
    Vector4f getColorForKey(const std::string &key) noexcept;

private:
    std::unordered_map<std::string, size_t> _colorMap;
    size_t _lastColortListIndex = 0;
};

/**
 * @brief The ColorRoulette returns a different color on each call.
 */
class ColorRoulette
{
public:
    /**
     * @brief Returns a random color.
     * @return Vector4f The color.
     */
    Vector4f getNextColor() noexcept;

private:
    size_t _lastColortListIndex = 0u;
};
}

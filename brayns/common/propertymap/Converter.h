/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include <vector>

#include <brayns/common/MathTypes.h>

namespace brayns
{
/**
 * @brief The struct used to abstract the static conversion from a type to
 * another. The default implementation uses explicit constructor of To from
 * From. Use output parameter to preserve a default value.
 *
 * @tparam From The type of the source object to convert from.
 * @tparam To The type of the destination object to be converted.
 */
template <typename From, typename To>
struct Converter
{
    /**
     * @brief The static method to perform conversion.
     *
     * @param from The value to convert from.
     * @param to The value to convert to.
     */
    static void convert(const From& from, To& to) { to = To(from); }
};

/**
 * @brief Converter specialization for std::vector to convert a vector of
 * convertible types (ie a Converter<From, To> exists).
 *
 * @tparam From The type of the objects contained in the source vector.
 * @tparam To The type of the objects contained in the destination vector.
 */
template <typename From, typename To>
struct Converter<std::vector<From>, std::vector<To>>
{
    /**
     * @brief Clear destination, take all objects from source, convert them in
     * destination type and put them in destination vector.
     *
     * @param from Source vector.
     * @param to Destination vector.
     */
    static void convert(const std::vector<From>& from, std::vector<To>& to)
    {
        to.clear();
        to.reserve(from.size());
        for (const auto& item : from)
        {
            to.emplace_back();
            Converter<From, To>::convert(item, to.back());
        }
    }
};

/**
 * @brief Converter specialization for glm::vecX to convert a vec of
 * convertible types (ie a Converter<From, To> exists).
 *
 * @tparam From The type of the objects contained in the source vec.
 * @tparam SizeFrom The size of the source vec
 * @tparam To The type of the objects contained in the destination vec.
 * @tparam SizeTo The size of the destination vec.
 */
template <typename From, glm::length_t L1, typename To, glm::length_t L2>
struct Converter<glm::vec<L1, From>, glm::vec<L2, To>>
{
    /**
     * @brief Fill destination with converted objects from source. If SizeFrom
     * != SizeTo, the smallest size will be used, the excessive elements of the
     * biggest array will remain unchanged.
     *
     * @param from Source vec.
     * @param to Destination vec.
     */
    static void convert(const glm::vec<L1, From>& from, glm::vec<L2, To>& to)
    {
        auto itemCount = std::min(from.length(), to.length());
        for (glm::length_t i = 0; i < itemCount; ++i)
        {
            Converter<From, To>::convert(from[i], to[i]);
        }
    }
};
} // namespace brayns

/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include <sstream>

#include <brayns/common/MathTypes.h>

#include "ArgvProperty.h"

namespace brayns
{
template<typename T>
struct ArgvReflector
{
    static ArgvProperty reflect(T &value)
    {
        auto property = GetArgvProperty::of<T>();
        property.extract = [&](const auto &values) { std::istringstream(values[0]) >> value; };
        property.minItems = 1;
        property.maxItems = 1;
    }
};

template<typename T>
struct ArgvReflector<std::vector<T>>
{
    static ArgvProperty reflect(std::vector<T> &value)
    {
        auto property = GetArgvProperty::of<T>();
        property.extract = [&](const auto &values)
        {
            auto size = values.size();
            value.resize(size);
            for (size_t i = 0; i < size; ++i)
            {
                std::istringstream(values[i]) >> value[i];
            }
        };
    }
};

template<glm::length_t S, typename T>
struct ArgvReflector<glm::vec<S, T>>
{
    static ArgvProperty reflect(glm::vec<S, T> &value)
    {
        auto property = GetArgvProperty::of<T>();
        property.extract = [&](const auto &values)
        {
            for (glm::length_t i = 0; i < S; ++i)
            {
                std::istringstream(values[i]) >> value[i];
            }
        };
        property.minItems = S;
        property.maxItems = S;
    }
};
} // namespace brayns

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

#include "MathTypes.h"

#include <brayns/utils/parsing/ByteParser.h>
#include <brayns/utils/parsing/TokenParser.h>

namespace brayns
{
template<glm::length_t S, typename T>
struct ByteParser<glm::vec<S, T>>
{
    static void parse(StringStream &stream, Endian endian, glm::vec<S, T> &value)
    {
        for (glm::length_t i = 0; i < S; ++i)
        {
            ByteParser<T>::parse(stream, endian, value[i]);
        }
    }
};

template<glm::length_t S, typename T>
struct TokenParser<glm::vec<S, T>>
{
    static void parse(StringStream &stream, glm::vec<S, T> &value)
    {
        for (glm::length_t i = 0; i < S; ++i)
        {
            TokenParser<T>::parse(stream, value[i]);
        }
    }
};
} // namespace brayns

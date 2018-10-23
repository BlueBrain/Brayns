/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include <brayns/common/types.h>

#include <type_traits>

namespace brayns
{
strings parseFolder(const std::string& folder, const strings& filters);

std::string shortenString(const std::string& string,
                          const size_t maxLength = 32);

inline auto lowerCase(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

template <typename T>
inline T enumSet(const T eA, const T eB)
{
    using U = typename std::underlying_type<T>::type;
    return static_cast<T>(static_cast<U>(eA) | static_cast<U>(eB));
}

template <typename T>
inline bool enumHas(const T eA, const T eB)
{
    using U = typename std::underlying_type<T>::type;
    return static_cast<T>(static_cast<U>(eA) & static_cast<U>(eB)) == eB;
}
}

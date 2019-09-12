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

#include <brayns/common/mathTypes.h>
#include <brayns/common/types.h>

#include <algorithm>

namespace brayns
{
strings parseFolder(const std::string& folder, const strings& filters);

std::string extractExtension(const std::string& filename);

template <size_t M, typename T>
inline glm::vec<M, T> toGlmVec(const std::array<T, M>& input)
{
    glm::vec<M, T> vec;
    memcpy(glm::value_ptr(vec), input.data(), input.size() * sizeof(T));
    return vec;
}

template <size_t M, typename T>
inline std::array<T, M> toArray(const glm::vec<M, T>& input)
{
    std::array<T, M> output;
    memcpy(output.data(), glm::value_ptr(input), M * sizeof(T));
    return output;
}

} // namespace brayns

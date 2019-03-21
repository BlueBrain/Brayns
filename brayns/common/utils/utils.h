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

std::string shortenString(const std::string& string,
                          const size_t maxLength = 32);

std::string extractExtension(const std::string& filename);

std::string replaceFirstOccurrence(std::string input,
                                   const std::string& toReplace,
                                   const std::string& replaceWith);

bool containsString(const int length, const char** input, const char* toFind);

inline auto lowerCase(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

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

std::string camelCaseToSeparated(const std::string& camelCase,
                                 const char separator);
std::string separatedToCamelCase(const std::string& separated,
                                 const char separator);
std::string joinStrings(const std::vector<std::string>& strings,
                        const std::string& joinWith);
std::string toLowercase(const std::string input);

template <typename T>
inline bool erase_value(std::vector<T>& vec, const T& value)
{
    auto it = std::find(vec.begin(), vec.end(), value);
    if (it != vec.end())
    {
        vec.erase(it);
        return true;
    }
    return false;
}

template <typename T, typename PredicateT>
inline bool erase_if(std::vector<T>& vec, const PredicateT predicate)
{
    auto it = std::find_if(vec.begin(), vec.end(), predicate);
    if (it != vec.end())
    {
        vec.erase(it);
        return true;
    }
    return false;
}

} // namespace brayns

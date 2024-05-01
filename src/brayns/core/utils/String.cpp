/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include "String.h"

namespace brayns::experimental
{
std::string join(std::span<std::string> values, char separator)
{
    return join(values, {&separator, 1});
}

std::string join(std::span<std::string> values, std::string_view separator)
{
    auto count = values.size();

    if (count == 0)
    {
        return {};
    }

    auto step = separator.size();
    auto reserved = (count - 1) * step;

    for (const auto &value : values)
    {
        reserved += value.size();
    }

    auto result = std::string();
    result.reserve(reserved);

    result.append(values[0]);

    for (auto i = std::size_t(1); i < values.size(); ++i)
    {
        result.append(separator);
        result.append(values[i]);
    }

    return result;
}
}

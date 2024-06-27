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

#include "CommandLine.h"

namespace
{
using namespace brayns;

bool isKey(std::string_view arg)
{
    return arg.starts_with("--");
}

std::string getKey(std::string_view arg)
{
    return std::string(arg.substr(2));
}

void consumeValue(int &index)
{
    index += 1;
}

void addKeyValue(ArgvMap &result, std::string key, std::string value)
{
    if (result.contains(key))
    {
        throw std::invalid_argument(fmt::format("Duplicated key: '{}'", key));
    }

    result.emplace(std::move(key), std::move(value));
}
}

namespace brayns
{
ArgvMap parseArgv(int argc, const char **argv)
{
    auto result = ArgvMap();

    for (auto i = 1; i < argc; ++i)
    {
        auto arg = std::string_view(argv[i]);

        if (!isKey(arg))
        {
            throw std::invalid_argument(fmt::format("Value without key: '{}'", arg));
        }

        auto key = getKey(arg);

        auto nextIndex = i + 1;

        if (nextIndex == argc)
        {
            addKeyValue(result, key, {});
            continue;
        }

        auto nextArg = std::string_view(argv[nextIndex]);

        if (isKey(nextArg))
        {
            addKeyValue(result, key, {});
            continue;
        }

        addKeyValue(result, key, std::string(nextArg));
        consumeValue(i);
    }

    return result;
}
}

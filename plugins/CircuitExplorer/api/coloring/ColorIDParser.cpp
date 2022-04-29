/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "ColorIDParser.h"

#include <brayns/utils/StringUtils.h>

namespace
{
uint64_t parseID(const std::string &input)
{
    try
    {
        return std::stoul(input);
    }
    catch (...)
    {
        throw std::invalid_argument("Could not parse ID '" + input + "'");
    }
}

std::array<uint64_t, 2> parseIDRange(const std::string &input, const size_t dashPos)
{
    std::array<uint64_t, 2> result;
    try
    {
        const auto firstRaw = input.substr(0, dashPos);
        result[0] = std::stoul(firstRaw);
        const auto secondRaw = input.substr(dashPos + 1);
        result[1] = std::stoul(secondRaw);
    }
    catch (...)
    {
        throw std::invalid_argument("Could not parse ID range '" + input + "'");
    }

    return result;
}
}

std::map<uint64_t, brayns::Vector4f> ColorIDParser::parse(const std::vector<ColoringInformation> &colors)
{
    std::map<uint64_t, brayns::Vector4f> colorMap;

    for (const auto &entry : colors)
    {
        const auto &rawIds = entry.variable;
        const auto &color = entry.color;

        const auto tokens = brayns::string_utils::split(rawIds, ',');
        for (const auto &token : tokens)
        {
            auto dashPos = token.find("-");
            if (dashPos == std::string::npos)
            {
                const auto id = parseID(token);
                colorMap[id] = color;
            }
            else
            {
                const auto range = parseIDRange(token, dashPos);
                for (uint64_t i = range[0]; i <= range[1]; ++i)
                {
                    colorMap[i] = color;
                }
            }
        }
    }

    return colorMap;
}

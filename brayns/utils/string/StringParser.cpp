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

#include "StringParser.h"

#include <cmath>
#include <stdexcept>

namespace brayns
{
double StringParserHelper::parseNumber(std::string_view data)
{
    auto buffer = std::string(data);
    try
    {
        return std::stod(buffer);
    }
    catch (const std::exception &e)
    {
        throw std::runtime_error("Cannot parse number from '" + buffer + "'");
    }
}

void StringParserHelper::checkLimits(double value, double min, double max)
{
    if (value < min)
    {
        throw std::runtime_error("Range error: " + std::to_string(value) + " < " + std::to_string(min));
    }
    if (value > max)
    {
        throw std::runtime_error("Range error: " + std::to_string(value) + " > " + std::to_string(max));
    }
}

void StringParserHelper::checkIsInteger(double value)
{
    if (std::floor(value) != value)
    {
        throw std::runtime_error(std::to_string(value) + " is not an integer");
    }
}

void StringParser<bool>::parse(std::string_view data, bool &value)
{
    if (data == "1" || data == "true")
    {
        value = true;
        return;
    }
    if (data == "0" || data == "false")
    {
        value = false;
        return;
    }
    throw std::runtime_error("Cannot parse boolean from '" + std::string(data) + "'");
}
} // namespace brayns

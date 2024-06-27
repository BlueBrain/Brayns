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

#include "Logger.h"

#include <iostream>
#include <stdexcept>

namespace brayns::experimental
{
std::string toString(const LogRecord &record)
{
    auto level = getEnumName(record.level);
    return fmt::format("[{}][{}]: {}", record.name, level, record.message);
}

Logger::Logger(std::string name, LogLevel level, LogHandler handler):
    _name(std::move(name)),
    _level(level),
    _handler(std::move(handler))
{
}

const std::string &Logger::getName() const
{
    return _name;
}

LogLevel Logger::getLevel() const
{
    return _level;
}

void Logger::setLevel(LogLevel level)
{
    _level = level;
}

bool Logger::isEnabled(LogLevel level) const
{
    return level >= _level;
}

Logger createConsoleLogger(std::string name)
{
    auto handler = [](const auto &record) { std::cout << toString(record) << '\n'; };
    return Logger(std::move(name), LogLevel::Info, handler);
}
}

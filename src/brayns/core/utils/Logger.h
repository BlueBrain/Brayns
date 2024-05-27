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

#pragma once

#include <functional>
#include <string>
#include <string_view>

#include <fmt/format.h>

#include <brayns/core/utils/EnumInfo.h>

namespace brayns
{
enum class LogLevel
{
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Fatal,
    Off,
};

template<>
struct EnumReflector<LogLevel>
{
    static EnumMap<LogLevel> reflect();
};

struct LogRecord
{
    std::string_view name;
    LogLevel level = LogLevel::Info;
    std::string_view message;
};

std::string toString(const LogRecord &record);

using LogHandler = std::function<void(const LogRecord &)>;

class Logger
{
public:
    explicit Logger(std::string name, LogLevel level, LogHandler handler);

    const std::string &getName() const;
    LogLevel getLevel() const;
    void setLevel(LogLevel level);
    bool isEnabled(LogLevel level) const;

    template<typename... Args>
    void log(LogLevel level, fmt::format_string<Args...> format, Args &&...args)
    {
        if (!isEnabled(level))
        {
            return;
        }
        auto message = fmt::format(format, std::forward<Args>(args)...);
        auto record = LogRecord(_name, _level, message);
        _handler(record);
    }

    template<typename... Args>
    void trace(fmt::format_string<Args...> format, Args &&...args)
    {
        log(LogLevel::Trace, format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void debug(fmt::format_string<Args...> format, Args &&...args)
    {
        log(LogLevel::Debug, format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void info(fmt::format_string<Args...> format, Args &&...args)
    {
        log(LogLevel::Info, format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void warn(fmt::format_string<Args...> format, Args &&...args)
    {
        log(LogLevel::Warn, format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void error(fmt::format_string<Args...> format, Args &&...args)
    {
        log(LogLevel::Error, format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void fatal(fmt::format_string<Args...> format, Args &&...args)
    {
        log(LogLevel::Fatal, format, std::forward<Args>(args)...);
    }

private:
    std::string _name;
    LogLevel _level;
    LogHandler _handler;
};

Logger createConsoleLogger(std::string name);
}

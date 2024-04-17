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

#include <string>
#include <string_view>

#include <fmt/format.h>

#include <brayns/core/utils/EnumInfo.h>

namespace brayns
{
/**
 * @brief Available log levels.
 *
 */
enum class LogLevel
{
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Critical,
    Off,
    Count,
};

template<>
struct EnumReflector<LogLevel>
{
    static EnumMap<LogLevel> reflect();
};

class Log
{
public:
    /**
     * @brief Set the global log level.
     *
     * @param level Log level.
     */
    static void setLevel(LogLevel level);

    /**
     * @brief Disable logging.
     *
     */
    static void disable();

    /**
     * @brief Log the given object with optional arguments.
     *
     * @tparam Args Arguments types.
     * @param level Log level.
     * @param format Message format.
     * @param args Arguments to format.
     */
    template<typename... Args>
    static void log(LogLevel level, fmt::format_string<Args...> format, Args &&...args)
    {
        if (level < _level)
        {
            return;
        }
        auto message = fmt::format(format, std::forward<Args>(args)...);
        _handleMessage(level, message);
    }

    /**
     * @brief Log the given message with optional arguments and trace level.
     *
     * @tparam Args Arguments types.
     * @param format Message format.
     * @param args Arguments to format.
     */
    template<typename... Args>
    static void trace(fmt::format_string<Args...> format, Args &&...args)
    {
        log(LogLevel::Trace, format, std::forward<Args>(args)...);
    }

    /**
     * @brief Log the given message with optional arguments and debug level.
     *
     * @tparam Args Arguments types.
     * @param format Message format.
     * @param args Arguments to format.
     */
    template<typename... Args>
    static void debug(fmt::format_string<Args...> format, Args &&...args)
    {
        log(LogLevel::Debug, format, std::forward<Args>(args)...);
    }

    /**
     * @brief Log the given message with optional arguments and info level.
     *
     * @tparam Args Arguments types.
     * @param format Message format.
     * @param args Arguments to format.
     */
    template<typename... Args>
    static void info(fmt::format_string<Args...> format, Args &&...args)
    {
        log(LogLevel::Info, format, std::forward<Args>(args)...);
    }

    /**
     * @brief Log the given message with optional arguments and warn level.
     *
     * @tparam Args Arguments types.
     * @param format Message format.
     * @param args Arguments to format.
     */
    template<typename... Args>
    static void warn(fmt::format_string<Args...> format, Args &&...args)
    {
        log(LogLevel::Warn, format, std::forward<Args>(args)...);
    }

    /**
     * @brief Log the given message with optional arguments and error level.
     *
     * @tparam Args Arguments types.
     * @param format Message format.
     * @param args Arguments to format.
     */
    template<typename... Args>
    static void error(fmt::format_string<Args...> format, Args &&...args)
    {
        log(LogLevel::Error, format, std::forward<Args>(args)...);
    }

    /**
     * @brief Log the given message with optional arguments and critical level.
     *
     * @tparam Args Arguments types.
     * @param format Message format.
     * @param args Arguments to format.
     */
    template<typename... Args>
    static void critical(fmt::format_string<Args...> format, Args &&...args)
    {
        log(LogLevel::Critical, format, std::forward<Args>(args)...);
    }

private:
    static inline LogLevel _level = LogLevel::Info;

    static void _handleMessage(LogLevel level, std::string_view message);
};
} // namespace brayns

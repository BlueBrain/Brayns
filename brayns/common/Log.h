/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#define SPDLOG_HEADER_ONLY
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace brayns
{
enum class LogLevel
{
    Trace = spdlog::level::trace,
    Debug = spdlog::level::debug,
    Info = spdlog::level::info,
    Warn = spdlog::level::warn,
    Error = spdlog::level::err,
    Critical = spdlog::level::critical,
    Off = spdlog::level::off,
    Count = spdlog::level::n_levels
};

class Log
{
public:
    static void disable() { setLevel(LogLevel::Off); }

    static void setLevel(LogLevel level)
    {
        getLogger().set_level(spdlog::level::level_enum(level));
    }

    template <typename... Args>
    static void log(LogLevel level, Args &&... args)
    {
        getLogger().log(spdlog::level::level_enum(level),
                        std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void trace(Args &&... args)
    {
        log(LogLevel::Trace, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void debug(Args &&... args)
    {
        log(LogLevel::Debug, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void info(Args &&... args)
    {
        log(LogLevel::Info, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void warn(Args &&... args)
    {
        log(LogLevel::Warn, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void error(Args &&... args)
    {
        log(LogLevel::Error, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void critical(Args &&... args)
    {
        log(LogLevel::Critical, std::forward<Args>(args)...);
    }

private:
    static spdlog::logger &getLogger()
    {
        static const auto logger = _createLogger();
        return *logger;
    }

    static std::shared_ptr<spdlog::logger> _createLogger()
    {
        auto logger = spdlog::stdout_color_mt("Brayns");
        logger->set_pattern("%^[%l][%T] %v%$");
#ifdef NDEBUG
        logger->set_level(spdlog::level::info);
#else
        logger->set_level(spdlog::level::trace);
#endif
        return logger;
    }
};
} // namespace brayns
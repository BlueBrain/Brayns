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

#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace
{
class LoggerFactory
{
public:
    static std::shared_ptr<spdlog::logger> createLogger()
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
} // namespace

namespace brayns
{
void Log::setLevel(LogLevel level)
{
    _logger->set_level(spdlog::level::level_enum(level));
}

void Log::disable()
{
    setLevel(LogLevel::Off);
}

std::shared_ptr<spdlog::logger> Log::_logger = LoggerFactory::createLogger();
} // namespace brayns
/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include "ApplicationParameters.h"

#include <stdexcept>
#include <string_view>

#include <brayns/common/Log.h>

#include <brayns/common/parameters/ParametersManager.h>

namespace
{
constexpr auto PARAM_PLUGIN = "plugin";
constexpr auto PARAM_LOG_LEVEL = "log-level";
constexpr auto PARAM_WINDOW_SIZE = "window-size";
constexpr auto PARAM_JPEG_QUALITY = "jpeg-quality";

constexpr size_t DEFAULT_WINDOW_WIDTH = 800;
constexpr size_t DEFAULT_WINDOW_HEIGHT = 600;
constexpr size_t DEFAULT_JPEG_QUALITY = 90;

class GetLogLevel
{
public:
    static brayns::LogLevel fromName(std::string_view name)
    {
        if (name == "trace")
        {
            return brayns::LogLevel::Trace;
        }
        if (name == "debug")
        {
            return brayns::LogLevel::Debug;
        }
        if (name == "info")
        {
            return brayns::LogLevel::Info;
        }
        if (name == "warn" || name == "warning")
        {
            return brayns::LogLevel::Warn;
        }
        if (name == "error")
        {
            return brayns::LogLevel::Error;
        }
        if (name == "critical")
        {
            return brayns::LogLevel::Critical;
        }
        throw std::runtime_error("Invalid log level '" + std::string(name) + "'");
    }
};
} // namespace

namespace brayns
{
ApplicationParameters::ApplicationParameters()
    : AbstractParameters("Application")
    , _windowSize(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT)
    , _jpegQuality(DEFAULT_JPEG_QUALITY)
{
    _parameters.add_options() //
        (PARAM_PLUGIN,
         po::value<std::vector<std::string>>(&_plugins)->composing(),
         "Dynamic plugin to load from LD_LIBRARY_PATH; "
         "can be repeated to load multiple plugins. "
         "Arguments to plugins can be added by inserting a space followed by "
         "the arguments like: --plugin 'myPluginName arg0 arg1'") //
        (PARAM_LOG_LEVEL,
         po::value<std::string>(),
         "Log level among [trace, debug, info, warn, error, critical] (default info).") //
        (PARAM_WINDOW_SIZE,
         po::fixed_tokens_value<std::vector<uint32_t>>(2, 2),
         "Window size [uint uint]") //
        (PARAM_JPEG_QUALITY, po::value<int>(&_jpegQuality), "JPEG quality [0-100] (100 is full quality)");
}

void ApplicationParameters::print()
{
    AbstractParameters::print();
    Log::info("Plugins                     :");
    for (const auto &plugin : _plugins)
    {
        Log::info("- {}", plugin);
    }
    Log::info("Window size                 : {}", _windowSize);
    Log::info("JPEG quality                : {}", _jpegQuality);
}

const std::vector<std::string> &ApplicationParameters::getPlugins() const noexcept
{
    return _plugins;
}

LogLevel ApplicationParameters::getLogLevel() const noexcept
{
    return _logLevel;
}

const Vector2ui &ApplicationParameters::getWindowSize() const noexcept
{
    return _windowSize;
}

void ApplicationParameters::setWindowSize(const Vector2ui &size) noexcept
{
    _updateValue(_windowSize, size);
}

int ApplicationParameters::getJpegQuality() const noexcept
{
    return _jpegQuality;
}

void ApplicationParameters::setJpegQuality(int quality) noexcept
{
    _updateValue(_jpegQuality, quality);
}

po::positional_options_description &ApplicationParameters::posArgs() noexcept
{
    return _positionalArgs;
}

void ApplicationParameters::parse(const po::variables_map &vm)
{
    auto i = vm.find(PARAM_LOG_LEVEL);
    if (i != vm.end())
    {
        auto &value = i->second.as<std::string>();
        _logLevel = GetLogLevel::fromName(value);
        Log::setLevel(_logLevel);
    }
    if (vm.count(PARAM_WINDOW_SIZE))
    {
        auto values = vm[PARAM_WINDOW_SIZE].as<std::vector<uint32_t>>();
        _windowSize.x = values[0];
        _windowSize.y = values[1];
    }
    if (_jpegQuality < 0 || _jpegQuality > 100)
    {
        throw std::runtime_error("Invalid JPEG quality: " + std::to_string(_jpegQuality));
    }
}

} // namespace brayns

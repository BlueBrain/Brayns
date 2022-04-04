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

#include <brayns/parameters/ParametersManager.h>

namespace
{
constexpr auto PARAM_LOG_LEVEL = "log-level";
constexpr auto PARAM_IMAGE_STREAM_FPS = "image-stream-fps";
constexpr auto PARAM_JPEG_COMPRESSION = "jpeg-compression";
constexpr auto PARAM_PLUGIN = "plugin";
constexpr auto PARAM_WINDOW_SIZE = "window-size";

constexpr size_t DEFAULT_WINDOW_WIDTH = 800;
constexpr size_t DEFAULT_WINDOW_HEIGHT = 600;
constexpr size_t DEFAULT_JPEG_COMPRESSION = 90;

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
    , _jpegCompression(DEFAULT_JPEG_COMPRESSION)
{
    _parameters.add_options() //
        (PARAM_LOG_LEVEL,
         po::value<std::string>(),
         "Log level among [trace, debug, info, warn, error, critical] (default info).") //
        (PARAM_PLUGIN,
         po::value<std::vector<std::string>>(&_plugins)->composing(),
         "Dynamic plugin to load from LD_LIBRARY_PATH; "
         "can be repeated to load multiple plugins. "
         "Arguments to plugins can be added by inserting a space followed by "
         "the arguments like: --plugin 'myPluginName arg0 arg1'") //
        (PARAM_WINDOW_SIZE,
         po::fixed_tokens_value<std::vector<uint32_t>>(2, 2),
         "Window size [uint uint]") //
        (PARAM_JPEG_COMPRESSION,
         po::value<size_t>(&_jpegCompression),
         "JPEG compression rate (100 is full quality) [int]") //
        (PARAM_IMAGE_STREAM_FPS, po::value<size_t>(&_imageStreamFPS), "Image stream FPS (60 default), [int]");
}

void ApplicationParameters::print()
{
    AbstractParameters::print();
    Log::info("Plugins                     :");
    for (const auto &plugin : _plugins)
        Log::info("- {}", plugin);
    Log::info("Window size                 : {}", _windowSize);
    Log::info("JPEG Compression            : {}", _jpegCompression);
    Log::info("Image stream FPS            : {}", _imageStreamFPS);
}

const Vector2ui &ApplicationParameters::getWindowSize() const noexcept
{
    return _windowSize;
}

void ApplicationParameters::setWindowSize(const Vector2ui &size) noexcept
{
    _updateValue(_windowSize, size);
}

void ApplicationParameters::setJpegCompression(const size_t cmpr) noexcept
{
    _updateValue(_jpegCompression, cmpr);
}

size_t ApplicationParameters::getJpegCompression() const noexcept
{
    return _jpegCompression;
}

size_t ApplicationParameters::getImageStreamFPS() const noexcept
{
    return _imageStreamFPS;
}

void ApplicationParameters::setImageStreamFPS(const size_t fps) noexcept
{
    _updateValue(_imageStreamFPS, fps);
}

bool ApplicationParameters::getUseQuantaRenderControl() const noexcept
{
    return _useQuantaRenderControl;
}

void ApplicationParameters::setUseQuantaRenderControl(const bool val) noexcept
{
    _updateValue(_useQuantaRenderControl, val);
}

LogLevel ApplicationParameters::getLogLevel() const noexcept
{
    return _logLevel;
}

const std::vector<std::string> &ApplicationParameters::getPlugins() const noexcept
{
    return _plugins;
}

po::positional_options_description &ApplicationParameters::posArgs() noexcept
{
    return _positionalArgs;
}

void ApplicationParameters::parse(const po::variables_map &vm)
{
    if (vm.count(PARAM_WINDOW_SIZE))
    {
        auto values = vm[PARAM_WINDOW_SIZE].as<std::vector<uint32_t>>();
        _windowSize.x = values[0];
        _windowSize.y = values[1];
    }
    auto i = vm.find(PARAM_LOG_LEVEL);
    if (i != vm.end())
    {
        auto &value = i->second.as<std::string>();
        _logLevel = GetLogLevel::fromName(value);
        Log::setLevel(_logLevel);
    }
}

} // namespace brayns

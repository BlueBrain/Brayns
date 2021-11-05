/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
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
#include <brayns/common/Log.h>
#include <brayns/parameters/ParametersManager.h>

namespace
{
const std::string PARAM_BENCHMARKING = "enable-benchmark";
const std::string PARAM_ENGINE = "engine";
const std::string PARAM_HTTP_SERVER = "http-server";
const std::string PARAM_IMAGE_STREAM_FPS = "image-stream-fps";
const std::string PARAM_INPUT_PATHS = "input-paths";
const std::string PARAM_JPEG_COMPRESSION = "jpeg-compression";
const std::string PARAM_MAX_RENDER_FPS = "max-render-fps";
const std::string PARAM_MODULE = "module";
const std::string PARAM_PARALLEL_RENDERING = "parallel-rendering";
const std::string PARAM_PLUGIN = "plugin";
const std::string PARAM_STEREO = "stereo";
const std::string PARAM_WINDOW_SIZE = "window-size";
const std::string PARAM_ENV_MAP = "env-map";
const std::string PARAM_SANDBOX_PATH = "sandbox-path";

const size_t DEFAULT_WINDOW_WIDTH = 800;
const size_t DEFAULT_WINDOW_HEIGHT = 600;
const size_t DEFAULT_JPEG_COMPRESSION = 90;
const std::string DEFAULT_SANDBOX_PATH = "/gpfs/bbp.cscs.ch/project";
} // namespace

namespace brayns
{
ApplicationParameters::ApplicationParameters()
    : AbstractParameters("Application")
    , _windowSize(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT)
    , _jpegCompression(DEFAULT_JPEG_COMPRESSION)
    , _sandBoxPath(DEFAULT_SANDBOX_PATH)
{
    _parameters.add_options() //
        (PARAM_ENGINE.c_str(), po::value<std::string>(&_engine),
         "Engine name [ospray]") //
        (PARAM_MODULE.c_str(), po::value<strings>(&_modules)->composing(),
         "OSPRay module name [string]") //
        (PARAM_INPUT_PATHS.c_str(), po::value<strings>(&_inputPaths),
         "List of files/folders to load data from") //
        (PARAM_PLUGIN.c_str(), po::value<strings>(&_plugins)->composing(),
         "Dynamic plugin to load from LD_LIBRARY_PATH; "
         "can be repeated to load multiple plugins. "
         "Arguments to plugins can be added by inserting a space followed by "
         "the arguments like: --plugin 'myPluginName arg0 arg1'") //
        (PARAM_WINDOW_SIZE.c_str(), po::fixed_tokens_value<uints>(2, 2),
         "Window size [uint uint]") //
        (PARAM_BENCHMARKING.c_str(),
         po::bool_switch(&_benchmarking)->default_value(false),
         "Enable benchmarking") //
        (PARAM_JPEG_COMPRESSION.c_str(), po::value<size_t>(&_jpegCompression),
         "JPEG compression rate (100 is full quality) [int]") //
        (PARAM_PARALLEL_RENDERING.c_str(),
         po::bool_switch(&_parallelRendering)->default_value(false),
         "Enable parallel rendering, equivalent to --osp:mpi") //
        (PARAM_STEREO.c_str(), po::bool_switch(&_stereo)->default_value(false),
         "Enable stereo rendering") //
        (PARAM_IMAGE_STREAM_FPS.c_str(), po::value<size_t>(&_imageStreamFPS),
         "Image stream FPS (60 default), [int]") //
        (PARAM_MAX_RENDER_FPS.c_str(), po::value<size_t>(&_maxRenderFPS),
         "Max. render FPS") //
        (PARAM_ENV_MAP.c_str(), po::value<std::string>(&_envMap),
         "Path to environment map") //
        (PARAM_SANDBOX_PATH.c_str(), po::value<std::string>(&_sandBoxPath),
         "Path to sandbox directory");

    _positionalArgs.add(PARAM_INPUT_PATHS.c_str(), -1);
}

void ApplicationParameters::parse(const po::variables_map& vm)
{
    if (vm.count(PARAM_WINDOW_SIZE))
    {
        uints values = vm[PARAM_WINDOW_SIZE].as<uints>();
        _windowSize.x = values[0];
        _windowSize.y = values[1];
    }
    markModified();
}

void ApplicationParameters::print()
{
    AbstractParameters::print();
    Log::info("Engine                      : {}", _engine);
    Log::info("Ospray modules              :");
    for (const auto& module : _modules)
        Log::info("- {}", module);
    Log::info("Plugins                     :");
    for (const auto& plugin : _plugins)
        Log::info("- {}", plugin);
    Log::info("Window size                 : {}", _windowSize);
    Log::info("Benchmarking                : {}", asString(_benchmarking));
    Log::info("JPEG Compression            : {}", _jpegCompression);
    Log::info("Image stream FPS            : {}", _imageStreamFPS);
    Log::info("Max. render  FPS            : {}", _maxRenderFPS);
    Log::info("Sandbox directory           : {}", _sandBoxPath);
}
} // namespace brayns

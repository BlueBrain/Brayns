/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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
#include <brayns/common/log.h>
#include <brayns/parameters/ParametersManager.h>

namespace
{
const std::string PARAM_BENCHMARKING = "enable-benchmark";
const std::string PARAM_ENGINE = "engine";
const std::string PARAM_HTTP_SERVER = "http-server";
const std::string PARAM_IMAGE_STREAM_FPS = "image-stream-fps";
const std::string PARAM_INPUT_PATHS = "input-paths";
const std::string PARAM_JPEG_COMPRESSION = "jpeg-compression";
const std::string PARAM_JPEG_SIZE = "jpeg-size";
const std::string PARAM_MAX_RENDER_FPS = "max-render-fps";
const std::string PARAM_MODULE = "module";
const std::string PARAM_PARALLEL_RENDERING = "parallel-rendering";
const std::string PARAM_PLUGIN = "plugin";
const std::string PARAM_STEREO = "stereo";
const std::string PARAM_WINDOW_SIZE = "window-size";

const size_t DEFAULT_WINDOW_WIDTH = 800;
const size_t DEFAULT_WINDOW_HEIGHT = 600;
const size_t DEFAULT_JPEG_COMPRESSION = 90;
}

namespace brayns
{
ApplicationParameters::ApplicationParameters()
    : AbstractParameters("Application")
    , _windowSize(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT)
    , _jpegCompression(DEFAULT_JPEG_COMPRESSION)
{
    _parameters.add_options()(PARAM_ENGINE.c_str(), po::value<std::string>(),
                              "Engine name [ospray|optix]")(
        PARAM_MODULE.c_str(), po::value<strings>(&_modules)->composing(),
        "OSPRay module name [string]")(PARAM_HTTP_SERVER.c_str(),
                                       po::value<std::string>(),
                                       "HTTP interface")(
        PARAM_INPUT_PATHS.c_str(), po::value<std::vector<std::string>>(),
        "List of files/folders to load data from")(
        PARAM_PLUGIN.c_str(), po::value<strings>()->composing(),
        "Dynamic plugin to load from LD_LIBRARY_PATH; "
        "can be repeated to load multiple plugins. "
        "Arguments to plugins can be added by inserting a space followed by "
        "the arguments like: --plugin 'myPluginName arg0 arg1'")(
        PARAM_WINDOW_SIZE.c_str(), po::value<uints>()->multitoken(),
        "Window size [int int]")(PARAM_BENCHMARKING.c_str(),
                                 po::bool_switch()->default_value(false),
                                 "Enable benchmarking")(
        PARAM_JPEG_COMPRESSION.c_str(), po::value<size_t>(),
        "JPEG compression rate (100 is full quality) [int]")(
        PARAM_JPEG_SIZE.c_str(), po::value<uints>()->multitoken(),
        "JPEG size [int int]")(
        PARAM_PARALLEL_RENDERING.c_str(),
        po::bool_switch()->default_value(false),
        "Enable parallel rendering, equivalent to --osp:mpi")(
        PARAM_STEREO.c_str(), po::bool_switch()->default_value(false),
        "Enable stereo rendering")(PARAM_IMAGE_STREAM_FPS.c_str(),
                                   po::value<size_t>(),
                                   "Image stream FPS (60 default), [int]")(
        PARAM_MAX_RENDER_FPS.c_str(), po::value<size_t>(), "Max. render FPS");

    _positionalArgs.add(PARAM_INPUT_PATHS.c_str(), -1);
}

void ApplicationParameters::parse(const po::variables_map& vm)
{
    if (vm.count(PARAM_ENGINE))
        _engine = vm[PARAM_ENGINE].as<std::string>();
    if (vm.count(PARAM_INPUT_PATHS))
        _inputPaths = vm[PARAM_INPUT_PATHS].as<strings>();
    if (vm.count(PARAM_HTTP_SERVER))
        _httpServerURI = vm[PARAM_HTTP_SERVER].as<std::string>();
    if (vm.count(PARAM_WINDOW_SIZE))
    {
        uints values = vm[PARAM_WINDOW_SIZE].as<uints>();
        if (values.size() == 2)
        {
            _windowSize.x() = values[0];
            _windowSize.y() = values[1];
        }
    }
    _benchmarking = vm[PARAM_BENCHMARKING].as<bool>();
    if (vm.count(PARAM_JPEG_COMPRESSION))
        _jpegCompression = vm[PARAM_JPEG_COMPRESSION].as<size_t>();
    _parallelRendering = vm[PARAM_PARALLEL_RENDERING].as<bool>();
    _stereo = vm[PARAM_STEREO].as<bool>();
    if (vm.count(PARAM_IMAGE_STREAM_FPS))
        _imageStreamFPS = vm[PARAM_IMAGE_STREAM_FPS].as<size_t>();
    if (vm.count(PARAM_MAX_RENDER_FPS))
        _maxRenderFPS = vm[PARAM_MAX_RENDER_FPS].as<size_t>();

    markModified();
}

void ApplicationParameters::print()
{
    AbstractParameters::print();
    BRAYNS_INFO << "Engine                      : " << _engine << std::endl;
    BRAYNS_INFO << "Ospray modules              : " << std::endl;
    for (const auto& module : _modules)
        BRAYNS_INFO << "- " << module << std::endl;
    BRAYNS_INFO << "Window size                 : " << _windowSize << std::endl;
    BRAYNS_INFO << "Benchmarking                : " << asString(_benchmarking)
                << std::endl;
    BRAYNS_INFO << "JPEG Compression            : " << _jpegCompression
                << std::endl;
    BRAYNS_INFO << "Image stream FPS            : " << _imageStreamFPS
                << std::endl;
    BRAYNS_INFO << "Max. render  FPS            : " << _maxRenderFPS
                << std::endl;
}
}

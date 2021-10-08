/* Copyright (c) 2015-2019, EPFL/Blue Brain Project
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

#include "RenderingParameters.h"
#include <brayns/common/log.h>

namespace
{
const std::string PARAM_ACCUMULATION = "disable-accumulation";
const std::string PARAM_BACKGROUND_COLOR = "background-color";
const std::string PARAM_CAMERA = "camera";
const std::string PARAM_HEAD_LIGHT = "no-head-light";
const std::string PARAM_MAX_ACCUMULATION_FRAMES = "max-accumulation-frames";
const std::string PARAM_RENDERER = "renderer";
const std::string PARAM_SPP = "samples-per-pixel";
const std::string PARAM_SUBSAMPLING = "subsampling";
const std::string PARAM_VARIANCE_THRESHOLD = "variance-threshold";
} // namespace

namespace brayns
{
RenderingParameters::RenderingParameters()
    : AbstractParameters("Rendering")
{
    _parameters.add_options() //
        (PARAM_RENDERER.c_str(), po::value<std::string>(),
         "The renderer to use") //
        (PARAM_SPP.c_str(), po::value<uint32_t>(&_spp),
         "Number of samples per pixel [uint]") //
        (PARAM_SUBSAMPLING.c_str(), po::value<uint32_t>(&_subsampling),
         "Subsampling factor [uint]") //
        (PARAM_ACCUMULATION.c_str(), po::bool_switch()->default_value(false),
         "Disable accumulation") //
        (PARAM_BACKGROUND_COLOR.c_str(), po::fixed_tokens_value<floats>(3, 3),
         "Background color [float float float]") //
        (PARAM_CAMERA.c_str(), po::value<std::string>(),
         "The camera to use") //
        (PARAM_HEAD_LIGHT.c_str(), po::bool_switch()->default_value(false),
         "Disable light source attached to camera origin.") //
        (PARAM_VARIANCE_THRESHOLD.c_str(),
         po::value<double>(&_varianceThreshold),
         "Threshold for adaptive accumulation [float]") //
        (PARAM_MAX_ACCUMULATION_FRAMES.c_str(),
         po::value<size_t>(&_maxAccumFrames),
         "Maximum number of accumulation frames");
}

void RenderingParameters::parse(const po::variables_map& vm)
{
    if (vm.count(PARAM_RENDERER))
    {
        const std::string& rendererName = vm[PARAM_RENDERER].as<std::string>();
        addRenderer(rendererName);
        _renderer = rendererName;
    }
    _accumulation = !vm[PARAM_ACCUMULATION].as<bool>();
    if (vm.count(PARAM_BACKGROUND_COLOR))
    {
        floats values = vm[PARAM_BACKGROUND_COLOR].as<floats>();
        _backgroundColor = Vector3f(values[0], values[1], values[2]);
    }
    if (vm.count(PARAM_CAMERA))
    {
        const std::string& cameraName = vm[PARAM_CAMERA].as<std::string>();
        _camera = cameraName;
        if (std::find(_cameras.begin(), _cameras.end(), cameraName) ==
            _cameras.end())
            _cameras.push_front(cameraName);
    }
    _headLight = !vm[PARAM_HEAD_LIGHT].as<bool>();
    markModified();
}

void RenderingParameters::print()
{
    AbstractParameters::print();
    BRAYNS_INFO << "Supported renderers               : " << std::endl;
    for (const auto& renderer : _renderers)
        BRAYNS_INFO << "- " << renderer << std::endl;
    BRAYNS_INFO << "Renderer                          : " << _renderer
                << std::endl;
    BRAYNS_INFO << "Samples per pixel                 : " << _spp << std::endl;
    BRAYNS_INFO << "Background color                  : " << _backgroundColor
                << std::endl;
    BRAYNS_INFO << "Camera                            : " << _camera
                << std::endl;
    BRAYNS_INFO << "Accumulation                      : "
                << asString(_accumulation) << std::endl;
    BRAYNS_INFO << "Max. accumulation frames          : " << _maxAccumFrames
                << std::endl;
}
} // namespace brayns

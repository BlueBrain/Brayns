/* Copyright 2015-2024 Blue Brain Project/EPFL
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
#include <brayns/common/Log.h>

namespace
{
constexpr auto PARAM_ACCUMULATION = "disable-accumulation";
constexpr auto PARAM_BACKGROUND_COLOR = "background-color";
constexpr auto PARAM_CAMERA = "camera";
constexpr auto PARAM_HEAD_LIGHT = "no-head-light";
constexpr auto PARAM_MAX_ACCUMULATION_FRAMES = "max-accumulation-frames";
constexpr auto PARAM_RENDERER = "renderer";
constexpr auto PARAM_SPP = "samples-per-pixel";
constexpr auto PARAM_SUBSAMPLING = "subsampling";
constexpr auto PARAM_VARIANCE_THRESHOLD = "variance-threshold";
} // namespace

namespace brayns
{
RenderingParameters::RenderingParameters()
    : AbstractParameters("Rendering")
{
    _parameters.add_options() //
        (PARAM_RENDERER, po::value<std::string>(),
         "The renderer to use") //
        (PARAM_SPP, po::value<uint32_t>(&_spp),
         "Number of samples per pixel [uint]") //
        (PARAM_SUBSAMPLING, po::value<uint32_t>(&_subsampling),
         "Subsampling factor [uint]") //
        (PARAM_ACCUMULATION, po::bool_switch()->default_value(false),
         "Disable accumulation") //
        (PARAM_BACKGROUND_COLOR,
         po::fixed_tokens_value<std::vector<float>>(3, 3),
         "Background color [float float float]") //
        (PARAM_CAMERA, po::value<std::string>(),
         "The camera to use") //
        (PARAM_HEAD_LIGHT, po::bool_switch()->default_value(false),
         "Disable light source attached to camera origin.") //
        (PARAM_VARIANCE_THRESHOLD, po::value<double>(&_varianceThreshold),
         "Threshold for adaptive accumulation [float]") //
        (PARAM_MAX_ACCUMULATION_FRAMES, po::value<size_t>(&_maxAccumFrames),
         "Maximum number of accumulation frames");
}

const std::string& RenderingParameters::getCurrentRenderer() const noexcept
{
    return _renderer;
}

void RenderingParameters::setCurrentRenderer(
    const std::string& renderer) noexcept
{
    _updateValue(_renderer, renderer);
}

const std::vector<std::string>& RenderingParameters::getRenderers() const
    noexcept
{
    return _renderers;
}

void RenderingParameters::addRenderer(const std::string& renderer) noexcept
{
    if (std::find(_renderers.begin(), _renderers.end(), renderer) ==
        _renderers.end())
        _renderers.push_back(renderer);
}

const std::string& RenderingParameters::getCurrentCamera() const noexcept
{
    return _camera;
}

const std::vector<std::string>& RenderingParameters::getCameras() const noexcept
{
    return _cameras;
}

void RenderingParameters::addCamera(const std::string& camera) noexcept
{
    auto it = std::find(_cameras.begin(), _cameras.end(), camera);
    if (it == _cameras.end())
        _cameras.push_back(camera);
}

uint32_t RenderingParameters::getSamplesPerPixel() const noexcept
{
    return _spp;
}

void RenderingParameters::setSamplesPerPixel(const uint32_t value) noexcept
{
    _updateValue(_spp, std::max(1u, value));
}

uint32_t RenderingParameters::getSubsampling() const noexcept
{
    return _subsampling;
}

void RenderingParameters::setSubsampling(const uint32_t subsampling) noexcept
{
    _updateValue(_subsampling, std::max(1u, subsampling));
}

const Vector3f& RenderingParameters::getBackgroundColor() const noexcept
{
    return _backgroundColor;
}

void RenderingParameters::setBackgroundColor(const Vector3f& value) noexcept
{
    _updateValue(_backgroundColor, value);
}

bool RenderingParameters::getHeadLight() const noexcept
{
    return _headLight;
}

void RenderingParameters::setHeadLight(bool headLight) noexcept
{
    _updateValue(_headLight, headLight);
}

bool RenderingParameters::getAccumulation() const noexcept
{
    return _accumulation;
}

void RenderingParameters::setAccumulation(bool accumulation) noexcept
{
    _updateValue(_accumulation, accumulation);
}

double RenderingParameters::getVarianceThreshold() const noexcept
{
    return _varianceThreshold;
}

void RenderingParameters::setVarianceThreshold(const double value) noexcept
{
    _updateValue(_varianceThreshold, value);
}

void RenderingParameters::setMaxAccumFrames(const size_t value) noexcept
{
    _updateValue(_maxAccumFrames, value);
}

size_t RenderingParameters::getMaxAccumFrames() const noexcept
{
    return _maxAccumFrames;
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
        auto values = vm[PARAM_BACKGROUND_COLOR].as<std::vector<float>>();
        _backgroundColor = Vector3f(values[0], values[1], values[2]);
    }
    if (vm.count(PARAM_CAMERA))
    {
        const std::string& cameraName = vm[PARAM_CAMERA].as<std::string>();
        _camera = cameraName;
        if (std::find(_cameras.begin(), _cameras.end(), cameraName) ==
            _cameras.end())
            _cameras.push_back(cameraName);
    }
    _headLight = !vm[PARAM_HEAD_LIGHT].as<bool>();
    markModified();
}

void RenderingParameters::print()
{
    AbstractParameters::print();
    Log::info("Supported renderers               :");
    for (const auto& renderer : _renderers)
        Log::info("- {}", renderer);
    Log::info("Renderer                          : {}", _renderer);
    Log::info("Samples per pixel                 : {}", _spp);
    Log::info("Background color                  : {}", _backgroundColor);
    Log::info("Camera                            : {}", _camera);
    Log::info("Accumulation                      : {}",
              asString(_accumulation));
    Log::info("Max. accumulation frames          : {}", _maxAccumFrames);
}
} // namespace brayns

/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
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

#ifndef RENDERINGPARAMETERS_H
#define RENDERINGPARAMETERS_H

#include <brayns/common/types.h>

#include "AbstractParameters.h"
#include <deque>

SERIALIZATION_ACCESS(RenderingParameters)

namespace brayns
{
class AbstractParameters;

/** Manages rendering parameters
 */
class RenderingParameters : public AbstractParameters
{
public:
    RenderingParameters();

    /** @copydoc AbstractParameters::print */
    void print() final;

    const std::string& getCurrentRenderer() const { return _renderer; }
    void setCurrentRenderer(const std::string& renderer)
    {
        _updateValue(_renderer, renderer);
    }
    /** All registered renderers */
    const auto& getRenderers() const { return _renderers; }
    void addRenderer(const std::string& renderer)
    {
        if (std::find(_renderers.begin(), _renderers.end(), renderer) ==
            _renderers.end())
            _renderers.push_front(renderer);
    }
    const std::string& getCurrentCamera() const { return _camera; }
    /** All registered cameras */
    const auto& getCameras() const { return _cameras; }
    void addCamera(const std::string& camera) { _cameras.push_front(camera); }
    /** Number of samples per pixel */
    uint32_t getSamplesPerPixel() const { return _spp; }
    void setSamplesPerPixel(const uint32_t value)
    {
        _updateValue(_spp, std::max(1u, value));
    }
    uint32_t getSubsampling() const { return _subsampling; }
    void setSubsampling(const uint32_t subsampling)
    {
        _updateValue(_subsampling, std::max(1u, subsampling));
    }
    const Vector3d& getBackgroundColor() const { return _backgroundColor; }
    void setBackgroundColor(const Vector3d& value)
    {
        _updateValue(_backgroundColor, value);
    }

    /**
       Light source follow camera origin
    */
    bool getHeadLight() const { return _headLight; }
    void setHeadLight(bool headLight) { _updateValue(_headLight, headLight); }
    /** If the rendering should be refined by accumulating multiple passes */
    bool getAccumulation() const { return _accumulation; }
    void setAccumulation(bool accumulation)
    {
        _updateValue(_accumulation, accumulation);
    }
    /**
     * @return the threshold where accumulation stops if the variance error
     * reaches this value.
     */
    double getVarianceThreshold() const { return _varianceThreshold; }
    /**
     * The threshold where accumulation stops if the variance error reaches this
     * value.
     */
    void setVarianceThreshold(const double value)
    {
        _updateValue(_varianceThreshold, value);
    }

    /**
     * The maximum number of accumulation frames before engine signals to stop
     * continuation of rendering.
     *
     * @sa Engine::continueRendering()
     */
    void setMaxAccumFrames(const size_t value)
    {
        _updateValue(_maxAccumFrames, value);
    }
    size_t getMaxAccumFrames() const { return _maxAccumFrames; }

protected:
    void parse(const po::variables_map& vm) final;

    std::string _renderer{"basic"};
    std::deque<std::string> _renderers;
    std::string _camera{"perspective"};
    std::deque<std::string> _cameras;
    uint32_t _spp{1};
    uint32_t _subsampling{1};
    bool _accumulation{true};
    Vector3d _backgroundColor{0., 0., 0.};
    bool _headLight{true};
    double _varianceThreshold{-1.};
    size_t _maxAccumFrames{10};

    SERIALIZATION_FRIEND(RenderingParameters)
};
} // namespace brayns
#endif // RENDERINGPARAMETERS_H

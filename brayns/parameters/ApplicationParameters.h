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

#ifndef APPLICATIONPARAMETERS_H
#define APPLICATIONPARAMETERS_H

#include "AbstractParameters.h"

#include <brayns/common/types.h>
#include <string>
#include <vector>

SERIALIZATION_ACCESS(ApplicationParameters)

namespace brayns
{
/** Manages application parameters
 */
class ApplicationParameters : public AbstractParameters
{
public:
    ApplicationParameters();

    /** @copydoc AbstractParameters::print */
    void print() final;

    /** Engine*/
    const std::string& getEngine() const { return _engine; }
    /** OSPRay modules */
    const std::vector<std::string>& getOsprayModules() const
    {
        return _modules;
    }

    bool getDynamicLoadBalancer() const { return _dynamicLoadBalancer; }
    void setDynamicLoadBalancer(const bool value)
    {
        _updateValue(_dynamicLoadBalancer, value);
    }

    /** window size */
    const Vector2ui getWindowSize() const { return Vector2ui(_windowSize); }
    void setWindowSize(const Vector2ui& size)
    {
        Vector2d value(size);
        _updateValue(_windowSize, value);
    }
    /** Benchmarking */
    bool isBenchmarking() const { return _benchmarking; }
    void setBenchmarking(bool enabled) { _benchmarking = enabled; }
    /** JPEG compression quality */
    void setJpegCompression(const size_t compression)
    {
        _updateValue(_jpegCompression, compression);
    }
    size_t getJpegCompression() const { return _jpegCompression; }
    /** Image stream FPS */
    size_t getImageStreamFPS() const { return _imageStreamFPS; }
    void setImageStreamFPS(const size_t fps)
    {
        _updateValue(_imageStreamFPS, fps);
    }

    /** Max render FPS to limit */
    size_t getMaxRenderFPS() const { return _maxRenderFPS; }
    bool isStereo() const { return _stereo; }
    bool getParallelRendering() const { return _parallelRendering; }
    const std::string& getHttpServerURI() const { return _httpServerURI; }
    void setHttpServerURI(const std::string& httpServerURI)
    {
        _updateValue(_httpServerURI, httpServerURI);
    }

    const std::string& getEnvMap() const {return _envMap;}

    const strings& getInputPaths() const { return _inputPaths; }
    po::positional_options_description& posArgs() { return _positionalArgs; }
protected:
    void parse(const po::variables_map& vm) final;

    std::string _engine{"ospray"};
    std::vector<std::string> _modules;
    Vector2d _windowSize;
    bool _benchmarking{false};
    size_t _jpegCompression;
    bool _stereo{false};
    size_t _imageStreamFPS{60};
    size_t _maxRenderFPS{std::numeric_limits<size_t>::max()};
    std::string _httpServerURI;
    bool _parallelRendering{false};
    bool _dynamicLoadBalancer{false};
    std::string _envMap;

    strings _inputPaths;

    po::positional_options_description _positionalArgs;

    SERIALIZATION_FRIEND(ApplicationParameters)
};
}

#endif // APPLICATIONPARAMETERS_H

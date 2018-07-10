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
struct PluginParam
{
    std::string name;
    std::vector<std::string> arguments;
};

/** Manages application parameters
 */
class ApplicationParameters : public AbstractParameters
{
public:
    ApplicationParameters();

    /** @copydoc AbstractParameters::print */
    void print() final;

    /** Engine*/
    EngineType getEngine() const { return _engine; }
    void setEngine(const EngineType name) { _updateValue(_engine, name); }
    const std::string& getEngineAsString(const EngineType value) const;
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

    /** Runtime plugins to load in Brayns::loadPlugins. */
    const std::vector<PluginParam>& getPlugins() const { return _plugins; }
    /** window size */
    const Vector2ui getWindowSize() const { return Vector2ui(_windowSize); }
    void setWindowSize(const Vector2ui& size)
    {
        Vector2f value(size);
        _updateValue(_windowSize, value);
    }
    /** Benchmarking */
    bool isBenchmarking() const { return _benchmarking; }
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
    const strings& getFilters() const { return _filters; }
    void setFrameExportFolder(const std::string& folder)
    {
        _updateValue(_frameExportFolder, folder);
    }
    std::string getFrameExportFolder() const { return _frameExportFolder; }
    /** Folder used by the application to store temporary files */
    std::string getTmpFolder() const { return _tmpFolder; }
    /** @return true if synchronous mode is enabled, aka rendering waits for
     * data loading. */
    bool getSynchronousMode() const { return _synchronousMode; }
    void setSynchronousMode(const bool synchronousMode)
    {
        _updateValue(_synchronousMode, synchronousMode);
    }

    bool getParallelRendering() const { return _parallelRendering; }
    const std::string& getHttpServerURI() const { return _httpServerURI; }
    void setHttpServerURI(const std::string& httpServerURI)
    {
        _updateValue(_httpServerURI, httpServerURI);
    }

    const strings& getInputPaths() const { return _inputPaths; }
    po::positional_options_description& posArgs() { return _positionalArgs; }
protected:
    void parse(const po::variables_map& vm) final;

    EngineType _engine{EngineType::ospray};
    std::vector<std::string> _modules;
    strings _pluginsRaw;
    std::vector<PluginParam> _plugins;
    Vector2f _windowSize;
    bool _benchmarking{false};
    size_t _jpegCompression;
    strings _filters;
    std::string _frameExportFolder;
    std::string _tmpFolder;
    bool _synchronousMode{false};
    size_t _imageStreamFPS{60};
    size_t _maxRenderFPS{std::numeric_limits<size_t>::max()};
    std::string _httpServerURI;
    bool _parallelRendering{false};
    bool _dynamicLoadBalancer{false};

    strings _inputPaths;

    po::positional_options_description _positionalArgs;

    SERIALIZATION_FRIEND(ApplicationParameters)
};
}

#endif // APPLICATIONPARAMETERS_H

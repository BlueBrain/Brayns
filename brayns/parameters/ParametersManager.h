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

#ifndef PARAMETERSMANAGER_H
#define PARAMETERSMANAGER_H

#include <boost/program_options.hpp>
#include <brayns/api.h>
#include <brayns/common/types.h>
#include <brayns/parameters/AnimationParameters.h>
#include <brayns/parameters/ApplicationParameters.h>
#include <brayns/parameters/GeometryParameters.h>
#include <brayns/parameters/RenderingParameters.h>
#include <brayns/parameters/SceneParameters.h>
#include <brayns/parameters/VolumeParameters.h>

namespace brayns
{
/**
   Class managing all parameters registered by the application. By default
   this class create an instance of Application, Rendering, and Geometry
   parameters are registered. Other parameters can also be added using the
   registerParameters method for as long as they are inherited from
   AbstractParameters.
 */
class ParametersManager
{
public:
    ParametersManager();

    /**
       Registers specific parameters to the manager
       @param parameters to be registered
     */
    BRAYNS_API void registerParameters(AbstractParameters* parameters);

    /**
       Displays usage of registered parameters
     */
    BRAYNS_API void usage();

    /**
       Displays values registered parameters
     */
    BRAYNS_API void print();

    /**
       Parses registered parameters
       @param argc number of command line parameters
       @param argv actual command line parameters
     */
    BRAYNS_API void parse(int argc, const char** argv);

    /**
       Gets animation parameters
       @return Animation parameters for the current scene
    */
    BRAYNS_API AnimationParameters& getAnimationParameters();

    /**
       Gets rendering parameters
       @return Rendering parameters for the current scene
    */
    BRAYNS_API RenderingParameters& getRenderingParameters();

    /**
       Gets geometry parameters
       @return Geometry parameters for the current scene
    */
    BRAYNS_API GeometryParameters& getGeometryParameters();

    /**
       Gets application parameters
       @return Application parameters for the current scene
    */
    BRAYNS_API ApplicationParameters& getApplicationParameters();
    BRAYNS_API const ApplicationParameters& getApplicationParameters() const;

    /**
       Gets scene parameters
       @return Parameters for the current scene
    */
    BRAYNS_API SceneParameters& getSceneParameters();

    /**
       Gets volume parameters
       @return Parameters for the current volume
    */
    BRAYNS_API VolumeParameters& getVolumeParameters();

    /**
       Sets a parameter (application, geometry, rendering, etc). If the
       parameter is not registered, the setting is ignored.
     */
    void set(const std::string& key, const std::string& value);

    /** Call resetModified on all parameters. */
    void resetModified();

    /**
     * @return true if any of the parameters has been modified since the last
     * resetModified().
     */
    bool isAnyModified() const;

private:
    void _processUnrecognizedOptions(
        const std::vector<std::string>& unrecognizedOptions) const;

    po::options_description _parameters;

    std::vector<AbstractParameters*> _parameterSets;
    AnimationParameters _animationParameters;
    ApplicationParameters _applicationParameters;
    GeometryParameters _geometryParameters;
    RenderingParameters _renderingParameters;
    SceneParameters _sceneParameters;
    VolumeParameters _volumeParameters;
};
}
#endif // PARAMETERSMANAGER_H

/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#pragma once

#include <brayns/parameters/AnimationParameters.h>
#include <brayns/parameters/ApplicationParameters.h>
#include <brayns/parameters/NetworkParameters.h>
#include <brayns/parameters/RenderingParameters.h>
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
    ParametersManager(int argc, const char** argv);

    /**
       Registers specific parameters to the manager
       @param parameters to be registered
     */
    void registerParameters(AbstractParameters* parameters);

    /**
       Displays usage of registered parameters
     */
    void usage();

    /**
       Displays values registered parameters
     */
    void print();

    /**
       Gets animation parameters
       @return Animation parameters for the current scene
    */
    AnimationParameters& getAnimationParameters();
    const AnimationParameters& getAnimationParameters() const;

    /**
       Gets rendering parameters
       @return Rendering parameters for the current scene
    */
    RenderingParameters& getRenderingParameters();
    const RenderingParameters& getRenderingParameters() const;

    /**
       Gets application parameters
       @return Application parameters for the current scene
    */
    ApplicationParameters& getApplicationParameters();
    const ApplicationParameters& getApplicationParameters() const;

    /**
       Gets volume parameters
       @return Parameters for the current volume
    */
    VolumeParameters& getVolumeParameters();
    const VolumeParameters& getVolumeParameters() const;

    /**
       Gets volume parameters
       @return Parameters for the current volume
    */
    NetworkParameters& getNetworkParameters();
    const NetworkParameters& getNetworkParameters() const;

    /** Call resetModified on all parameters. */
    void resetModified();

    /**
     * @return true if any of the parameters has been modified since the last
     * resetModified().
     */
    bool isAnyModified() const;

private:
    void _parse(int argc, const char** argv);
    void _processUnrecognizedOptions(
        const std::vector<std::string>& unrecognizedOptions) const;

    po::options_description _allOptions;

    std::vector<AbstractParameters*> _parameterSets;
    AnimationParameters _animationParameters;
    ApplicationParameters _applicationParameters;
    RenderingParameters _renderingParameters;
    VolumeParameters _volumeParameters;
    NetworkParameters _networkParameters;
};
} // namespace brayns

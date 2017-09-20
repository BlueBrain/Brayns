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

#include "ParametersManager.h"

#include <brayns/parameters/AbstractParameters.h>

namespace brayns
{
ParametersManager::ParametersManager()
{
    registerParameters(&_sceneParameters);
    registerParameters(&_applicationParameters);
    registerParameters(&_geometryParameters);
    registerParameters(&_renderingParameters);
    registerParameters(&_volumeParameters);
}

void ParametersManager::registerParameters(AbstractParameters* parameters)
{
    _parameterSets.push_back(parameters);
}

void ParametersManager::parse(int argc, const char** argv)
{
    bool help = false;
    for (int i = 0; i < argc && !help; ++i)
        if (std::string(argv[i]) == "--help")
            help = true;

    if (help)
    {
        usage();
        exit(0);
    }
    else
        for (AbstractParameters* parameters : _parameterSets)
            parameters->parse(argc, argv);
}

void ParametersManager::usage()
{
    for (AbstractParameters* parameters : _parameterSets)
        parameters->usage();
}

void ParametersManager::print()
{
    for (AbstractParameters* parameters : _parameterSets)
        parameters->print();
}

void ParametersManager::resetModified()
{
    for (AbstractParameters* parameters : _parameterSets)
        parameters->resetModified();
}

ApplicationParameters& ParametersManager::getApplicationParameters()
{
    return _applicationParameters;
}

RenderingParameters& ParametersManager::getRenderingParameters()
{
    return _renderingParameters;
}

GeometryParameters& ParametersManager::getGeometryParameters()
{
    return _geometryParameters;
}

SceneParameters& ParametersManager::getSceneParameters()
{
    return _sceneParameters;
}

VolumeParameters& ParametersManager::getVolumeParameters()
{
    return _volumeParameters;
}

void ParametersManager::set(const std::string& key, const std::string& value)
{
    for (AbstractParameters* parameters : _parameterSets)
        parameters->set(key, value);
}
}

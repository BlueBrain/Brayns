/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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

#include <brayns/common/parameters/AbstractParameters.h>

namespace brayns
{

ParametersManager::ParametersManager()
{
    registerParameters( &_applicationParameters );
    registerParameters( &_geometryParameters );
    registerParameters( &_renderingParameters );
}

void ParametersManager::registerParameters( AbstractParameters* parameters )
{
    _parameterSets.push_back( parameters );
}

void ParametersManager::parse( int argc, const char **argv )
{
    for( AbstractParameters* parameters: _parameterSets )
        if( !parameters->parse( argc, argv ))
        {
            printHelp();
            exit( 0 );
        }
}

void ParametersManager::printHelp( )
{
    for( AbstractParameters* parameters: _parameterSets )
        parameters->usage( );
}

void ParametersManager::print( )
{
    for( AbstractParameters* parameters: _parameterSets )
        parameters->print( );
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

}

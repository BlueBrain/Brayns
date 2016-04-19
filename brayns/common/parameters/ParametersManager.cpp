/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#include "ParametersManager.h"

#include <brayns/common/parameters/AbstractParameters.h>

namespace brayns
{

ParametersManager::ParametersManager()
{
    registerParameters( &_sceneParameters );
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
        parameters->parse( argc, argv );
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

void ParametersManager::set( const std::string& key, const std::string& value )
{
    for( AbstractParameters* parameters: _parameterSets )
        parameters->set( key, value );
}

}

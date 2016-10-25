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

#include "KeyboardHandler.h"

#include <brayns/parameters/ParametersManager.h>
#include <brayns/parameters/RenderingParameters.h>
#include <brayns/parameters/SceneParameters.h>
#include <brayns/parameters/VolumeParameters.h>
#include <brayns/common/scene/Scene.h>

#include <sstream>

namespace brayns
{

KeyboardHandler::KeyboardHandler(
    ScenePtr scene,
    ParametersManagerPtr parametersManager )
    : _parametersManager( parametersManager )
    , _scene( scene )
{
    registerKey('0', "Black background" );
    registerKey('1', "Gray background" );
    registerKey('2', "White background" );
    registerKey('6', "Default renderer" );
    registerKey('7', "Particle renderer" );
    registerKey('8', "Proximity renderer" );
    registerKey('9', "Simulation renderer" );
    registerKey('[', "Decrease timestamp by 1" );
    registerKey(']', "Increase timestamp by 1" );
    registerKey('e', "Enable eletron shading" );
    registerKey('o', "Decrease ambient occlusion strength" );
    registerKey('O', "Increase ambient occlusion strength" );
    registerKey('p', "Enable diffuse shading" );
    registerKey('P', "Disable shading" );
    registerKey('r', "Set timestamp to 0" );
    registerKey('R', "Set timestamp to infinity" );
    registerKey('s', "Enable/Disable shadows" );
    registerKey('S', "Enable/Disable soft shadows" );
    registerKey('t', "Multiply samples per ray by 2" );
    registerKey('T', "Divide samples per ray by 2" );
    registerKey('y', "Enable/Disable light emitting materials" );

}

void KeyboardHandler::processKey( const unsigned char key )
{
    RenderingParameters& renderParams = _parametersManager->getRenderingParameters( );
    SceneParameters& sceneParams = _parametersManager->getSceneParameters();
    VolumeParameters& volumeParams = _parametersManager->getVolumeParameters();

    switch( key )
    {
    case '0':
        renderParams.setBackgroundColor( Vector3f( 0.f, 0.f, 0.f ));
        break;
    case '1':
        renderParams.setBackgroundColor( Vector3f( 0.5f, 0.5f, 0.5f ));
        break;
    case '2':
        renderParams.setBackgroundColor( Vector3f( 1.f, 1.f, 1.f ));
        break;
    case '6':
        renderParams.setRenderer( "exobj" );
        break;
    case '7':
        renderParams.setRenderer( "particlerenderer" );
        break;
    case '8':
        renderParams.setRenderer( "proximityrenderer" );
        break;
    case '9':
        renderParams.setRenderer( "simulationrenderer" );
        break;
    case ']':
        {
            float ts = sceneParams.getTimestamp();
            sceneParams.setTimestamp( ts + 1 );
        }
        break;
    case '[':
        {
            float ts = sceneParams.getTimestamp();
            if( ts > 0.f )
                sceneParams.setTimestamp( ts - 1 );
        }
        break;
    case 'e':
        renderParams.setMaterialType( MT_ELECTRON );
        break;
    case 'o':
    {
        float aaStrength = renderParams.getAmbientOcclusionStrength();
        aaStrength += 0.1f;
        if( aaStrength>1.f ) aaStrength=1.f;
        renderParams.setAmbientOcclusionStrength( aaStrength );
        break;
    }
    case 'O':
    {
        float aaStrength = renderParams.getAmbientOcclusionStrength( );
        aaStrength -= 0.1f;
        if( aaStrength < 0.f ) aaStrength=0.f;
        renderParams.setAmbientOcclusionStrength( aaStrength );
        break;
    }
    case 'p':
        renderParams.setMaterialType( MT_DIFFUSE );
        break;
    case 'P':
        renderParams.setMaterialType( MT_NO_SHADING );
        break;
    case 'r':
        sceneParams.setTimestamp( 0.f );
        break;
    case 'R':
        sceneParams.setTimestamp( std::numeric_limits< size_t >::max( ));
        break;
    case 's':
        renderParams.setShadows(
            !renderParams.getShadows( ));
        break;
    case 'S':
        renderParams.setSoftShadows( !renderParams.getSoftShadows( ));
        break;
    case 't':
        volumeParams.setSamplesPerRay( volumeParams.getSamplesPerRay() * 2 );
        _scene->commitVolumeData();
        break;
    case 'T':
        if( volumeParams.getSamplesPerRay() >= 4 )
            volumeParams.setSamplesPerRay( volumeParams.getSamplesPerRay() / 2 );
        _scene->commitVolumeData();
        break;
    case 'Y':
        renderParams.setLightEmittingMaterials(
            !renderParams.getLightEmittingMaterials( ));
        break;
    }
}

void KeyboardHandler::registerKey( const unsigned char key, const std::string& description )
{
    if( _registeredKeys.find( key ) != _registeredKeys.end() )
    {
        std::stringstream message;
        message << key << " is already registered";
        BRAYNS_ERROR << message.str() << std::endl;
    }
    else
        _registeredKeys[ key ] = description;
}

std::string KeyboardHandler::help()
{
    std::stringstream result;
    result << "Keyboard shortcuts:\n";
    for( const auto& registeredKey: _registeredKeys )
        result << "- " << registeredKey.first
               << ": " + registeredKey.second << "\n";
    return result.str();
}

void KeyboardHandler::logDescription( const unsigned char key )
{
    if( _registeredKeys.find( key ) != _registeredKeys.end() )
        BRAYNS_INFO << "'" << key << "': " << _registeredKeys[ key ] << std::endl;
    else
        BRAYNS_ERROR << key << " shortcut is not defined" << std::endl;
}

}

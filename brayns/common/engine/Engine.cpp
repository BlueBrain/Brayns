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

#include "Engine.h"

#include <brayns/common/scene/Scene.h>
#include <brayns/common/input/KeyboardHandler.h>
#include <brayns/common/renderer/FrameBuffer.h>
#include <brayns/common/renderer/Renderer.h>
#include <brayns/common/camera/Camera.h>
#include <brayns/common/camera/FlyingModeManipulator.h>
#include <brayns/common/camera/InspectCenterManipulator.h>
#include <brayns/common/light/DirectionalLight.h>

#include <brayns/parameters/ParametersManager.h>

namespace brayns
{

Engine::Engine( ParametersManager& parametersManager )
    : _parametersManager( parametersManager )
    , _scene( 0 )
    , _camera( 0 )
    , _frameBuffer( 0 )
    , _keyboardHandler( 0 )
    , _dirty( true )
{
}

void Engine::setActiveRenderer( const std::string& renderer )
{
    if( _activeRenderer != renderer )
        _activeRenderer = renderer;
}

void Engine::reshape( const Vector2ui& frameSize )
{
    if( _frameBuffer->getSize() == frameSize )
        return;

    _frameBuffer->resize( frameSize );
    _camera->setAspectRatio(
        static_cast< float >( frameSize.x()) /
        static_cast< float >( frameSize.y()));
}

void Engine::commit()
{
    _frameBuffer->clear();
    _dirty = false;
}

void Engine::_render(
    const RenderInput& renderInput,
    RenderOutput& renderOutput )
{
    reshape( renderInput.windowSize );

    preRender();

    _camera->set( renderInput.position, renderInput.target, renderInput.up );
    _camera->commit();

    const Vector2i& frameSize = _frameBuffer->getSize();

    if( _parametersManager.getRenderingParameters().getHeadLight() )
    {
        LightPtr sunLight = _scene->getLight( 0 );
        DirectionalLight* sun = dynamic_cast< DirectionalLight* > ( sunLight.get() );
        if( sun )
        {
            sun->setDirection( _camera->getTarget() - _camera->getPosition() );
            _scene->commitLights();
        }
    }

    _render( );

    uint8_t* colorBuffer = _frameBuffer->getColorBuffer( );
    if( colorBuffer )
    {
        const size_t size = frameSize.x( ) * frameSize.y( ) * _frameBuffer->getColorDepth( );
        renderOutput.colorBuffer.assign( colorBuffer, colorBuffer + size );
    }

    float* depthBuffer = _frameBuffer->getDepthBuffer( );
    if( depthBuffer )
    {
        const size_t size = frameSize.x( ) * frameSize.y( );
        renderOutput.depthBuffer.assign( depthBuffer, depthBuffer + size );
    }

    postRender();
}

void Engine::_render()
{
    const Vector2i& frameSize = _frameBuffer->getSize();

    preRender();

    if( _parametersManager.getRenderingParameters().getHeadLight() )
    {
        LightPtr sunLight = _scene->getLight( 0 );
        DirectionalLight* sun =
            dynamic_cast< DirectionalLight* > ( sunLight.get() );
        if( sun )
        {
            sun->setDirection( _camera->getTarget() - _camera->getPosition() );
            _scene->commitLights();
        }
    }

    _camera->commit();
    setActiveRenderer( _parametersManager.getRenderingParameters().getRenderer( ));
    render();

    postRender();

    const Vector2ui windowSize = _parametersManager.getApplicationParameters().getWindowSize();
    if( windowSize != frameSize )
        reshape(windowSize);
}

void Engine::setDefaultCamera()
{
    const Vector2i& frameSize = _frameBuffer->getSize();

    const Boxf& worldBounds = _scene->getWorldBounds();
    const Vector3f& target = worldBounds.getCenter();
    const Vector3f& diag = worldBounds.getSize();
    Vector3f position = target;
    position.z() -= 1.5f * diag.find_max();

    const Vector3f up = Vector3f( 0.f, 1.f, 0.f );
    _camera->setInitialState( position, target, up );
    _camera->setAspectRatio(
        static_cast< float >( frameSize.x()) /
        static_cast< float >( frameSize.y()));

    BRAYNS_INFO << "World bounding box: " << worldBounds << std::endl;
    BRAYNS_INFO << "World center      : " << worldBounds.getCenter() << std::endl;
}

void Engine::setDefaultEpsilon()
{
    float epsilon = _parametersManager.getRenderingParameters().getEpsilon();
    if( epsilon == 0.f )
    {
        const Vector3f& worldBoundsSize = _scene->getWorldBounds().getSize();
        epsilon = worldBoundsSize.length() / 1e6f;
        BRAYNS_INFO << "Default epsilon: " << epsilon << std::endl;
        _parametersManager.getRenderingParameters().setEpsilon( epsilon );
    }
}

void Engine::setupCameraManipulator( const CameraMode mode )
{
    _cameraManipulator.reset(); // deregister previous keyboard handlers

    switch( mode )
    {
    case CameraMode::flying:
        _cameraManipulator.reset( new FlyingModeManipulator( *_camera, *_keyboardHandler ));
        break;
    case CameraMode::inspect:
        _cameraManipulator.reset( new InspectCenterManipulator( *_camera, *_keyboardHandler ));
        break;
    };
}

void Engine::registerKeyboardShortcuts()
{
    _keyboardHandler->registerKeyboardShortcut(
        '0', "Black background",
         std::bind( &Engine::_blackBackground, this ));
    _keyboardHandler->registerKeyboardShortcut(
        '1', "Gray background",
        std::bind( &Engine::_grayBackground, this ));
    _keyboardHandler->registerKeyboardShortcut(
        '2', "White background",
        std::bind( &Engine::_whiteBackground, this ));
    _keyboardHandler->registerKeyboardShortcut(
        '6', "Default renderer",
        std::bind( &Engine::_defaultRenderer, this ));
    _keyboardHandler->registerKeyboardShortcut(
        '7', "Particle renderer",
        std::bind( &Engine::_particleRenderer, this ));
    _keyboardHandler->registerKeyboardShortcut(
        '8', "Proximity renderer",
        std::bind( &Engine::_proximityRenderer, this ));
    _keyboardHandler->registerKeyboardShortcut(
        '9', "Simulation renderer",
        std::bind( &Engine::_simulationRenderer, this ));
    _keyboardHandler->registerKeyboardShortcut(
        '[', "Decrease timestamp by 1",
        std::bind( &Engine::_decreaseTimestamp, this ));
    _keyboardHandler->registerKeyboardShortcut(
        ']', "Increase timestamp by 1",
        std::bind( &Engine::_increaseTimestamp, this ));
    _keyboardHandler->registerKeyboardShortcut(
        'e', "Enable eletron shading",
        std::bind( &Engine::_electronShading, this ));
    _keyboardHandler->registerKeyboardShortcut(
        'f', "Enable fly mode",
        [this](){ setupCameraManipulator( CameraMode::flying ); });
    _keyboardHandler->registerKeyboardShortcut(
        'i', "Enable inspect mode",
        [this](){ setupCameraManipulator( CameraMode::inspect ); });
    _keyboardHandler->registerKeyboardShortcut(
        'o', "Decrease ambient occlusion strength",
        std::bind( &Engine::_decreaseAmbientOcclusionStrength, this ));
    _keyboardHandler->registerKeyboardShortcut(
        'O', "Increase ambient occlusion strength",
        std::bind( &Engine::_increaseAmbientOcclusionStrength, this ));
    _keyboardHandler->registerKeyboardShortcut(
        'p', "Enable diffuse shading",
        std::bind( &Engine::_diffuseShading, this ));
    _keyboardHandler->registerKeyboardShortcut(
        'P', "Disable shading",
        std::bind( &Engine::_disableShading, this ));
    _keyboardHandler->registerKeyboardShortcut(
        'r', "Set timestamp to 0",
        std::bind( &Engine::_resetTimestamp, this ));
    _keyboardHandler->registerKeyboardShortcut(
        'R', "Set timestamp to infinity",
        std::bind( &Engine::_infiniteTimestamp, this ));
    _keyboardHandler->registerKeyboardShortcut(
        'u', "Enable/Disable shadows",
        std::bind( &Engine::_toggleShadows, this ));
    _keyboardHandler->registerKeyboardShortcut(
        'U', "Enable/Disable soft shadows",
        std::bind( &Engine::_toggleSoftShadows, this ));
    _keyboardHandler->registerKeyboardShortcut(
        't', "Multiply samples per ray by 2",
        std::bind( &Engine::_increaseSamplesPerRay, this ));
    _keyboardHandler->registerKeyboardShortcut(
        'T', "Divide samples per ray by 2",
        std::bind( &Engine::_decreaseSamplesPerRay, this ));
    _keyboardHandler->registerKeyboardShortcut(
        'y', "Enable/Disable light emitting materials",
        std::bind( &Engine::_toggleLightEmittingMaterials, this ));
}

void Engine::_blackBackground()
{
    RenderingParameters& renderParams = _parametersManager.getRenderingParameters();
    renderParams.setBackgroundColor( Vector3f( 0.f, 0.f, 0.f ));
}

void Engine::_grayBackground()
{
    RenderingParameters& renderParams = _parametersManager.getRenderingParameters();
    renderParams.setBackgroundColor( Vector3f( 0.5f, 0.5f, 0.5f ));
}

void Engine::_whiteBackground()
{
    RenderingParameters& renderParams = _parametersManager.getRenderingParameters();
    renderParams.setBackgroundColor( Vector3f( 1.f, 1.f, 1.f ));
}

void Engine::_defaultRenderer()
{
    RenderingParameters& renderParams = _parametersManager.getRenderingParameters();
    renderParams.setRenderer( "exobj" );
}

void Engine::_particleRenderer()
{
    RenderingParameters& renderParams = _parametersManager.getRenderingParameters();
    renderParams.setRenderer( "particlerenderer" );
}

void Engine::_proximityRenderer()
{
    RenderingParameters& renderParams = _parametersManager.getRenderingParameters();
    renderParams.setRenderer( "proximityrenderer" );
}

void Engine::_simulationRenderer()
{
    RenderingParameters& renderParams = _parametersManager.getRenderingParameters();
    renderParams.setRenderer( "simulationrenderer" );
}

void Engine::_increaseTimestamp()
{
    SceneParameters& sceneParams = _parametersManager.getSceneParameters();
    float ts = sceneParams.getTimestamp();
    sceneParams.setTimestamp( ts + 1.f );
}

void Engine::_decreaseTimestamp()
{
    SceneParameters& sceneParams = _parametersManager.getSceneParameters();
    float ts = sceneParams.getTimestamp();
    if( ts > 0.f )
        sceneParams.setTimestamp( ts - 1.f );
}

void Engine::_diffuseShading()
{
    RenderingParameters& renderParams = _parametersManager.getRenderingParameters();
    renderParams.setShading( ShadingType::diffuse );
}

void Engine::_electronShading()
{
    RenderingParameters& renderParams = _parametersManager.getRenderingParameters();
    renderParams.setShading( ShadingType::electron );
}

void Engine::_disableShading()
{
    RenderingParameters& renderParams = _parametersManager.getRenderingParameters();
    renderParams.setShading( ShadingType::none );
}

void Engine::_increaseAmbientOcclusionStrength()
{
    RenderingParameters& renderParams = _parametersManager.getRenderingParameters();
    float aaStrength = renderParams.getAmbientOcclusionStrength();
    aaStrength += 0.1f;
    if( aaStrength>1.f )
        aaStrength = 1.f;
    renderParams.setAmbientOcclusionStrength( aaStrength );
}

void Engine::_decreaseAmbientOcclusionStrength()
{
    RenderingParameters& renderParams = _parametersManager.getRenderingParameters();
    float aaStrength = renderParams.getAmbientOcclusionStrength( );
    aaStrength -= 0.1f;
    if( aaStrength < 0.f )
        aaStrength = 0.f;
    renderParams.setAmbientOcclusionStrength( aaStrength );
}

void Engine::_resetTimestamp()
{
    SceneParameters& sceneParams = _parametersManager.getSceneParameters();
    sceneParams.setTimestamp( 0.f );
}

void Engine::_infiniteTimestamp()
{
    SceneParameters& sceneParams = _parametersManager.getSceneParameters();
    sceneParams.setTimestamp( std::numeric_limits< size_t >::max() );
}

void Engine::_toggleShadows()
{
    RenderingParameters& renderParams = _parametersManager.getRenderingParameters();
    renderParams.setShadows( !renderParams.getShadows() );
}

void Engine::_toggleSoftShadows()
{
    RenderingParameters& renderParams = _parametersManager.getRenderingParameters();
    renderParams.setSoftShadows( !renderParams.getSoftShadows() );
}

void Engine::_increaseSamplesPerRay()
{
    VolumeParameters& volumeParams = _parametersManager.getVolumeParameters();
    volumeParams.setSamplesPerRay( volumeParams.getSamplesPerRay() * 2 );
    _scene->commitVolumeData();
}

void Engine::_decreaseSamplesPerRay()
{
    VolumeParameters& volumeParams = _parametersManager.getVolumeParameters();
    if( volumeParams.getSamplesPerRay() >= 4 )
        volumeParams.setSamplesPerRay( volumeParams.getSamplesPerRay() / 2 );
    _scene->commitVolumeData();
}

void Engine::_toggleLightEmittingMaterials()
{
    RenderingParameters& renderParams = _parametersManager.getRenderingParameters();
    renderParams.setLightEmittingMaterials( !renderParams.getLightEmittingMaterials() );
}



}

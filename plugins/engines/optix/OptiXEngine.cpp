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

#include "OptiXEngine.h"

#include <brayns/common/log.h>

#include <plugins/engines/optix/OptiXRenderer.h>
#include <plugins/engines/optix/OptiXScene.h>
#include <plugins/engines/optix/OptiXFrameBuffer.h>
#include <plugins/engines/optix/OptiXCamera.h>

namespace brayns
{

OptiXEngine::OptiXEngine(
    int /*argc*/,
    const char ** /*argv*/,
    ParametersManagerPtr parametersManager )
    : Engine()
    , _context( 0 )
{
    BRAYNS_INFO << "Initializing OptiX" << std::endl;
    // Set up context
    _context = optix::Context::create();
    if( !_context )
        BRAYNS_THROW( std::runtime_error( "Failed to initialize OptiX" ));

    _context->setRayTypeCount( 2 );
    _context->setEntryPointCount( 1 );
    _context->setStackSize( 2800 );

    BRAYNS_INFO << "Context " << &_context << std::endl;

    BRAYNS_INFO << "Initializing renderers" << std::endl;
    _activeRenderer =
        parametersManager->getRenderingParameters().getRenderer();

    _rendererNames = parametersManager->getRenderingParameters().getRenderers();

    Renderers renderersForScene;
    for( std::string renderer: _rendererNames )
    {
        _renderers[renderer].reset(
            new OptiXRenderer( renderer, *parametersManager, _context ));
        renderersForScene.push_back( _renderers[renderer] );
    }

    BRAYNS_INFO << "Initializing scene" << std::endl;
    _scene.reset( new OptiXScene(
        renderersForScene,
        parametersManager->getSceneParameters(),
        parametersManager->getGeometryParameters(),
        _context));

    _scene->setMaterials( MT_DEFAULT, NB_MAX_MATERIALS );

    BRAYNS_INFO << "Initializing frame buffer" << std::endl;
    _frameSize =
        parametersManager->getApplicationParameters( ).getWindowSize( );

    const bool accumulation = parametersManager->getApplicationParameters().getFilters().empty( );

    _frameBuffer.reset( new OptiXFrameBuffer(
        _frameSize, FBF_RGBA_I8, accumulation, _context ));
    _camera.reset( new OptiXCamera(
        parametersManager->getRenderingParameters().getCameraType(),
        _context));
    BRAYNS_INFO << "Engine initialization complete" << std::endl;
}

OptiXEngine::~OptiXEngine()
{
    if( _context )
    {
        _context->destroy();
        _context = 0;
    }
}

std::string OptiXEngine::name() const
{
    return "OptiX";
}

void OptiXEngine::commit()
{
    for( std::string renderer: _rendererNames )
    {
        _renderers[renderer]->setScene( _scene );
        _renderers[renderer]->setCamera( _camera );
        _renderers[renderer]->commit( );
    }
    _camera->commit( );
}

void OptiXEngine::render()
{
    _frameBuffer->map();
    _scene->commitSimulationData();
    _renderers[_activeRenderer]->render( _frameBuffer );
    _frameBuffer->unmap();
}

void OptiXEngine::preRender()
{
}

void OptiXEngine::postRender()
{
}

}

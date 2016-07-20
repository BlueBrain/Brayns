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

#include "OSPRayEngine.h"

#include <plugins/engines/ospray/render/OSPRayRenderer.h>
#include <plugins/engines/ospray/render/OSPRayScene.h>
#include <plugins/engines/ospray/render/OSPRayFrameBuffer.h>
#include <plugins/engines/ospray/render/OSPRayCamera.h>

namespace brayns
{

OSPRayEngine::OSPRayEngine(
    int argc,
    const char **argv,
    ParametersManagerPtr parametersManager )
{
    BRAYNS_INFO << "Initializing OSPRay" << std::endl;
    try
    {
        ospInit( &argc, argv );
    }
    catch( std::runtime_error& )
    {
        // Note: This is necessary because OSPRay does not yet implement a
        // ospDestroy API.
        BRAYNS_WARN << "OSPRay is already initialized. Did you call it twice? "
                    << std::endl;
    }

    BRAYNS_INFO << "Initializing renderers" << std::endl;
    _activeRenderer =
        parametersManager->getRenderingParameters().getRenderer();

    _rendererNames = parametersManager->getRenderingParameters().getRenderers();

    Renderers renderersForScene;
    for( std::string renderer: _rendererNames )
    {
        _renderers[renderer].reset(
            new OSPRayRenderer( renderer, *parametersManager ));
        renderersForScene.push_back( _renderers[renderer] );
    }

    BRAYNS_INFO << "Initializing scene" << std::endl;
    _scene.reset( new OSPRayScene( renderersForScene,
        parametersManager->getSceneParameters(),
        parametersManager->getGeometryParameters()));

    _scene->setMaterials( MT_DEFAULT, NB_MAX_MATERIALS );

    BRAYNS_INFO << "Initializing frame buffer" << std::endl;
    _frameSize =
        parametersManager->getApplicationParameters( ).getWindowSize( );
    _frameBuffer.reset( new OSPRayFrameBuffer( _frameSize, FBF_RGBA_I8 ));
    _camera.reset( new OSPRayCamera(
        parametersManager->getRenderingParameters().getCameraType( )));
    BRAYNS_INFO << "Engine initialization complete" << std::endl;
}

std::string OSPRayEngine::name() const
{
    return "ospray";
}

void OSPRayEngine::commit()
{
    for( std::string renderer: _rendererNames )
    {
        _renderers[renderer]->setScene( _scene );
        _renderers[renderer]->setCamera( _camera );
        _renderers[renderer]->commit( );
    }
    _camera->commit( );
}

void OSPRayEngine::render()
{
    _renderers[_activeRenderer]->render( _frameBuffer );
}

void OSPRayEngine::preRender()
{
    _frameBuffer->map();
}

void OSPRayEngine::postRender()
{
    _frameBuffer->unmap();
}

}

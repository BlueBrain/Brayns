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

#include <brayns/common/input/KeyboardHandler.h>

#include <plugins/engines/ospray/OSPRayCamera.h>
#include <plugins/engines/ospray/OSPRayFrameBuffer.h>
#include <plugins/engines/ospray/OSPRayRenderer.h>
#include <plugins/engines/ospray/OSPRayScene.h>

namespace brayns
{
OSPRayEngine::OSPRayEngine(int argc, const char** argv,
                           ParametersManager& parametersManager)
    : Engine(parametersManager)
{
    BRAYNS_INFO << "Initializing OSPRay" << std::endl;
    try
    {
        ospInit(&argc, argv);
    }
    catch (std::runtime_error&)
    {
        // Note: This is necessary because OSPRay does not yet implement a
        // ospDestroy API.
        BRAYNS_WARN << "OSPRay is already initialized. Did you call it twice? "
                    << std::endl;
    }

    BRAYNS_INFO << "Initializing renderers" << std::endl;
    _activeRenderer = _parametersManager.getRenderingParameters().getRenderer();

    Renderers renderersForScene;
    for (const auto renderer :
         parametersManager.getRenderingParameters().getRenderers())
    {
        const auto& rendererName =
            parametersManager.getRenderingParameters().getRendererAsString(
                renderer);
        _renderers[renderer].reset(
            new OSPRayRenderer(rendererName, _parametersManager));
        renderersForScene.push_back(_renderers[renderer]);
    }

    BRAYNS_INFO << "Initializing scene" << std::endl;
    _scene.reset(new OSPRayScene(renderersForScene, _parametersManager));

    _scene->setMaterials(MT_DEFAULT, NB_MAX_MATERIALS);

    BRAYNS_INFO << "Initializing frame buffer" << std::endl;
    _frameSize = _parametersManager.getApplicationParameters().getWindowSize();

    const bool accumulation =
        _parametersManager.getApplicationParameters().getFilters().empty();

    _frameBuffer.reset(
        new OSPRayFrameBuffer(_frameSize, FBF_RGBA_I8, accumulation));
    _camera.reset(new OSPRayCamera(
        _parametersManager.getRenderingParameters().getCameraType()));

    BRAYNS_INFO << "Engine initialization complete" << std::endl;
}

OSPRayEngine::~OSPRayEngine()
{
}

std::string OSPRayEngine::name() const
{
    return "ospray";
}

void OSPRayEngine::commit()
{
    Engine::commit();
    for (const auto& renderer : _renderers)
    {
        _renderers[renderer.first]->setScene(_scene);
        _renderers[renderer.first]->setCamera(_camera);
        _renderers[renderer.first]->commit();
    }
    _camera->commit();
}

void OSPRayEngine::render()
{
    Engine::render();
    _scene->commitVolumeData();
    _scene->commitSimulationData();
    _renderers[_activeRenderer]->commit();
    _renderers[_activeRenderer]->render(_frameBuffer);
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

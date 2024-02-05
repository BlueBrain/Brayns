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

#include "Engine.h"

#include <brayns/engine/Camera.h>
#include <brayns/engine/FrameBuffer.h>
#include <brayns/engine/Renderer.h>
#include <brayns/engine/Scene.h>

#include <brayns/parameters/ParametersManager.h>

namespace brayns
{
Engine::Engine(ParametersManager& parametersManager)
    : _parametersManager(parametersManager)
{
}

void Engine::commit()
{
    _renderer->commit();
}

void Engine::preRender()
{
    _frameExporter.preRender(getCamera(), getRenderer(), getFrameBuffer(),
                             getParametersManager());

    if (!mustRender())
        return;

    const auto& renderParams = _parametersManager.getRenderingParameters();
    if (!renderParams.isModified())
        return;
    for (auto frameBuffer : _frameBuffers)
    {
        frameBuffer->setAccumulation(renderParams.getAccumulation());
        frameBuffer->setSubsampling(renderParams.getSubsampling());
    }
}

void Engine::render()
{
    if (!mustRender())
        return;

    for (auto frameBuffer : _frameBuffers)
    {
        _camera->setBufferTarget(frameBuffer->getName());
        _camera->commit();
        _camera->resetModified();
        _renderer->render(frameBuffer);
    }
}

void Engine::postRender()
{
    _frameExporter.postRender(getFrameBuffer());

    if (!mustRender())
        return;

    for (auto frameBuffer : _frameBuffers)
        frameBuffer->incrementAccumFrames();
}

Renderer& Engine::getRenderer()
{
    return *_renderer;
}

FrameExporter& Engine::getFrameExporter() noexcept
{
    return _frameExporter;
}

bool Engine::continueRendering() const
{
    auto frameBuffer = _frameBuffers[0];
    return (frameBuffer->getAccumulation() &&
            (frameBuffer->numAccumFrames() <
             _parametersManager.getRenderingParameters().getMaxAccumFrames()));
}

void Engine::addFrameBuffer(FrameBufferPtr frameBuffer)
{
    _frameBuffers.push_back(frameBuffer);
}

void Engine::removeFrameBuffer(FrameBufferPtr frameBuffer)
{
    _frameBuffers.erase(std::remove(_frameBuffers.begin(), _frameBuffers.end(),
                                    frameBuffer),
                        _frameBuffers.end());
}

void Engine::clearFrameBuffers()
{
    for (auto frameBuffer : _frameBuffers)
        frameBuffer->clear();
}

void Engine::resetFrameBuffers()
{
    for (auto frameBuffer : _frameBuffers)
        frameBuffer->resetModified();
}

void Engine::addRendererType(const std::string& name,
                             const PropertyMap& properties)
{
    _parametersManager.getRenderingParameters().addRenderer(name);
    getRenderer().setProperties(name, properties);
}

void Engine::addCameraType(const std::string& name,
                           const PropertyMap& properties)
{
    _parametersManager.getRenderingParameters().addCamera(name);
    getCamera().setProperties(name, properties);
}

bool Engine::mustRender()
{
    if (_parametersManager.getApplicationParameters()
            .getUseQuantaRenderControl())
    {
        // When playing an animation:
        //  - A frame data from the animation gets loaded
        //  (Model::commitSimulationData())
        //      - This triggers the scene to be marked as modified
        //          - This triggers Brayns to clear the frame buffer accumation
        //          frames
        //  This is way the animations keep working even though the camera might
        //  be fixed at a specific location and direction

        // Do not render if camera hasnt been modified and either there is no
        // accumulation frames or they are completed for the current pass
        auto frameBuffer = _frameBuffers[0];
        if (!_camera->isModified() &&
            (!frameBuffer->getAccumulation() ||
             (frameBuffer->getAccumulation() &&
              frameBuffer->numAccumFrames() >=
                  _parametersManager.getRenderingParameters()
                      .getMaxAccumFrames())))
            return false;
    }

    return true;
}
} // namespace brayns

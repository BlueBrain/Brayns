/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
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

#include <brayns/common/ImageManager.h>

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
    for (auto frameBuffer : _frameBuffers)
        frameBuffer->incrementAccumFrames();
}

bool Engine::continueRendering() const
{
    auto frameBuffer = _frameBuffers[0];
    return _parametersManager.getAnimationParameters().isPlaying() ||
           (frameBuffer->getAccumulation() &&
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
} // namespace brayns

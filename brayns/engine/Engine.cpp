/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <brayns/common/Log.h>
#include <brayns/engine/Engine.h>
#include <brayns/engine/EngineObjectRegisterer.h>
#include <brayns/engine/FrameRenderer.h>
#include <brayns/engine/cameras/PerspectiveCamera.h>
#include <brayns/engine/renderers/InteractiveRenderer.h>

#include <thread>

namespace brayns
{
Engine::Engine(const ParametersManager& parameters)
 : _params(parameters)
{
    _device = ospNewDevice("cpu");

    const auto logLevel = OSPLogLevel::OSP_LOG_WARNING;
    const auto logOutput = "cout";
    const auto logErrorOutput = "cerr";
    ospDeviceSetParam(_device, "logLevel", OSPDataType::OSP_INT, &logLevel);
    ospDeviceSetParam(_device, "logOutput", OSPDataType::OSP_STRING, logOutput);
    ospDeviceSetParam(_device, "errorOutput", OSPDataType::OSP_STRING, logErrorOutput);

    ospDeviceCommit(_device);
    const auto error = ospDeviceGetLastErrorCode(_device);
    if(error != OSPError::OSP_NO_ERROR)
    {
        const auto ospErrorMessage = ospDeviceGetLastErrorMsg(_device);
        Log::critical("Could not initialize OSPRay device: {}", ospErrorMessage);
        throw std::runtime_error("Could not initialize OSPRay device");
    }

    EngineObjectRegisterer::registerDefaultTypes(*this);

    // Default camera and renderer
    _camera = std::make_unique<PerspectiveCamera>();
    _renderer = std::make_unique<InteractiveRenderer>();
}

Engine::~Engine()
{
    if(_device)
        ospDeviceRelease(_device);

    ospShutdown();
}

void Engine::preRender()
{
    const auto& animation = _params.getAnimationParameters();

    _scene.preRender(animation);
}

void Engine::commit()
{
    if(!_keepRunning)
        return;

    // update statistics
    const auto avgFPS = _fpsCounter.getAverageFPS();
    _statistics.setFPS(avgFPS);

    // Update changes on the viewport
    auto& appParams = _params.getApplicationParameters();
    if(appParams.isModified())
    {
        const auto& frameSize = appParams.getWindowSize();
        const auto aspectRatio = static_cast<float>(frameSize.x) / static_cast<float>(frameSize.y);

        _frameBuffer.setFrameSize(frameSize);
        _camera->setAspectRatio(aspectRatio);
    }

    // Clear the framebuffer if something changed, so that we do not accumulate on top of old stuff,
    // which would produce an incorrect image
    const bool somethingChanged = _frameBuffer.isModified() || _scene.isModified()
            || _camera->isModified() || _renderer->isModified();

    if(somethingChanged)
        _frameBuffer.clear();

    _frameBuffer.commit();
    _camera->commit();
    _renderer->commit();
    _scene.commit();

    const auto sceneSize = _scene._getSizeBytes();
    _statistics.setSceneSizeInBytes(sceneSize);
}

void Engine::render()
{
    if(!_keepRunning)
        return;

    // Check wether we should keep rendering or not
    const auto maxSpp = _renderer->getSamplesPerPixel();
    const auto currentSpp = _frameBuffer.numAccumFrames();
    if(currentSpp >= maxSpp)
        return;

    // A frame is counted from the momment we start rendering until we come again to the same point.
    _fpsCounter.endFrame();
    const auto& appParams = _params.getApplicationParameters();
    const auto maxFPS = appParams.getMaxRenderFPS();
    const auto minFrameTimeMilis = (1.0 / static_cast<double>(maxFPS)) * 1000.0;

    const auto avgFPS = _fpsCounter.getAverageFPS();
    const auto avgFrameTimeMillis = (1.0 / static_cast<double>(avgFPS)) * 1000.0;

    const auto diff = avgFrameTimeMillis - minFrameTimeMilis;

    if(diff < 0.0)
    {
        Log::info("Slowing down FPS. Last frame rendered {} ms too fast", diff);
        std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(diff));
    }

    // Start measuring a new frame render time.
    _fpsCounter.startFrame();

    FrameRenderer::render(*_camera, _frameBuffer, *_renderer, _scene);

    _frameBuffer.incrementAccumFrames();
}

void Engine::postRender()
{
    _scene.postRender();
}

Scene &Engine::getScene()
{
    return _scene;
}

FrameBuffer &Engine::getFrameBuffer() noexcept
{
    return _frameBuffer;
}

Camera &Engine::getCamera() noexcept
{
    return *_camera;
}

EngineObjectFactory<Camera> &Engine::getCameraFactory() noexcept
{
    return _cameraFactory;
}

Renderer &Engine::getRenderer() noexcept
{
    return *_renderer;
}

EngineObjectFactory<Renderer> &Engine::getRendererFactory() noexcept
{
    return _rendererFactory;
}

void Engine::setRunning(bool keepRunning) noexcept
{
    _keepRunning = keepRunning;
}

bool Engine::isRunning() const noexcept
{
    return _keepRunning;
}

const Statistics &Engine::getStatistics() const noexcept
{
    return _statistics;
}

const ParametersManager &Engine::getParametersManager() const noexcept
{
    return _params;
}
} // namespace brayns

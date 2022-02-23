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
#include <brayns/engine/DefaultEngineObjects.h>
#include <brayns/engine/Engine.h>

#include <ospray/version.h>

#include <thread>

namespace brayns
{
Engine::Engine(const ParametersManager& parameters)
 : _params(parameters)
{
    try
    {
        // Setup log and error output
        std::vector<const char *> argv =
        {
            "--osp:log-level=warning",
            "--osp:log-output=cout",
            "--osp:error-output=cerr"
        };

        auto argc = static_cast<int>(argv.size());
        const auto error = ospInit(&argc, argv.data());

        switch(error)
        {
        case OSPError::OSP_OUT_OF_MEMORY:
            throw std::runtime_error("Cannot initialize OSPRay: out of memory");
            break;
        case OSPError::OSP_UNSUPPORTED_CPU:
            throw std::runtime_error("Cannot initialize OSPRay: unsupported CPU");
            break;
        case OSPError::OSP_INVALID_ARGUMENT:
            throw std::runtime_error("Cannot initialize OSPRay: invalid argument given to to ospInit()");
            break;
        case OSPError::OSP_INVALID_OPERATION:
        case OSPError::OSP_UNKNOWN_ERROR:
        case OSPError::OSP_VERSION_MISMATCH: // We are not loading any module, so this shouldn't happen ...
            throw std::runtime_error("Cannot initialize OSPRay: Unknown error");
            break;
        default:
            break;
        }
    }
    catch (const std::exception &e)
    {
        // Note: This is necessary because OSPRay does not yet implement a
        // ospDestroy API.
        Log::error("{}", e.what());

        auto device = ospGetCurrentDevice();
        auto errorMessage = ospDeviceGetLastErrorMsg(device);
        Log::error("OSPRay message: {}", errorMessage);
    }

    // Register core renderers, cameras, materials and lights
    DefaultEngineObjects::registerObjects(*this);
}

Engine::~Engine()
{
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

    auto ospFrameBuffer = _frameBuffer.handle();
    auto ospRenderer = _renderer->handle();
    auto ospCamera = _camera->handle();
    auto ospWorld = _scene.handle();

    auto ospRenderTask = ospRenderFrame(ospFrameBuffer, ospRenderer, ospCamera, ospWorld);
    ospWait(ospRenderTask);

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

Renderer &Engine::getRenderer() noexcept
{
    return *_renderer;
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

EngineFactories &Engine::getObjectFactories() noexcept
{
    return _engineFactory;
}
} // namespace brayns

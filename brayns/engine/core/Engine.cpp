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

#include "Engine.h"

#include <brayns/engine/camera/projections/Perspective.h>
#include <brayns/engine/core/FrameRenderer.h>
#include <brayns/engine/renderer/types/Interactive.h>
#include <brayns/utils/Log.h>

#include <thread>

namespace
{
class OsprayLogLevel
{
public:
    static OSPLogLevel fromParameters(const brayns::ApplicationParameters &params)
    {
        const auto systemLogLevel = params.getLogLevel();
        switch (systemLogLevel)
        {
        case brayns::LogLevel::Off:
            return OSPLogLevel::OSP_LOG_NONE;
        case brayns::LogLevel::Critical:
        case brayns::LogLevel::Error:
            return OSPLogLevel::OSP_LOG_ERROR;
        case brayns::LogLevel::Warn:
            return OSPLogLevel::OSP_LOG_WARNING;
        case brayns::LogLevel::Info:
            return OSPLogLevel::OSP_LOG_INFO;
        case brayns::LogLevel::Debug:
        case brayns::LogLevel::Trace:
        case brayns::LogLevel::Count:
            return OSPLogLevel::OSP_LOG_DEBUG;
        }

        return OSPLogLevel::OSP_LOG_NONE;
    }
};

class OsprayDeviceInitializer
{
public:
    static ospray::cpp::Device init(const brayns::ParametersManager &parameters)
    {
        auto device = ospray::cpp::Device("cpu");

        device.setErrorCallback(
            [](void *data, OSPError error, const char *message)
            {
                (void)data;
                brayns::Log::error("[OSPRAY] Error {}: {}", error, message);
                throw std::runtime_error(message);
            });

        device.setStatusCallback(
            [](void *data, const char *message)
            {
                (void)data;
                brayns::Log::debug("[OSPRAY] {}", message);
            });

        auto &appParams = parameters.getApplicationParameters();
        const auto logLevel = OsprayLogLevel::fromParameters(appParams);
        device.setParam("logLevel", logLevel);
        device.commit();
        device.setCurrent();

        return device;
    }
};
}

namespace brayns
{
Engine::OsprayModuleHandler::OsprayModuleHandler()
{
    ospLoadModule("cpu");
}

Engine::OsprayModuleHandler::~OsprayModuleHandler()
{
    ospShutdown();
}

Engine::Engine(ParametersManager &parameters)
    : _params(parameters)
    , _osprayDevice(OsprayDeviceInitializer::init(parameters))
    , _camera(Perspective())
    , _renderer(Interactive())
{
    EngineFactoriesInitializer::init(_factories);
}

void Engine::commitAndRender()
{
    // Pre render engine
    preRender();

    // Commit any change to the engine (scene, camera, renderer, parameters, ...)
    commit();

    // Render new frame, if needed
    render();

    // The parameters are modified on network update, and processed on engine.preRender and engine.commit
    _params.resetModified();

    // Post render engine
    postRender();
}

void Engine::preRender()
{
    _scene.preRender(_params);
}

void Engine::commit()
{
    if (!_keepRunning)
    {
        return;
    }

    // Update changes on the viewport
    auto &appParams = _params.getApplicationParameters();
    auto &frameSize = appParams.getWindowSize();
    _frameBuffer.setFrameSize(frameSize);
    _camera.setAspectRatio(_frameBuffer.getAspectRatio());

    bool needResetFramebuffer = false;
    if (_frameBuffer.commit())
    {
        Log::debug("[Engine] Framebuffer committed");
        needResetFramebuffer = true;
    }

    if (_camera.commit())
    {
        Log::debug("[Engine] Camera committed");
        needResetFramebuffer = true;
    }

    if (_renderer.commit())
    {
        Log::debug("[Engine] Renderer committed");
        needResetFramebuffer = true;
    }

    if (_scene.commit())
    {
        Log::debug("[Engine] Scene committed");
        needResetFramebuffer = true;
    }

    if (needResetFramebuffer)
    {
        _frameBuffer.clear();
    }
}

void Engine::render()
{
    if (!_keepRunning)
    {
        return;
    }

    // Check wether we should keep rendering or not
    auto maxSpp = _renderer.getSamplesPerPixel();
    auto currentSpp = _frameBuffer.numAccumFrames();
    if (currentSpp >= maxSpp)
    {
        return;
    }

    FrameRenderer::synchronous(_camera, _frameBuffer, _renderer, _scene);
    _frameBuffer.incrementAccumFrames();
}

void Engine::postRender()
{
    if (!_keepRunning)
    {
        return;
    }
    _scene.postRender(_params);
}

Scene &Engine::getScene()
{
    return _scene;
}

Framebuffer &Engine::getFramebuffer() noexcept
{
    return _frameBuffer;
}

Camera &Engine::getCamera() noexcept
{
    return _camera;
}

Renderer &Engine::getRenderer() noexcept
{
    return _renderer;
}

EngineFactories &Engine::getFactories() noexcept
{
    return _factories;
}

void Engine::setRunning(bool keepRunning) noexcept
{
    _keepRunning = keepRunning;
}

bool Engine::isRunning() const noexcept
{
    return _keepRunning;
}

const ParametersManager &Engine::getParametersManager() const noexcept
{
    return _params;
}
} // namespace brayns

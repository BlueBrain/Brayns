/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Jafet Villafranca <jafet.villafrancadiaz@epfl.ch>
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

#include "Brayns.h"
#include "EngineFactory.h"
#include "PluginManager.h"

#include <brayns/common/Log.h>
#include <brayns/common/MathTypes.h>
#include <brayns/common/PixelFormat.h>
#include <brayns/common/Timer.h>

#include <brayns/engine/Camera.h>
#include <brayns/engine/Engine.h>
#include <brayns/engine/FrameBuffer.h>
#include <brayns/engine/Light.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/Renderer.h>
#include <brayns/engine/Scene.h>

#include <brayns/pluginapi/PluginAPI.h>

#include <brayns/utils/DynamicLib.h>
#include <brayns/utils/StringUtils.h>

#include <mutex>
#include <thread>

namespace
{
const brayns::Vector3f DEFAULT_SUN_DIRECTION = {1.f, -1.f, -1.f};
const brayns::Vector3f DEFAULT_SUN_COLOR = {0.9f, 0.9f, 0.9f};
constexpr double DEFAULT_SUN_ANGULAR_DIAMETER = 0.53;
constexpr double DEFAULT_SUN_INTENSITY = 1.0;
} // namespace

namespace brayns
{
struct Brayns::Impl : public PluginAPI
{
    Impl(int argc, const char **argv)
        : _parametersManager{argc, argv}
        , _engineFactory{argc, argv, _parametersManager}
        , _pluginManager{argc, argv}
    {
        Log::info("");
        Log::info(" _|_|_|");
        Log::info(" _|    _|  _|  _|_|    _|_|_|  _|    _|  _|_|_|      _|_|_|  ");
        Log::info(" _|_|_|    _|_|      _|    _|  _|    _|  _|    _|  _|_|     ");
        Log::info(" _|    _|  _|        _|    _|  _|    _|  _|    _|      _|_| ");
        Log::info(" _|_|_|    _|          _|_|_|    _|_|_|  _|    _|  _|_|_|   ");
        Log::info("                                    _|                     ");
        Log::info("                                  _|_|                       ");
        Log::info("");

        // This initialization must happen before plugin intialization.
        _createEngine();

        // Plugin init before frame buffer creation needed by OpenDeck plugin
        _pluginManager.initPlugins(this);
        _createFrameBuffer();

        _loadData();

        _engine->getScene().commit(); // Needed to obtain a bounding box
        _adjustCamera();

        auto network = _pluginManager.getNetworkManager();
        if (network)
        {
            network->start();
        }
    }

    ~Impl()
    {
        // make sure that plugin objects are removed first, as plugins are
        // destroyed before the engine, but plugin destruction still should have
        // a valid engine and _api (aka this object).
        _loaderRegistry.clear();
        _pluginManager.destroyPlugins();
    }

    bool commit()
    {
        std::unique_lock<std::mutex> lock{_renderMutex, std::defer_lock};
        if (!lock.try_lock())
            return false;

        _pluginManager.preRender();

        auto &scene = _engine->getScene();
        auto &lightManager = scene.getLightManager();
        const auto &rp = _parametersManager.getRenderingParameters();
        auto &camera = _engine->getCamera();

        // Need to update head light before scene is committed
        if (rp.getHeadLight() && (camera.isModified() || rp.isModified()))
        {
            const auto newDirection = glm::rotate(camera.getOrientation(), Vector3d(0, 0, -1));
            _sunLight->_direction = newDirection;
            lightManager.addLight(_sunLight);
        }

        scene.commit();

        _engine->getStatistics().setSceneSizeInBytes(scene.getSizeInBytes());

        auto &renderer = _engine->getRenderer();
        renderer.setCurrentType(rp.getCurrentRenderer());

        const auto windowSize = _parametersManager.getApplicationParameters().getWindowSize();

        if (camera.hasProperty("aspect"))
        {
            camera.updateProperty("aspect", static_cast<double>(windowSize.x) / static_cast<double>(windowSize.y));
        }
        for (auto frameBuffer : _frameBuffers)
            frameBuffer->resize(windowSize);

        _engine->preRender();

        camera.commit();

        _engine->commit();

        if (_parametersManager.isAnyModified() || camera.isModified() || scene.isModified() || renderer.isModified()
            || lightManager.isModified())
        {
            _engine->clearFrameBuffers();
        }

        _parametersManager.resetModified();
        camera.resetModified();
        scene.resetModified();
        renderer.resetModified();
        lightManager.resetModified();

        return true;
    }

    void render()
    {
        std::lock_guard<std::mutex> lock{_renderMutex};

        _renderTimer.start();
        _engine->render();
        _renderTimer.stop();
        _lastFPS = _renderTimer.perSecondSmoothed();

        const auto &params = _parametersManager.getApplicationParameters();
        const auto fps = params.getMaxRenderFPS();
        const auto delta = _lastFPS - fps;
        if (delta > 0)
        {
            const int64_t targetTime = (1. / fps) * 1000.f;
            std::this_thread::sleep_for(std::chrono::milliseconds(targetTime - _renderTimer.milliseconds()));
        }
    }

    void postRender()
    {
        _engine->getStatistics().setFPS(_lastFPS);

        _engine->postRender();

        _pluginManager.postRender();

        _engine->resetFrameBuffers();
        _engine->getStatistics().resetModified();
    }

    NetworkManager *getNetworkManager()
    {
        return _pluginManager.getNetworkManager();
    }

    Engine &getEngine() final
    {
        return *_engine;
    }

    ParametersManager &getParametersManager() final
    {
        return _parametersManager;
    }

    Camera &getCamera() final
    {
        return _engine->getCamera();
    }

    Renderer &getRenderer() final
    {
        return _engine->getRenderer();
    }

    LoaderRegistry &getLoaderRegistry() final
    {
        return _loaderRegistry;
    }

    void triggerRender() final
    {
        _engine->triggerRender();
    }

    INetworkInterface *getNetworkInterface() final
    {
        return _networkInterface.get();
    }

    void setNetworkInterface(std::unique_ptr<INetworkInterface> interface) final
    {
        _networkInterface = std::move(interface);
    }

    Scene &getScene() final
    {
        return _engine->getScene();
    }

private:
    void _createEngine()
    {
        _engine = _engineFactory.create("braynsOSPRayEngine");
        if (!_engine)
            throw std::runtime_error("Could not allocate ospray engine");

        // Default sun light
        _sunLight = std::make_shared<DirectionalLight>(
            DEFAULT_SUN_DIRECTION,
            DEFAULT_SUN_ANGULAR_DIAMETER,
            DEFAULT_SUN_COLOR,
            DEFAULT_SUN_INTENSITY,
            false);
        _engine->getScene().getLightManager().addLight(_sunLight);

        _engine->getCamera().setCurrentType(_parametersManager.getRenderingParameters().getCurrentCamera());
        _engine->getRenderer().setCurrentType(_parametersManager.getRenderingParameters().getCurrentRenderer());
    }

    void _createFrameBuffer()
    {
        if (!_engine->getFrameBuffers().empty())
            return;

        _addFrameBuffer("default");
    }

    void _addFrameBuffer(const std::string &name)
    {
        const auto &ap = _parametersManager.getApplicationParameters();
        const auto frameSize = ap.getWindowSize();

        auto frameBuffer = _engine->createFrameBuffer(name, frameSize, PixelFormat::RGBA_I8);
        _engine->addFrameBuffer(frameBuffer);
        _frameBuffers.push_back(frameBuffer);
    }

    void _loadData()
    {
        auto &scene = _engine->getScene();

        const auto &paths = _parametersManager.getApplicationParameters().getInputPaths();
        if (!paths.empty())
        {
            if (paths.size() == 1 && paths[0] == "demo")
            {
                _engine->getScene().buildDefault();
                return;
            }

            for (const auto &path : paths)
                if (!_loaderRegistry.isSupportedFile(path))
                    throw std::runtime_error("No loader found for '" + path + "'");

            for (const auto &path : paths)
            {
                int percentageLast = 0;
                std::string msgLast;
                auto timeLast = std::chrono::steady_clock::now();

                Log::info("Loading '{}'.", path);

                auto progress = [&](const std::string &msg, float t)
                {
                    constexpr auto MIN_SECS = 5;
                    constexpr auto MIN_PERCENTAGE = 10;

                    t = std::max(0.f, std::min(t, 1.f));
                    const int percentage = static_cast<int>(100.0f * t);
                    const auto time = std::chrono::steady_clock::now();
                    const auto secondsElapsed =
                        std::chrono::duration_cast<std::chrono::seconds>(time - timeLast).count();
                    const auto percentageElapsed = percentage - percentageLast;

                    if ((secondsElapsed >= MIN_SECS && percentageElapsed > 0) || msgLast != msg
                        || (percentageElapsed >= MIN_PERCENTAGE))
                    {
                        std::string p = std::to_string(percentage);
                        p.insert(p.begin(), 3 - p.size(), ' ');

                        Log::info("[{}%] {}.", p, msg);
                        msgLast = msg;
                        percentageLast = percentage;
                        timeLast = time;
                    }
                };

                const auto &loader = _loaderRegistry.getSuitableLoader(path, "", "");

                auto models = loader.loadFromFile(path, {progress}, {}, scene);

                // No properties passed, use command line defaults.
                ModelParams params(path, path, {});
                scene.addModels(models, params);
            }
        }
        scene.markModified();
    }

    void _adjustCamera()
    {
        // Extract data
        auto &camera = _engine->getCamera();
        auto &scene = _engine->getScene();
        auto &bounds = scene.getBounds();

        // Camera position
        auto position = bounds.getCenter();
        auto size = bounds.isEmpty() ? 1 : glm::compMax(bounds.getSize());
        position.z += size;

        // Camera target
        auto target = position;

        // Set Camera state
        camera.setInitialState(position, glm::identity<Quaterniond>(), target);

        // Log camera status
        if (bounds.isEmpty())
        {
            Log::info("World bounding box: empty.");
        }
        else
        {
            Log::info("World bounding box: {}.", bounds);
        }
        Log::info("World center      : {}.", bounds.getCenter());
    }

    ParametersManager _parametersManager;
    EngineFactory _engineFactory;
    PluginManager _pluginManager;
    Engine *_engine{nullptr};
    LoaderRegistry _loaderRegistry;
    std::vector<FrameBufferPtr> _frameBuffers;

    // protect render() vs commit() when doing all the commits
    std::mutex _renderMutex;

    Timer _renderTimer;
    std::atomic<double> _lastFPS;

    std::unique_ptr<INetworkInterface> _networkInterface;
    std::shared_ptr<DirectionalLight> _sunLight;
};

// -----------------------------------------------------------------------------

Brayns::Brayns(int argc, const char **argv)
    : _impl(std::make_unique<Impl>(argc, argv))
{
}

Brayns::~Brayns() = default;

bool Brayns::commitAndRender()
{
    if (_impl->commit())
    {
        _impl->render();
        _impl->postRender();
    }
    return _impl->getEngine().getKeepRunning();
}

bool Brayns::commit()
{
    return _impl->commit();
}

void Brayns::render()
{
    return _impl->render();
}

void Brayns::postRender()
{
    _impl->postRender();
}

Engine &Brayns::getEngine()
{
    return _impl->getEngine();
}

LoaderRegistry &Brayns::getLoaderRegistry()
{
    return _impl->getLoaderRegistry();
}

ParametersManager &Brayns::getParametersManager()
{
    return _impl->getParametersManager();
}

NetworkManager *Brayns::getNetworkManager()
{
    return _impl->getNetworkManager();
}
} // namespace brayns

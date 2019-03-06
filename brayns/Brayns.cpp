/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
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

#include <brayns/common/PropertyMap.h>
#include <brayns/common/Timer.h>
#include <brayns/common/input/KeyboardHandler.h>
#include <brayns/common/light/Light.h>
#include <brayns/common/log.h>
#include <brayns/common/mathTypes.h>
#include <brayns/common/utils/DynamicLib.h>
#include <brayns/common/utils/utils.h>

#include <brayns/engine/Camera.h>
#include <brayns/engine/Engine.h>
#include <brayns/engine/FrameBuffer.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/Renderer.h>
#include <brayns/engine/Scene.h>

#include <brayns/manipulators/FlyingModeManipulator.h>
#include <brayns/manipulators/InspectCenterManipulator.h>

#include <brayns/parameters/ParametersManager.h>

#if BRAYNS_USE_LIBARCHIVE
#include <brayns/io/ArchiveLoader.h>
#endif
#if BRAYNS_USE_ASSIMP
#include <brayns/io/MeshLoader.h>
#endif
#include <brayns/io/ProteinLoader.h>
#include <brayns/io/VolumeLoader.h>
#include <brayns/io/XYZBLoader.h>

#include <brayns/pluginapi/PluginAPI.h>

#include <thread>

namespace
{
const float DEFAULT_MOTION_ACCELERATION = 1.5f;

const brayns::Vector3f DEFAULT_SUN_DIRECTION = {1.f, -1.f, -1.f};
const brayns::Vector3f DEFAULT_SUN_COLOR = {0.9f, 0.9f, 0.9f};
constexpr double DEFAULT_SUN_ANGULAR_DIAMETER = 0.53;
constexpr double DEFAULT_SUN_INTENSITY = 1.0;
} // namespace

namespace brayns
{
struct Brayns::Impl : public PluginAPI
{
    Impl(int argc, const char** argv)
        : _parametersManager{argc, argv}
        , _engineFactory{argc, argv, _parametersManager}
        , _pluginManager{argc, argv}
    {
        // This initialization must happen before plugin intialization.
        _createEngine();
        _registerKeyboardShortcuts();
        _setupCameraManipulator(CameraMode::inspect, false);

        // Loaders before plugin init since 'Brain Atlas' uses the mesh loader
        _registerLoaders();

        // Plugin init before frame buffer creation needed by OpenDeck plugin
        _pluginManager.initPlugins(this);
        _createFrameBuffer();

        _loadData();

        _engine->getScene().commit(); // Needed to obtain a bounding box
        _cameraManipulator->adjust(_engine->getScene().getBounds());
    }

    ~Impl()
    {
        // make sure that plugin objects are removed first, as plugins are
        // destroyed before the engine, but plugin destruction still should have
        // a valid engine and _api (aka this object).
        _engine->getScene().getLoaderRegistry().clear();
        _pluginManager.destroyPlugins();
    }

    bool commit()
    {
        auto lock{_engine->getDeferredRenderScopeLock()};

        if (!lock.try_lock())
            return false;

        _pluginManager.preRender();

        auto& scene = _engine->getScene();
        auto& lightManager = scene.getLightManager();
        const auto& rp = _parametersManager.getRenderingParameters();
        auto& camera = _engine->getCamera();

        // Need to update head light before scene is committed
        if (rp.getHeadLight() && (camera.isModified() || rp.isModified()))
        {
            const auto newDirection =
                glm::rotate(camera.getOrientation(), Vector3d(0, 0, -1));
            _sunLight->_direction = newDirection;
            lightManager.addLight(_sunLight);
        }

        scene.commit();

        _engine->getStatistics().setSceneSizeInBytes(scene.getSizeInBytes());

        _parametersManager.getAnimationParameters().update();

        auto& renderer = _engine->getRenderer();
        renderer.setCurrentType(rp.getCurrentRenderer());

        const auto windowSize =
            _parametersManager.getApplicationParameters().getWindowSize();

        if (camera.hasProperty("aspect"))
        {
            camera.updateProperty("aspect",
                                  static_cast<double>(windowSize.x) /
                                      static_cast<double>(windowSize.y));
        }
        for (auto frameBuffer : _frameBuffers)
            frameBuffer->resize(windowSize);

        _engine->preRender();

        camera.commit();

        _engine->commit();

        if (_parametersManager.isAnyModified() || camera.isModified() ||
            scene.isModified() || renderer.isModified() ||
            lightManager.isModified())
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
        auto lock{_engine->getRenderScopeLock()};

        _renderTimer.start();
        _engine->render();
        _renderTimer.stop();
        _lastFPS = _renderTimer.perSecondSmoothed();

        const auto& params = _parametersManager.getApplicationParameters();
        const auto fps = params.getMaxRenderFPS();
        const auto delta = _lastFPS - fps;
        if (delta > 0)
        {
            const int64_t targetTime = (1. / fps) * 1000.f;
            std::this_thread::sleep_for(std::chrono::milliseconds(
                targetTime - _renderTimer.milliseconds()));
        }
    }

    void postRender(RenderOutput* output)
    {
        auto lock{_engine->getRenderScopeLock()};

        if (output)
            _updateRenderOutput(*output);

        _engine->getStatistics().setFPS(_lastFPS);

        _pluginManager.postRender();

        _engine->postRender();

        _engine->resetFrameBuffers();
        _engine->getStatistics().resetModified();
    }

    bool commit(const RenderInput& renderInput)
    {
        _engine->getCamera().set(renderInput.position, renderInput.orientation,
                                 renderInput.target);
        _parametersManager.getApplicationParameters().setWindowSize(
            renderInput.windowSize);

        return commit();
    }

    void _updateRenderOutput(RenderOutput& renderOutput)
    {
        FrameBuffer& frameBuffer = _engine->getFrameBuffer();
        frameBuffer.map();
        const Vector2i& frameSize = frameBuffer.getSize();
        const auto colorBuffer = frameBuffer.getColorBuffer();
        if (colorBuffer)
        {
            const size_t size =
                frameSize.x * frameSize.y * frameBuffer.getColorDepth();
            renderOutput.colorBuffer.assign(colorBuffer, colorBuffer + size);
            renderOutput.colorBufferFormat = frameBuffer.getFrameBufferFormat();
        }

        const auto depthBuffer = frameBuffer.getDepthBuffer();
        if (depthBuffer)
        {
            const size_t size = frameSize.x * frameSize.y;
            renderOutput.depthBuffer.assign(depthBuffer, depthBuffer + size);
        }

        renderOutput.frameSize = frameSize;

        frameBuffer.unmap();
    }

    Engine& getEngine() final { return *_engine; }
    ParametersManager& getParametersManager() final
    {
        return _parametersManager;
    }
    KeyboardHandler& getKeyboardHandler() final { return _keyboardHandler; }
    AbstractManipulator& getCameraManipulator() final
    {
        return *_cameraManipulator;
    }
    Camera& getCamera() final { return _engine->getCamera(); }
    Renderer& getRenderer() final { return _engine->getRenderer(); }
    void triggerRender() final { _engine->triggerRender(); }
    ActionInterface* getActionInterface() final
    {
        return _actionInterface.get();
    }
    void setActionInterface(const ActionInterfacePtr& interface) final
    {
        _actionInterface = interface;
    }
    Scene& getScene() final { return _engine->getScene(); }

private:
    void _createEngine()
    {
        auto engineName =
            _parametersManager.getApplicationParameters().getEngine();

        if (toLowercase(engineName) == "optix")
            engineName = "braynsOptixEngine";
        else if (toLowercase(engineName) == "ospray")
            engineName = "braynsOSPRayEngine";

        _engine = _engineFactory.create(engineName);
        if (!_engine)
            throw std::runtime_error("Unsupported engine: " + engineName);

        // Default sun light
        _sunLight =
            std::make_shared<DirectionalLight>(DEFAULT_SUN_DIRECTION,
                                               DEFAULT_SUN_ANGULAR_DIAMETER,
                                               DEFAULT_SUN_COLOR,
                                               DEFAULT_SUN_INTENSITY, false);
        _engine->getScene().getLightManager().addLight(_sunLight);

        _engine->getCamera().setCurrentType(
            _parametersManager.getRenderingParameters().getCurrentCamera());
    }

    void _createFrameBuffer()
    {
        if (!_engine->getFrameBuffers().empty())
            return;

        const auto& ap = _parametersManager.getApplicationParameters();
        const auto names =
            ap.isStereo() ? strings{"0L", "0R"} : strings{"default"};
        for (const auto& name : names)
            _addFrameBuffer(name);
    }

    void _addFrameBuffer(const std::string& name)
    {
        const auto& ap = _parametersManager.getApplicationParameters();
        const auto frameSize = ap.getWindowSize();

        auto frameBuffer =
            _engine->createFrameBuffer(name, frameSize,
                                       FrameBufferFormat::rgba_i8);
        _engine->addFrameBuffer(frameBuffer);
        _frameBuffers.push_back(frameBuffer);
    }

    void _registerLoaders()
    {
        auto& registry = _engine->getScene().getLoaderRegistry();
        auto& scene = _engine->getScene();

        auto params = _parametersManager.getGeometryParameters();

        registry.registerLoader(std::make_unique<ProteinLoader>(scene, params));
        registry.registerLoader(std::make_unique<RawVolumeLoader>(scene));
        registry.registerLoader(std::make_unique<MHDVolumeLoader>(scene));
        registry.registerLoader(std::make_unique<XYZBLoader>(scene));
#if BRAYNS_USE_ASSIMP
        registry.registerLoader(std::make_unique<MeshLoader>(scene, params));
#endif
#if BRAYNS_USE_LIBARCHIVE
        registry.registerArchiveLoader(
            std::make_unique<ArchiveLoader>(scene, registry));
#endif
    }

    void _loadData()
    {
        auto& scene = _engine->getScene();
        const auto& registry = scene.getLoaderRegistry();

        const auto& paths =
            _parametersManager.getApplicationParameters().getInputPaths();
        if (!paths.empty())
        {
            if (paths.size() == 1 && paths[0] == "demo")
            {
                _engine->getScene().buildDefault();
                return;
            }

            for (const auto& path : paths)
                if (!registry.isSupportedFile(path))
                    throw std::runtime_error("No loader found for '" + path +
                                             "'");

            for (const auto& path : paths)
            {
                int percentageLast = 0;
                std::string msgLast;
                auto timeLast = std::chrono::steady_clock::now();

                BRAYNS_INFO << "Loading '" << path << "'" << std::endl;

                auto progress = [&](const std::string& msg, float t) {
                    constexpr auto MIN_SECS = 5;
                    constexpr auto MIN_PERCENTAGE = 10;

                    t = std::max(0.f, std::min(t, 1.f));
                    const int percentage = static_cast<int>(100.0f * t);
                    const auto time = std::chrono::steady_clock::now();
                    const auto secondsElapsed =
                        std::chrono::duration_cast<std::chrono::seconds>(
                            time - timeLast)
                            .count();
                    const auto percentageElapsed = percentage - percentageLast;

                    if ((secondsElapsed >= MIN_SECS && percentageElapsed > 0) ||
                        msgLast != msg || (percentageElapsed >= MIN_PERCENTAGE))
                    {
                        std::string p = std::to_string(percentage);
                        p.insert(p.begin(), 3 - p.size(), ' ');

                        BRAYNS_INFO << "[" << p << "%] " << msg << std::endl;
                        msgLast = msg;
                        percentageLast = percentage;
                        timeLast = time;
                    }
                };

                // No properties passed, use command line defaults.
                ModelParams params(path, path, {});
                scene.loadModel(path, params, {progress});
            }
        }
        scene.setEnvironmentMap(
            _parametersManager.getApplicationParameters().getEnvMap());
        scene.markModified();
    }

    void _setupCameraManipulator(const CameraMode mode,
                                 const bool adjust = true)
    {
        _cameraManipulator.reset();

        switch (mode)
        {
        case CameraMode::flying:
            _cameraManipulator.reset(
                new FlyingModeManipulator(_engine->getCamera(),
                                          _keyboardHandler));
            break;
        case CameraMode::inspect:
            _cameraManipulator.reset(
                new InspectCenterManipulator(_engine->getCamera(),
                                             _keyboardHandler));
            break;
        };

        if (adjust)
            _cameraManipulator->adjust(_engine->getScene().getBounds());
    }

    void _registerKeyboardShortcuts()
    {
        _keyboardHandler.registerKeyboardShortcut(
            '0', "Black background",
            std::bind(&Brayns::Impl::_blackBackground, this));
        _keyboardHandler.registerKeyboardShortcut(
            '1', "Gray background",
            std::bind(&Brayns::Impl::_grayBackground, this));
        _keyboardHandler.registerKeyboardShortcut(
            '2', "White background",
            std::bind(&Brayns::Impl::_whiteBackground, this));
        _keyboardHandler.registerKeyboardShortcut(
            '3', "Set gradient materials",
            std::bind(&Brayns::Impl::_gradientMaterials, this));
        _keyboardHandler.registerKeyboardShortcut(
            '4', "Set random materials",
            std::bind(&Brayns::Impl::_randomMaterials, this));
        _keyboardHandler.registerKeyboardShortcut(
            '5', "Scientific visualization renderer",
            std::bind(&Brayns::Impl::_scivisRenderer, this));
        _keyboardHandler.registerKeyboardShortcut(
            '6', "Default renderer",
            std::bind(&Brayns::Impl::_defaultRenderer, this));
        _keyboardHandler.registerKeyboardShortcut(
            '7', "Basic simulation renderer",
            std::bind(&Brayns::Impl::_basicSimulationRenderer, this));
        _keyboardHandler.registerKeyboardShortcut(
            '8', "Advanced Simulation renderer",
            std::bind(&Brayns::Impl::_advancedSimulationRenderer, this));
        _keyboardHandler.registerKeyboardShortcut(
            '9', "Proximity renderer",
            std::bind(&Brayns::Impl::_proximityRenderer, this));
        _keyboardHandler.registerKeyboardShortcut(
            '[', "Decrease animation frame by 1",
            std::bind(&Brayns::Impl::_decreaseAnimationFrame, this));
        _keyboardHandler.registerKeyboardShortcut(
            ']', "Increase animation frame by 1",
            std::bind(&Brayns::Impl::_increaseAnimationFrame, this));
        _keyboardHandler.registerKeyboardShortcut(
            'e', "Enable eletron shading",
            std::bind(&Brayns::Impl::_electronShading, this));
        _keyboardHandler.registerKeyboardShortcut(
            'f', "Enable fly mode", [this]() {
                Brayns::Impl::_setupCameraManipulator(CameraMode::flying);
            });
        _keyboardHandler.registerKeyboardShortcut(
            'i', "Enable inspect mode", [this]() {
                Brayns::Impl::_setupCameraManipulator(CameraMode::inspect);
            });
        _keyboardHandler.registerKeyboardShortcut(
            'o', "Decrease ambient occlusion strength",
            std::bind(&Brayns::Impl::_decreaseAmbientOcclusionStrength, this));
        _keyboardHandler.registerKeyboardShortcut(
            'O', "Increase ambient occlusion strength",
            std::bind(&Brayns::Impl::_increaseAmbientOcclusionStrength, this));
        _keyboardHandler.registerKeyboardShortcut(
            'p', "Enable diffuse shading",
            std::bind(&Brayns::Impl::_diffuseShading, this));
        _keyboardHandler.registerKeyboardShortcut(
            'P', "Disable shading",
            std::bind(&Brayns::Impl::_disableShading, this));
        _keyboardHandler.registerKeyboardShortcut(
            'r', "Set animation frame to 0",
            std::bind(&Brayns::Impl::_resetAnimationFrame, this));
        _keyboardHandler.registerKeyboardShortcut(
            'u', "Enable/Disable shadows",
            std::bind(&Brayns::Impl::_toggleShadows, this));
        _keyboardHandler.registerKeyboardShortcut(
            'U', "Enable/Disable soft shadows",
            std::bind(&Brayns::Impl::_toggleSoftShadows, this));
        _keyboardHandler.registerKeyboardShortcut(
            't', "Multiply samples per ray by 2",
            std::bind(&Brayns::Impl::_increaseSamplesPerRay, this));
        _keyboardHandler.registerKeyboardShortcut(
            'T', "Divide samples per ray by 2",
            std::bind(&Brayns::Impl::_decreaseSamplesPerRay, this));
        _keyboardHandler.registerKeyboardShortcut(
            'l', "Toggle load dynamic/static load balancer",
            std::bind(&Brayns::Impl::_toggleLoadBalancer, this));
        _keyboardHandler.registerKeyboardShortcut(
            'g', "Enable/Disable animation playback",
            std::bind(&Brayns::Impl::_toggleAnimationPlayback, this));
        _keyboardHandler.registerKeyboardShortcut(
            '{', "Decrease eye separation",
            std::bind(&Brayns::Impl::_changeEyeSeparation, this, -0.01));
        _keyboardHandler.registerKeyboardShortcut(
            '}', "Increase eye separation",
            std::bind(&Brayns::Impl::_changeEyeSeparation, this, 0.01));
        _keyboardHandler.registerKeyboardShortcut(
            '<', "Decrease field of view",
            std::bind(&Brayns::Impl::_changeFieldOfView, this, -1.));
        _keyboardHandler.registerKeyboardShortcut(
            '>', "Increase field of view",
            std::bind(&Brayns::Impl::_changeFieldOfView, this, 1.));
        _keyboardHandler.registerKeyboardShortcut(
            ' ', "Camera reset to initial state",
            std::bind(&Brayns::Impl::_resetCamera, this));
        _keyboardHandler.registerKeyboardShortcut(
            '+', "Increase motion speed",
            std::bind(&Brayns::Impl::_increaseMotionSpeed, this));
        _keyboardHandler.registerKeyboardShortcut(
            '-', "Decrease motion speed",
            std::bind(&Brayns::Impl::_decreaseMotionSpeed, this));
        _keyboardHandler.registerKeyboardShortcut(
            'c', "Display current camera information",
            std::bind(&Brayns::Impl::_displayCameraInformation, this));
        _keyboardHandler.registerKeyboardShortcut(
            'b', "Toggle benchmarking", [this]() {
                auto& ap = _parametersManager.getApplicationParameters();
                ap.setBenchmarking(!ap.isBenchmarking());
            });
    }

    void _blackBackground()
    {
        RenderingParameters& renderParams =
            _parametersManager.getRenderingParameters();
        renderParams.setBackgroundColor(Vector3f(0.f, 0.f, 0.f));
    }

    void _grayBackground()
    {
        RenderingParameters& renderParams =
            _parametersManager.getRenderingParameters();
        renderParams.setBackgroundColor(Vector3f(0.5f, 0.5f, 0.5f));
    }

    void _whiteBackground()
    {
        RenderingParameters& renderParams =
            _parametersManager.getRenderingParameters();
        renderParams.setBackgroundColor(Vector3f(1.f, 1.f, 1.f));
    }

    void _scivisRenderer()
    {
        RenderingParameters& renderParams =
            _parametersManager.getRenderingParameters();
        renderParams.setCurrentRenderer("scivis");
    }

    void _defaultRenderer()
    {
        RenderingParameters& renderParams =
            _parametersManager.getRenderingParameters();
        renderParams.setCurrentRenderer("basic");
    }

    void _basicSimulationRenderer()
    {
        RenderingParameters& renderParams =
            _parametersManager.getRenderingParameters();
        renderParams.setCurrentRenderer("basic_simulation");
    }

    void _proximityRenderer()
    {
        RenderingParameters& renderParams =
            _parametersManager.getRenderingParameters();
        renderParams.setCurrentRenderer("proximity");
    }

    void _advancedSimulationRenderer()
    {
        RenderingParameters& renderParams =
            _parametersManager.getRenderingParameters();
        renderParams.setCurrentRenderer("advanced_simulation");
    }

    void _increaseAnimationFrame()
    {
        _parametersManager.getAnimationParameters().jumpFrames(1);
    }

    void _decreaseAnimationFrame()
    {
        _parametersManager.getAnimationParameters().jumpFrames(-1);
    }

    void _diffuseShading()
    {
        _engine->getRenderer().updateProperty("shadingEnabled", true);
        _engine->getRenderer().updateProperty("electronShadingEnabled", false);
    }

    void _electronShading()
    {
        _engine->getRenderer().updateProperty("shadingEnabled", false);
        _engine->getRenderer().updateProperty("electronShadingEnabled", true);
    }

    void _disableShading()
    {
        _engine->getRenderer().updateProperty("shadingEnabled", false);
        _engine->getRenderer().updateProperty("electronShadingEnabled", false);
    }

    void _increaseAmbientOcclusionStrength()
    {
        auto& renderer = _engine->getRenderer();
        if (!renderer.hasProperty("aoWeight"))
            return;

        auto aoStrength = renderer.getProperty<double>("aoWeight");
        aoStrength += 0.1;
        if (aoStrength > 1.)
            aoStrength = 1.;
        renderer.updateProperty("aoWeight", aoStrength);
    }

    void _decreaseAmbientOcclusionStrength()
    {
        auto& renderer = _engine->getRenderer();
        if (!renderer.hasProperty("aoWeight"))
            return;

        auto aoStrength = renderer.getProperty<double>("aoWeight");
        aoStrength -= 0.1;
        if (aoStrength < 0.)
            aoStrength = 0.;
        renderer.updateProperty("aoWeight", aoStrength);
    }

    void _resetAnimationFrame()
    {
        auto& animParams = _parametersManager.getAnimationParameters();
        animParams.setFrame(0);
    }

    void _toggleShadows()
    {
        auto& renderer = _engine->getRenderer();
        if (!renderer.hasProperty("shadows"))
            return;

        renderer.updateProperty(
            "shadows", renderer.getProperty<double>("shadows") == 0. ? 1. : 0.);
    }

    void _toggleSoftShadows()
    {
        auto& renderer = _engine->getRenderer();
        if (!renderer.hasProperty("softShadows"))
            return;

        renderer.updateProperty("softShadows", renderer.getProperty<double>(
                                                   "softShadows") == 0.
                                                   ? 1.
                                                   : 0.);
    }

    void _increaseSamplesPerRay()
    {
        auto& vp = _parametersManager.getVolumeParameters();
        vp.setSamplingRate(vp.getSamplingRate() * 2);
    }

    void _decreaseSamplesPerRay()
    {
        auto& vp = _parametersManager.getVolumeParameters();
        vp.setSamplingRate(vp.getSamplingRate() / 2);
    }

    void _toggleLoadBalancer()
    {
        auto& appParams = _parametersManager.getApplicationParameters();
        appParams.setDynamicLoadBalancer(!appParams.getDynamicLoadBalancer());
    }

    void _changeFieldOfView(const double delta)
    {
        auto& camera = _engine->getCamera();
        if (!camera.hasProperty("fovy"))
            return;
        auto fovy = camera.getProperty<double>("fovy");
        fovy += delta;
        camera.updateProperty("fovy", fovy);
        BRAYNS_INFO << "Field of view: " << fovy << std::endl;
    }

    void _changeEyeSeparation(const double delta)
    {
        auto& camera = _engine->getCamera();
        if (!camera.hasProperty("interpupillaryDistance"))
            return;
        auto eyeSeparation =
            camera.getProperty<double>("interpupillaryDistance");
        eyeSeparation += delta;
        camera.updateProperty("interpupillaryDistance", eyeSeparation);
        BRAYNS_INFO << "Eye separation: " << eyeSeparation << std::endl;
    }

    void _gradientMaterials()
    {
        _engine->getScene().setMaterialsColorMap(MaterialsColorMap::gradient);
    }

    void _randomMaterials()
    {
        _engine->getScene().setMaterialsColorMap(MaterialsColorMap::random);
    }

    void _toggleAnimationPlayback()
    {
        _parametersManager.getAnimationParameters().togglePlayback();
    }

    void _resetCamera()
    {
        auto& camera = _engine->getCamera();
        camera.reset();
    }

    void _increaseMotionSpeed()
    {
        _cameraManipulator->updateMotionSpeed(DEFAULT_MOTION_ACCELERATION);
    }

    void _decreaseMotionSpeed()
    {
        _cameraManipulator->updateMotionSpeed(1.f /
                                              DEFAULT_MOTION_ACCELERATION);
    }

    void _displayCameraInformation()
    {
        BRAYNS_INFO << _engine->getCamera() << std::endl;
    }

    ParametersManager _parametersManager;
    EngineFactory _engineFactory;
    PluginManager _pluginManager;
    Engine* _engine{nullptr};
    KeyboardHandler _keyboardHandler;
    std::unique_ptr<AbstractManipulator> _cameraManipulator;
    std::vector<FrameBufferPtr> _frameBuffers;

    Timer _renderTimer;
    std::atomic<double> _lastFPS;

    std::shared_ptr<ActionInterface> _actionInterface;
    std::shared_ptr<DirectionalLight> _sunLight;
};

// -----------------------------------------------------------------------------

Brayns::Brayns(int argc, const char** argv)
    : _impl(std::make_unique<Impl>(argc, argv))
{
}
Brayns::~Brayns() = default;

void Brayns::commitAndRender(const RenderInput& renderInput,
                             RenderOutput& renderOutput)
{
    if (_impl->commit(renderInput))
    {
        _impl->render();
        _impl->postRender(&renderOutput);
    }
}

bool Brayns::commitAndRender()
{
    if (_impl->commit())
    {
        _impl->render();
        _impl->postRender(nullptr);
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
    _impl->postRender(nullptr);
}
Engine& Brayns::getEngine()
{
    return _impl->getEngine();
}
ParametersManager& Brayns::getParametersManager()
{
    return _impl->getParametersManager();
}

KeyboardHandler& Brayns::getKeyboardHandler()
{
    return _impl->getKeyboardHandler();
}

AbstractManipulator& Brayns::getCameraManipulator()
{
    return _impl->getCameraManipulator();
}
} // namespace brayns

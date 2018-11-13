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

#include <brayns/common/Timer.h>
#include <brayns/common/camera/Camera.h>
#include <brayns/common/camera/FlyingModeManipulator.h>
#include <brayns/common/camera/InspectCenterManipulator.h>
#include <brayns/common/engine/Engine.h>
#include <brayns/common/input/KeyboardHandler.h>
#include <brayns/common/light/DirectionalLight.h>
#include <brayns/common/log.h>
#include <brayns/common/renderer/FrameBuffer.h>
#include <brayns/common/renderer/Renderer.h>
#include <brayns/common/scene/Model.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/common/utils/DynamicLib.h>
#include <brayns/common/utils/Utils.h>

#include <brayns/parameters/ParametersManager.h>

#if BRAYNS_USE_LIBARCHIVE
#include <brayns/io/ArchiveLoader.h>
#endif
#include <brayns/io/MeshLoader.h>
#include <brayns/io/MolecularSystemReader.h>
#include <brayns/io/ProteinLoader.h>
#include <brayns/io/VolumeLoader.h>
#include <brayns/io/XYZBLoader.h>

#include <brayns/pluginapi/PluginAPI.h>
#include <brayns/tasks/AddModelTask.h>

#if BRAYNS_USE_ASSIMP
#include <brayns/io/MeshLoader.h>
#endif

#include <brayns/io/MolecularSystemReader.h>

#ifdef BRAYNS_USE_OSPRAY
#include <ospcommon/library.h>
#endif

namespace
{
const float DEFAULT_TEST_ANIMATION_FRAME = 10000;
const float DEFAULT_MOTION_ACCELERATION = 1.5f;
}

namespace brayns
{
struct Brayns::Impl : public PluginAPI
{
    Impl(int argc, const char** argv)
        : _pluginManager{argc, argv}
        , _engineFactory{argc, argv, _parametersManager}
    {
        BRAYNS_INFO << "Parsing command line options" << std::endl;
        _parametersManager.parse(argc, argv);
        _parametersManager.print();

        _createEngine();

        _pluginManager.initPlugins(this);

        _registerLoaders();
        _loadData();

        _engine->getScene().commit(); // Needed to obtain a bounding box
        _registerKeyboardShortcuts();
        _setupCameraManipulator(CameraMode::inspect);
        const auto frameSize = Vector2d(_engine->getFrameBuffer().getSize());
        _engine->getCamera().updateProperty("aspect",
                                            frameSize.x() / frameSize.y());
    }

    bool commit()
    {
        std::unique_lock<std::mutex> lock{_renderMutex, std::defer_lock};
        if (!lock.try_lock())
            return false;

        _pluginManager.preRender();

        auto& scene = _engine->getScene();
        scene.commit();

        _engine->getStatistics().setSceneSizeInBytes(scene.getSizeInBytes());

        _parametersManager.getAnimationParameters().update();

        auto& renderer = _engine->getRenderer();
        const auto& rp = _parametersManager.getRenderingParameters();
        renderer.setCurrentType(rp.getCurrentRenderer());

        const auto windowSize =
            _parametersManager.getApplicationParameters().getWindowSize();

        _engine->reshape(windowSize);
        _engine->preRender();

        auto& camera = _engine->getCamera();
        camera.commit();
        _engine->commit();

        if (rp.getHeadLight())
        {
            LightPtr sunLight = scene.getLight(0);
            auto sun = std::dynamic_pointer_cast<DirectionalLight>(sunLight);
            if (sun && (camera.isModified() || rp.isModified()))
            {
                sun->setDirection(camera.getOrientation().rotate(
                    Vector3f(0.0f, 0.0f, -1.0f)));
                scene.commitLights();
            }
        }

        if (_parametersManager.isAnyModified() || camera.isModified() ||
            scene.isModified() || renderer.isModified())
        {
            _engine->getFrameBuffer().clear();
        }

        _parametersManager.resetModified();
        camera.resetModified();
        scene.resetModified();
        renderer.resetModified();

        return true;
    }

    void render()
    {
        std::lock_guard<std::mutex> lock{_renderMutex};

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
        if (output)
            _updateRenderOutput(*output);

        _engine->getStatistics().setFPS(_lastFPS);

        _engine->postRender();

        _pluginManager.postRender();

        _engine->getFrameBuffer().resetModified();
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
        uint8_t* colorBuffer = frameBuffer.getColorBuffer();
        if (colorBuffer)
        {
            const size_t size =
                frameSize.x() * frameSize.y() * frameBuffer.getColorDepth();
            renderOutput.colorBuffer.assign(colorBuffer, colorBuffer + size);
            renderOutput.colorBufferFormat = frameBuffer.getFrameBufferFormat();
        }

        float* depthBuffer = frameBuffer.getDepthBuffer();
        if (depthBuffer)
        {
            const size_t size = frameSize.x() * frameSize.y();
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
        _engine.reset(); // Free resources before creating a new engine

        const auto& engineName =
            _parametersManager.getApplicationParameters().getEngine();
        _engine = _engineFactory.create(engineName);
        if (!_engine)
            throw std::runtime_error(
                "Unsupported engine: " +
                _parametersManager.getApplicationParameters().getEngineAsString(
                    engineName));

        // Default sun light
        DirectionalLightPtr sunLight(
            new DirectionalLight(DEFAULT_SUN_DIRECTION, DEFAULT_SUN_COLOR,
                                 DEFAULT_SUN_INTENSITY));
        _engine->getScene().addLight(sunLight);
        _engine->getScene().commitLights();
    }

    void _registerLoaders()
    {
        auto& registry = _engine->getScene().getLoaderRegistry();
        auto& scene = _engine->getScene();
        auto& geometryParameters = _parametersManager.getGeometryParameters();

        registry.registerLoader(
            std::make_unique<ProteinLoader>(scene, geometryParameters));
        registry.registerLoader(std::make_unique<VolumeLoader>(
            scene, _parametersManager.getVolumeParameters()));
        registry.registerLoader(std::make_unique<XYZBLoader>(scene));
#if BRAYNS_USE_ASSIMP
        registry.registerLoader(
            std::make_unique<MeshLoader>(scene, geometryParameters));
#endif
#if BRAYNS_USE_LIBARCHIVE
        registry.registerLoader(
            std::make_unique<ArchiveLoader>(scene, registry));
#endif
        registry.registerLoader(
            std::make_unique<MolecularSystemReader>(scene, geometryParameters));
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
                _engine->getScene().buildEnvironmentMap();
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

                scene.loadModel(path, NO_MATERIAL, {path, path}, {progress});
            }
        }

        auto& geometryParameters = _parametersManager.getGeometryParameters();
        if (!geometryParameters.getLoadCacheFile().empty())
            scene.loadFromCacheFile();

        scene.saveToCacheFile();
        scene.buildEnvironmentMap();
        scene.markModified();
    }

    void _setupCameraManipulator(const CameraMode mode)
    {
        _cameraManipulator.reset();

        switch (mode)
        {
        case CameraMode::flying:
            _cameraManipulator.reset(
                new FlyingModeManipulator(_engine->getCamera(),
                                          _keyboardHandler,
                                          _engine->getScene().getBounds()));
            break;
        case CameraMode::inspect:
            _cameraManipulator.reset(
                new InspectCenterManipulator(_engine->getCamera(),
                                             _keyboardHandler,
                                             _engine->getScene().getBounds()));
            break;
        };
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
            'x', "Set animation frame to " +
                     std::to_string(DEFAULT_TEST_ANIMATION_FRAME),
            std::bind(&Brayns::Impl::_defaultAnimationFrame, this));
        _keyboardHandler.registerKeyboardShortcut(
            '|', "Create cache file ",
            std::bind(&Brayns::Impl::_saveSceneToCacheFile, this));
        _keyboardHandler.registerKeyboardShortcut(
            '{', "Decrease eye separation",
            std::bind(&Brayns::Impl::_decreaseEyeSeparation, this));
        _keyboardHandler.registerKeyboardShortcut(
            '}', "Increase eye separation",
            std::bind(&Brayns::Impl::_increaseEyeSeparation, this));
        _keyboardHandler.registerKeyboardShortcut(
            '<', "Decrease field of view",
            std::bind(&Brayns::Impl::_decreaseFieldOfView, this));
        _keyboardHandler.registerKeyboardShortcut(
            '>', "Increase field of view",
            std::bind(&Brayns::Impl::_increaseFieldOfView, this));
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
            'm', "Toggle synchronous/asynchronous mode",
            std::bind(&Brayns::Impl::_toggleSynchronousMode, this));
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
        _engine->getRenderer().updateProperty("electronShading", false);
    }

    void _electronShading()
    {
        _engine->getRenderer().updateProperty("shadingEnabled", false);
        _engine->getRenderer().updateProperty("electronShading", true);
    }

    void _disableShading()
    {
        _engine->getRenderer().updateProperty("shadingEnabled", false);
        _engine->getRenderer().updateProperty("electronShading", false);
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

    void _decreaseFieldOfView()
    {
        _fieldOfView -= 1.;
        _engine->getCamera().updateProperty("fovy", _fieldOfView);
        BRAYNS_INFO << "Field of view: " << _fieldOfView << std::endl;
    }

    void _increaseFieldOfView()
    {
        _fieldOfView += 1.;
        _engine->getCamera().updateProperty("fovy", _fieldOfView);
        BRAYNS_INFO << "Field of view: " << _fieldOfView << std::endl;
    }

    void _decreaseEyeSeparation()
    {
        _eyeSeparation -= 0.01;
        _engine->getCamera().updateProperty("interpupillaryDistance",
                                            _eyeSeparation);
        BRAYNS_INFO << "Eye separation: " << _eyeSeparation << std::endl;
    }

    void _increaseEyeSeparation()
    {
        _eyeSeparation += 0.01;
        _engine->getCamera().updateProperty("interpupillaryDistance",
                                            _eyeSeparation);
        BRAYNS_INFO << "Eye separation: " << _eyeSeparation << std::endl;
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
        auto& animParams = _parametersManager.getAnimationParameters();
        animParams.setDelta(animParams.getDelta() == 0 ? 1 : 0);
    }

    void _defaultAnimationFrame()
    {
        auto& animParams = _parametersManager.getAnimationParameters();
        animParams.setFrame(DEFAULT_TEST_ANIMATION_FRAME);
    }

    void _saveSceneToCacheFile()
    {
        auto& scene = _engine->getScene();
        scene.saveToCacheFile();
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

    void _toggleSynchronousMode()
    {
        auto& app = _parametersManager.getApplicationParameters();
        app.setSynchronousMode(!app.getSynchronousMode());
    }

    ParametersManager _parametersManager;
    PluginManager _pluginManager;
    EngineFactory _engineFactory;
    EnginePtr _engine;
    KeyboardHandler _keyboardHandler;
    AbstractManipulatorPtr _cameraManipulator;

    double _fieldOfView{45.};
    double _eyeSeparation{0.0635};

    // protect render() vs commit() when doing all the commits
    std::mutex _renderMutex;

    Timer _renderTimer;
    std::atomic<double> _lastFPS;

    std::shared_ptr<ActionInterface> _actionInterface;
};

// -----------------------------------------------------------------------------

Brayns::Brayns(int argc, const char** argv)
    : _impl([argc, argv]() {
        BRAYNS_INFO << "     ____                             " << std::endl;
        BRAYNS_INFO << "    / __ )_________ ___  ______  _____" << std::endl;
        BRAYNS_INFO << "   / __  / ___/ __ `/ / / / __ \\/ ___/" << std::endl;
        BRAYNS_INFO << "  / /_/ / /  / /_/ / /_/ / / / (__  ) " << std::endl;
        BRAYNS_INFO << " /_____/_/   \\__,_/\\__, /_/ /_/____/  " << std::endl;
        BRAYNS_INFO << "                  /____/              " << std::endl;
        BRAYNS_INFO << std::endl;
        return std::make_unique<Impl>(argc, argv);
    }())
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
}

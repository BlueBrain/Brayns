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

#include <brayns/Brayns.h>

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
#include <brayns/common/scene/Scene.h>
#include <brayns/common/utils/Utils.h>
#include <brayns/common/volume/VolumeHandler.h>

#include <brayns/parameters/ParametersManager.h>

#include <brayns/io/MeshLoader.h>
#include <brayns/io/MolecularSystemReader.h>
#include <brayns/io/ProteinLoader.h>
#include <brayns/io/TransferFunctionLoader.h>
#include <brayns/io/XYZBLoader.h>
#include <brayns/io/simulation/SpikeSimulationHandler.h>

#include <brayns/pluginapi/ExtensionPluginFactory.h>
#include <brayns/pluginapi/PluginAPI.h>
#include <plugins/engines/EngineFactory.h>
#ifdef BRAYNS_USE_NETWORKING
#include <plugins/RocketsPlugin/RocketsPlugin.h>
#endif
#ifdef BRAYNS_USE_DEFLECT
#include <plugins/DeflectPlugin/DeflectPlugin.h>
#endif

#ifdef BRAYNS_USE_BRION
#include <brayns/io/ConnectivityLoader.h>
#include <brayns/io/MorphologyLoader.h>
#include <brayns/io/NESTLoader.h>
#include <brayns/io/SceneLoader.h>
#include <servus/uri.h>
#endif

#include <future>
#ifdef BRAYNS_USE_LUNCHBOX
#include <lunchbox/threadPool.h>
#endif

#ifdef BRAYNS_USE_OSPRAY
#include <ospcommon/library.h>
#endif

#include <boost/progress.hpp>

namespace
{
const float DEFAULT_TEST_ANIMATION_FRAME = 10000;
const float DEFAULT_MOTION_ACCELERATION = 1.5f;
const size_t LOADING_PROGRESS_DATA = 100;
const size_t LOADING_PROGRESS_STEP = 10;
}

namespace brayns
{
struct Brayns::Impl : public PluginAPI
{
    Impl(int argc, const char** argv)
        : _engineFactory{argc, argv, _parametersManager}
        , _meshLoader(_parametersManager.getGeometryParameters())
    {
        auto& types =
            _parametersManager.getGeometryParameters().getSupportedDataTypes();
        types = MeshLoader::getSupportedDataTypes();
        types.insert("xyz");

        BRAYNS_INFO << "     ____                             " << std::endl;
        BRAYNS_INFO << "    / __ )_________ ___  ______  _____" << std::endl;
        BRAYNS_INFO << "   / __  / ___/ __ `/ / / / __ \\/ ___/" << std::endl;
        BRAYNS_INFO << "  / /_/ / /  / /_/ / /_/ / / / (__  ) " << std::endl;
        BRAYNS_INFO << " /_____/_/   \\__,_/\\__, /_/ /_/____/  " << std::endl;
        BRAYNS_INFO << "                  /____/              " << std::endl;
        BRAYNS_INFO << std::endl;

        BRAYNS_INFO << "Parsing command line options" << std::endl;
        _parametersManager.parse(argc, argv);
        _parametersManager.print();

        _registerKeyboardShortcuts();

        createEngine();

        _finishLoadScene();
    }

    void addPlugins()
    {
#ifdef BRAYNS_USE_NETWORKING
        auto plugin{std::make_shared<RocketsPlugin>(_engine, this)};
        _extensionPluginFactory.add(plugin);
        _actionInterface = plugin;
#endif
#ifdef BRAYNS_USE_DEFLECT
        _extensionPluginFactory.add(
            std::make_shared<DeflectPlugin>(_engine, this));
#endif
    }

    void loadPlugins()
    {
#ifdef BRAYNS_USE_OSPRAY
        for (const auto& pluginName :
             _parametersManager.getApplicationParameters().getPlugins())
        {
            try
            {
                ospcommon::Library library(pluginName);
                auto createSym = library.getSymbol("brayns_plugin_create");
                if (!createSym)
                {
                    BRAYNS_ERROR << "Plugin '" << pluginName
                                 << "' is not a valid Brayns plugin; missing "
                                    "brayns_plugin_create()"
                                 << std::endl;
                    return;
                }

                ExtensionPlugin* (*createFunc)(PluginAPI*) =
                    (ExtensionPlugin * (*)(PluginAPI*))createSym;
                auto plugin = createFunc(this);

                _extensionPluginFactory.add(ExtensionPluginPtr{plugin});
                BRAYNS_INFO << "Loaded plugin '" << pluginName << "'"
                            << std::endl;
            }
            catch (const std::runtime_error& exc)
            {
                BRAYNS_ERROR << exc.what() << std::endl;
            }
        }
#endif
    }

    bool preRender()
    {
        if (!isLoadingFinished())
        {
#ifdef BRAYNS_USE_LUNCHBOX
            if (isAsyncMode())
            {
                postSceneLoading();
                return false;
            }
#endif
        }

        _extensionPluginFactory.preRender();

        std::unique_lock<std::mutex> lock{_renderMutex, std::defer_lock};
        if (!lock.try_lock())
            return false;

        std::shared_lock<std::shared_timed_mutex> dataLock{_engine->dataMutex(),
                                                           std::defer_lock};
        if (!dataLock.try_lock())
            return false;

        Scene& scene = _engine->getScene();

        // consider updates on the scene also from plugins, not only 'our own'
        // built-in data loading
        if (scene.isModified())
            _finishLoadScene();

        _updateAnimation();

        _engine->setActiveRenderer(
            _parametersManager.getRenderingParameters().getRenderer());

        const Vector2ui windowSize =
            _parametersManager.getApplicationParameters().getWindowSize();

        _engine->reshape(windowSize);
        _engine->preRender();

        _engine->commit();

        Camera& camera = _engine->getCamera();
        camera.commit();

        if (scene.getTransferFunction().isModified())
            scene.commitTransferFunctionData();

        if (_parametersManager.getRenderingParameters().getHeadLight())
        {
            LightPtr sunLight = scene.getLight(0);
            auto sun = std::dynamic_pointer_cast<DirectionalLight>(sunLight);
            if (sun &&
                (camera.isModified() ||
                 _parametersManager.getRenderingParameters().isModified()))
            {
                sun->setDirection(camera.getTarget() - camera.getPosition());
                scene.commitLights();
            }
        }

        if (_parametersManager.isAnyModified() || camera.isModified() ||
            scene.isModified())
        {
            _engine->getFrameBuffer().clear();
        }

        _engine->getScene().getTransferFunction().resetModified();
        _parametersManager.resetModified();
        _engine->getCamera().resetModified();
        _engine->getScene().resetModified();

        return true;
    }

    void postRender()
    {
        _engine->postRender();

        _fpsUpdateElapsed += _renderTimer.milliseconds();
        if (_fpsUpdateElapsed > 750)
        {
            _engine->getStatistics().setFPS(_renderTimer.perSecondSmoothed());
            _fpsUpdateElapsed = 0;
        }

        // WAR to keep clients updated about current animation frame
        auto& ap = _parametersManager.getAnimationParameters();
        if (ap.getDelta() != 0)
            ap.markModified();

        _extensionPluginFactory.postRender();

        ap.resetModified();
        _engine->getFrameBuffer().resetModified();
        _engine->getStatistics().resetModified();
    }

    void postSceneLoading()
    {
        _extensionPluginFactory.postSceneLoading();
        _engine->getStatistics().resetModified();
    }

    void createEngine()
    {
        _engine.reset(); // Free resources before creating a new engine

        const auto& engineName =
            _parametersManager.getRenderingParameters().getEngine();
        _engine = _engineFactory.create(engineName);
        if (!_engine)
            throw std::runtime_error(
                "Unsupported engine: " +
                _parametersManager.getRenderingParameters().getEngineAsString(
                    engineName));

        _setupCameraManipulator(CameraMode::inspect);

        // Default sun light
        DirectionalLightPtr sunLight(
            new DirectionalLight(DEFAULT_SUN_DIRECTION, DEFAULT_SUN_COLOR,
                                 DEFAULT_SUN_INTENSITY));
        _engine->getScene().addLight(sunLight);
        _engine->getScene().commitLights();

        buildScene();
    }

    void buildScene()
    {
        if (!isLoadingFinished())
            throw std::runtime_error("Build scene already in progress");

        std::promise<void> promise;
        _dataLoadingFuture = promise.get_future();

#ifdef BRAYNS_USE_LUNCHBOX
        if (isAsyncMode())
            _loadingThread.post(std::bind(&Brayns::Impl::_loadScene, this))
                .get();
        else
#endif
            _loadScene();

        promise.set_value();

        const auto frameSize = Vector2f(_engine->getFrameBuffer().getSize());

        auto& camera = _engine->getCamera();
        camera.setInitialState(_engine->getScene().getWorldBounds());
        camera.setAspectRatio(frameSize.x() / frameSize.y());
    }

    bool preRender(const RenderInput& renderInput)
    {
        _engine->getCamera().set(renderInput.position, renderInput.target,
                                 renderInput.up);
        _parametersManager.getApplicationParameters().setWindowSize(
            renderInput.windowSize);

        return preRender();
    }

    void postRender(RenderOutput& renderOutput)
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
        frameBuffer.unmap();

        postRender();
    }

    void render()
    {
        std::lock_guard<std::mutex> lock{_renderMutex};
        std::shared_lock<std::shared_timed_mutex> dataLock{
            _engine->dataMutex()};

        _renderTimer.start();
        _engine->render();
        _renderTimer.stop();
    }

    Engine& getEngine() { return *_engine; }
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
    ActionInterface* getActionInterface() final
    {
        return _actionInterface.get();
    }
    Scene& getScene() final { return _engine->getScene(); }
    bool isLoadingFinished() const
    {
        return !_dataLoadingFuture.valid() ||
               _dataLoadingFuture.wait_for(std::chrono::milliseconds(0)) ==
                   std::future_status::ready;
    }
    bool isAsyncMode() const
    {
        return !_parametersManager.getApplicationParameters()
                    .getSynchronousMode();
    }

private:
    void _loadScene()
    {
        // fix race condition: we have to wait until rendering is finished
        std::lock_guard<std::shared_timed_mutex> lock{_engine->dataMutex()};

        boost::progress_display loadingProgress(
            LOADING_PROGRESS_DATA + 3 * LOADING_PROGRESS_STEP, std::cout,
            "[INFO ] Loading scene ...\n[INFO ] ", "[INFO ] ", "[INFO ] ");

        Scene& scene = _engine->getScene();

        scene.unload();
        loadingProgress += LOADING_PROGRESS_STEP;

        _meshLoader.clear();

        scene.resetMaterials();
        const bool success = _loadData(loadingProgress);

        if (!success || (scene.empty() && !scene.getVolumeHandler()))
        {
            scene.unload();
            _meshLoader.clear();
            BRAYNS_INFO << "Building default scene" << std::endl;
            scene.buildDefault();
        }

        scene.buildEnvironment();

        const auto& geomParams = _parametersManager.getGeometryParameters();
        scene.buildGeometry();
        if (geomParams.getLoadCacheFile().empty() &&
            !geomParams.getSaveCacheFile().empty())
        {
            scene.saveToCacheFile();
        }

        loadingProgress += LOADING_PROGRESS_STEP;

        scene.commit();
        loadingProgress += LOADING_PROGRESS_STEP;

        BRAYNS_INFO << "Now rendering ..." << std::endl;
    }

    // do this in the main thread again to avoid race conditions
    void _finishLoadScene()
    {
        if (_dataLoadingFuture.valid())
            _dataLoadingFuture.get();

        // Set default epsilon according to scene bounding box
        _engine->setDefaultEpsilon();

        _engine->getStatistics().setSceneSizeInBytes(
            _engine->getScene().getSizeInBytes());

        // finish reporting of progress
        postSceneLoading();
    }

    void _updateAnimation()
    {
        if (!isLoadingFinished())
            return;

        auto simHandler = _engine->getScene().getSimulationHandler();
        auto& animParams = _parametersManager.getAnimationParameters();
        if ((animParams.isModified() || animParams.getDelta() != 0) &&
            simHandler && simHandler->isReady())
        {
            animParams.setFrame(animParams.getFrame() + animParams.getDelta());
        }
    }

    bool _loadData(boost::progress_display& loadingProgress)
    {
        size_t nextTic = 0;
        const size_t tic = LOADING_PROGRESS_DATA;
        auto updateProgress = [&nextTic,
                               &loadingProgress](const std::string&,
                                                 const float progress) {
#pragma omp critical
            {
                const size_t newProgress = progress * tic;
                if (newProgress % tic > nextTic)
                {
                    loadingProgress += newProgress - nextTic;
                    nextTic = newProgress;
                }
            }
        };

        auto& geometryParameters = _parametersManager.getGeometryParameters();
        auto& volumeParameters = _parametersManager.getVolumeParameters();
        auto& sceneParameters = _parametersManager.getSceneParameters();
        auto& scene = _engine->getScene();

        // set environment map if applicable
        const std::string& environmentMap =
            _parametersManager.getSceneParameters().getEnvironmentMap();
        if (!environmentMap.empty())
        {
            const size_t materialId = static_cast<size_t>(MaterialType::skybox);
            auto& material = scene.getMaterials()[materialId];
            material.setTexture(TT_DIFFUSE, environmentMap);
            material.setType(MaterialType::skybox);
        }

        const std::string& colorMapFilename =
            sceneParameters.getColorMapFilename();
        if (!colorMapFilename.empty())
        {
            TransferFunctionLoader transferFunctionLoader;
            transferFunctionLoader.loadFromFile(
                colorMapFilename, sceneParameters.getColorMapRange(), scene);
        }

        if (!geometryParameters.getLoadCacheFile().empty())
        {
            scene.loadFromCacheFile();
            loadingProgress += tic;
        }

        if (!geometryParameters.getPDBFile().empty())
        {
            _loadPDBFile(geometryParameters.getPDBFile());
            loadingProgress += tic;
        }

        if (!geometryParameters.getPDBFolder().empty())
            _loadPDBFolder(updateProgress);

        if (!geometryParameters.getSplashSceneFolder().empty())
            _loadMeshFolder(geometryParameters.getSplashSceneFolder(),
                            updateProgress);

        if (!geometryParameters.getMeshFolder().empty())
            _loadMeshFolder(geometryParameters.getMeshFolder(), updateProgress);

        if (!geometryParameters.getMeshFile().empty())
        {
            _loadMeshFile(geometryParameters.getMeshFile());
            loadingProgress += tic;
        }

#if (BRAYNS_USE_BRION)
        if (!geometryParameters.getSceneFile().empty())
            _loadSceneFile(geometryParameters.getSceneFile(), updateProgress);

        if (!geometryParameters.getNESTCircuit().empty())
        {
            _loadNESTCircuit();
            loadingProgress += tic;
        }

        if (!geometryParameters.getMorphologyFolder().empty())
            _loadMorphologyFolder(updateProgress);

        if (!geometryParameters.getCircuitConfiguration().empty() &&
            geometryParameters.getConnectivityFile().empty())
            _loadCircuitConfiguration(updateProgress);

        if (!geometryParameters.getConnectivityFile().empty())
            _loadConnectivityFile();
#endif

        if (!geometryParameters.getXYZBFile().empty())
        {
            _loadXYZBFile(updateProgress);
            loadingProgress += tic;
        }

        if (!geometryParameters.getMolecularSystemConfig().empty())
            _loadMolecularSystem(updateProgress);

        if (scene.getVolumeHandler())
        {
            scene.commitTransferFunctionData();
            scene.getVolumeHandler()->setCurrentIndex(0);
            const Vector3ui& volumeDimensions =
                scene.getVolumeHandler()->getDimensions();
            const Vector3f& volumeOffset =
                scene.getVolumeHandler()->getOffset();
            const Vector3f& volumeElementSpacing =
                volumeParameters.getElementSpacing();
            Boxf& worldBounds = scene.getWorldBounds();
            worldBounds.merge(Vector3f(0.f, 0.f, 0.f));
            worldBounds.merge(volumeOffset +
                              Vector3f(volumeDimensions) *
                                  volumeElementSpacing);
        }
        return true;
    }

    /**
        Loads data from a PDB file (command line parameter --pdb-file)
    */
    void _loadPDBFolder(const ProgressReporter::UpdateCallback& progressUpdate)
    {
        // Load PDB File
        auto& geometryParameters = _parametersManager.getGeometryParameters();
        const std::string& folder = geometryParameters.getPDBFolder();
        const strings filters = {".pdb", ".pdb1"};
        const strings files = parseFolder(folder, filters);

        size_t current = 0;
        for (const auto& file : files)
        {
            _loadPDBFile(file);
            ++current;
            progressUpdate("Loading PDB folder " + folder,
                           float(current) / files.size());
        }
    }

    /**
        Loads data from a PDB file (command line parameter --pdb-file)
    */
    void _loadPDBFile(const std::string& filename)
    {
        // Load PDB File
        auto& geometryParameters = _parametersManager.getGeometryParameters();
        auto& scene = _engine->getScene();
        std::string pdbFile = filename;
        if (pdbFile == "")
        {
            pdbFile = geometryParameters.getPDBFile();
            BRAYNS_INFO << "Loading PDB file " << pdbFile << std::endl;
        }
        ProteinLoader proteinLoader(geometryParameters);
        if (!proteinLoader.importPDBFile(pdbFile, Vector3f(0, 0, 0), 0, scene))
            BRAYNS_ERROR << "Failed to import " << pdbFile << std::endl;

        for (size_t i = 0; i < scene.getMaterials().size(); ++i)
        {
            auto& material = scene.getMaterials()[i];
            material.setColor(proteinLoader.getMaterialKd(i));
        }
    }

    /**
        Loads data from a XYZR file (command line parameter --xyzr-file)
    */
    void _loadXYZBFile(const ProgressReporter::UpdateCallback& progressUpdate)
    {
        // Load XYZB File
        auto& geometryParameters = _parametersManager.getGeometryParameters();
        auto& scene = _engine->getScene();
        BRAYNS_INFO << "Loading XYZB file " << geometryParameters.getXYZBFile()
                    << std::endl;
        XYZBLoader xyzbLoader(geometryParameters);
        xyzbLoader.setProgressCallback(progressUpdate);
        try
        {
            xyzbLoader.importFromFile(geometryParameters.getXYZBFile(), scene);
        }
        catch (const std::runtime_error& e)
        {
            BRAYNS_ERROR << "Failed to import "
                         << geometryParameters.getXYZBFile() << ": " << e.what()
                         << std::endl;
        }
    }

    /**
        Loads data from mesh files located in the folder specified in
       the geometry parameters (command line parameter --mesh-folder)
    */
    void _loadMeshFolder(const std::string& folder,
                         const ProgressReporter::UpdateCallback& progressUpdate)
    {
        const auto& geometryParameters =
            _parametersManager.getGeometryParameters();
        auto& scene = _engine->getScene();

        strings filters = {".obj", ".dae", ".fbx", ".ply", ".lwo",
                           ".stl", ".3ds", ".ase", ".ifc", ".off"};
        strings files = parseFolder(folder, filters);
        size_t i = 0;
        std::stringstream msg;
        msg << "Loading " << files.size() << " meshes from " << folder;
        for (const auto& file : files)
        {
            size_t material =
                geometryParameters.getColorScheme() == ColorScheme::neuron_by_id
                    ? NB_SYSTEM_MATERIALS + i
                    : NO_MATERIAL;

            if (!_meshLoader.importMeshFromFile(file, scene, Matrix4f(),
                                                material))
                BRAYNS_ERROR << "Failed to import " << file << std::endl;
            ++i;
            progressUpdate(msg.str(), float(i) / files.size());
        }
    }

    /**
        Loads data from mesh file (command line parameter --mesh-file)
    */
    void _loadMeshFile(const std::string& filename)
    {
        const auto& geometryParameters =
            _parametersManager.getGeometryParameters();
        auto& scene = _engine->getScene();
        size_t material =
            geometryParameters.getColorScheme() == ColorScheme::neuron_by_id
                ? NB_SYSTEM_MATERIALS
                : NO_MATERIAL;

        if (!_meshLoader.importMeshFromFile(filename, scene, Matrix4f(),
                                            material))
            BRAYNS_ERROR << "Failed to import " << filename << std::endl;
    }

#if (BRAYNS_USE_BRION)
    /**
        Loads data from a neuron connectivity file (command line parameter
       --connectivity-file)
    */
    void _loadConnectivityFile()
    {
        // Load Connectivity File
        GeometryParameters& geometryParameters =
            _parametersManager.getGeometryParameters();
        ConnectivityLoader connectivityLoader(geometryParameters);
        if (!connectivityLoader.importFromFile(_engine->getScene(),
                                               _meshLoader))
            BRAYNS_ERROR << "Failed to import "
                         << geometryParameters.getConnectivityFile()
                         << std::endl;
    }

    /**
     * Loads data from a scene description file (command line parameter
     * --scene-file)
     */
    void _loadSceneFile(const std::string& filename,
                        const ProgressReporter::UpdateCallback& progressUpdate)
    {
        auto& applicationParameters =
            _parametersManager.getApplicationParameters();
        auto& geometryParameters = _parametersManager.getGeometryParameters();
        auto& scene = _engine->getScene();
        SceneLoader sceneLoader(applicationParameters, geometryParameters);
        sceneLoader.setProgressCallback(progressUpdate);
        sceneLoader.importFromFile(filename, scene, _meshLoader);
    }

    /**
     * Loads data from a NEST circuit file (command line parameter
     * --nest-circuit)
     */
    void _loadNESTCircuit()
    {
        auto& geometryParameters = _parametersManager.getGeometryParameters();
        auto& scene = _engine->getScene();

        const std::string& circuit(geometryParameters.getNESTCircuit());
        if (!circuit.empty())
        {
            NESTLoader loader(geometryParameters);

            // need to import circuit first to determine _frameSize for report
            // loading
            loader.importCircuit(circuit, scene);

            const std::string& cacheFile(geometryParameters.getNESTCacheFile());
            if (!geometryParameters.getNESTReport().empty() &&
                cacheFile.empty())
                throw std::runtime_error(
                    "Need cache file to visualize simulation data");

            if (!cacheFile.empty())
            {
                SpikeSimulationHandlerPtr simulationHandler(
                    new SpikeSimulationHandler(
                        _parametersManager.getGeometryParameters()));
                if (!simulationHandler->attachSimulationToCacheFile(cacheFile))
                {
                    if (!loader.importSpikeReport(
                            geometryParameters.getNESTReport()))
                    {
                        throw std::runtime_error(
                            "Could not load spike report, aborting");
                    }

                    if (!simulationHandler->attachSimulationToCacheFile(
                            cacheFile))
                    {
                        throw std::runtime_error(
                            "Could load cache file, aborting");
                    }
                }

                scene.setSimulationHandler(simulationHandler);
            }
        }
    }

    /**
        Loads data from SWC and H5 files located in the folder specified
       in the geometry parameters (command line parameter --morphology-folder)
    */
    void _loadMorphologyFolder(
        const ProgressReporter::UpdateCallback& progressUpdate)
    {
        auto& applicationParameters =
            _parametersManager.getApplicationParameters();
        auto& geometryParameters = _parametersManager.getGeometryParameters();
        auto& scene = _engine->getScene();
        const auto& folder = geometryParameters.getMorphologyFolder();
        MorphologyLoader morphologyLoader(applicationParameters,
                                          geometryParameters, scene);

        const strings filters = {".swc", ".h5"};
        const strings files = parseFolder(folder, filters);
        uint64_t morphologyIndex = 0;
        for (const auto& file : files)
        {
            servus::URI uri(file);
            if (!morphologyLoader.importMorphology(uri, morphologyIndex,
                                                   NO_MATERIAL))
                BRAYNS_ERROR << "Failed to import " << file << std::endl;
            ++morphologyIndex;
            progressUpdate("Loading morphologies from " + folder,
                           float(morphologyIndex) / files.size());
        }
    }

    /**
        Loads morphologies from circuit configuration (command line
       parameter --circuit-configuration)
    */
    void _loadCircuitConfiguration(
        const ProgressReporter::UpdateCallback& progressUpdate)
    {
        auto& applicationParameters =
            _parametersManager.getApplicationParameters();
        auto& geometryParameters = _parametersManager.getGeometryParameters();
        auto& scene = _engine->getScene();
        const std::string& filename =
            geometryParameters.getCircuitConfiguration();
        const auto& targets = geometryParameters.getCircuitTargetsAsStrings();

        BRAYNS_INFO << "Loading circuit configuration from " << filename
                    << std::endl;
        const std::string& report = geometryParameters.getCircuitReport();
        MorphologyLoader morphologyLoader(applicationParameters,
                                          geometryParameters, scene);
        morphologyLoader.setProgressCallback(progressUpdate);

        const servus::URI uri(filename);
        morphologyLoader.importCircuit(uri, targets, report, scene,
                                       _meshLoader);
    }
#endif // BRAYNS_USE_BRION

    /**
        Loads molecular system from configuration (command line
       parameter
        --molecular-system-config )
    */
    void _loadMolecularSystem(
        const ProgressReporter::UpdateCallback& progressUpdate)
    {
        auto& geometryParameters = _parametersManager.getGeometryParameters();
        auto& scene = _engine->getScene();
        MolecularSystemReader molecularSystemReader(geometryParameters);
        molecularSystemReader.setProgressCallback(progressUpdate);
        molecularSystemReader.import(scene, _meshLoader);
    }

    void _setupCameraManipulator(const CameraMode mode)
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
            '4', "Set pastel materials",
            std::bind(&Brayns::Impl::_pastelMaterials, this));
        _keyboardHandler.registerKeyboardShortcut(
            '5', "Scientific visualization renderer",
            std::bind(&Brayns::Impl::_scivisRenderer, this));
        _keyboardHandler.registerKeyboardShortcut(
            '6', "Default renderer",
            std::bind(&Brayns::Impl::_defaultRenderer, this));
        _keyboardHandler.registerKeyboardShortcut(
            '7', "Particle renderer",
            std::bind(&Brayns::Impl::_particleRenderer, this));
        _keyboardHandler.registerKeyboardShortcut(
            '8', "Proximity renderer",
            std::bind(&Brayns::Impl::_proximityRenderer, this));
        _keyboardHandler.registerKeyboardShortcut(
            '9', "Simulation renderer",
            std::bind(&Brayns::Impl::_simulationRenderer, this));
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
            'y', "Enable/Disable light emitting materials",
            std::bind(&Brayns::Impl::_toggleLightEmittingMaterials, this));
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
        renderParams.setRenderer(RendererType::scientificvisualization);
    }

    void _defaultRenderer()
    {
        RenderingParameters& renderParams =
            _parametersManager.getRenderingParameters();
        renderParams.setRenderer(RendererType::default_);
    }

    void _particleRenderer()
    {
        RenderingParameters& renderParams =
            _parametersManager.getRenderingParameters();
        renderParams.setRenderer(RendererType::particle);
    }

    void _proximityRenderer()
    {
        RenderingParameters& renderParams =
            _parametersManager.getRenderingParameters();
        renderParams.setRenderer(RendererType::proximity);
    }

    void _simulationRenderer()
    {
        RenderingParameters& renderParams =
            _parametersManager.getRenderingParameters();
        renderParams.setRenderer(RendererType::simulation);
    }

    void _increaseAnimationFrame()
    {
        if (_engine->getScene().getSimulationHandler() &&
            !_engine->getScene().getSimulationHandler()->isReady())
        {
            return;
        }

        auto& animParams = _parametersManager.getAnimationParameters();
        const auto animationFrame = animParams.getFrame();
        animParams.setFrame(animationFrame + 1);
    }

    void _decreaseAnimationFrame()
    {
        if (_engine->getScene().getSimulationHandler() &&
            !_engine->getScene().getSimulationHandler()->isReady())
        {
            return;
        }

        auto& animParams = _parametersManager.getAnimationParameters();
        const auto animationFrame = animParams.getFrame();
        if (animationFrame > 0)
            animParams.setFrame(animationFrame - 1);
    }

    void _diffuseShading()
    {
        RenderingParameters& renderParams =
            _parametersManager.getRenderingParameters();
        renderParams.setShading(ShadingType::diffuse);
    }

    void _electronShading()
    {
        RenderingParameters& renderParams =
            _parametersManager.getRenderingParameters();
        renderParams.setShading(ShadingType::electron);
    }

    void _disableShading()
    {
        RenderingParameters& renderParams =
            _parametersManager.getRenderingParameters();
        renderParams.setShading(ShadingType::none);
    }

    void _increaseAmbientOcclusionStrength()
    {
        RenderingParameters& renderParams =
            _parametersManager.getRenderingParameters();
        float aaStrength = renderParams.getAmbientOcclusionStrength();
        aaStrength += 0.1f;
        if (aaStrength > 1.f)
            aaStrength = 1.f;
        renderParams.setAmbientOcclusionStrength(aaStrength);
    }

    void _decreaseAmbientOcclusionStrength()
    {
        RenderingParameters& renderParams =
            _parametersManager.getRenderingParameters();
        float aaStrength = renderParams.getAmbientOcclusionStrength();
        aaStrength -= 0.1f;
        if (aaStrength < 0.f)
            aaStrength = 0.f;
        renderParams.setAmbientOcclusionStrength(aaStrength);
    }

    void _resetAnimationFrame()
    {
        auto& animParams = _parametersManager.getAnimationParameters();
        animParams.setFrame(0);
    }

    void _toggleShadows()
    {
        RenderingParameters& renderParams =
            _parametersManager.getRenderingParameters();
        renderParams.setShadows(renderParams.getShadows() == 0.f ? 1.f : 0.f);
    }

    void _toggleSoftShadows()
    {
        RenderingParameters& renderParams =
            _parametersManager.getRenderingParameters();
        renderParams.setSoftShadows(renderParams.getSoftShadows() == 0.f ? 0.1f
                                                                         : 0.f);
    }

    void _increaseSamplesPerRay()
    {
        RenderingParameters& renderParams =
            _parametersManager.getRenderingParameters();
        renderParams.setSamplesPerRay(renderParams.getSamplesPerRay() * 2);
    }

    void _decreaseSamplesPerRay()
    {
        RenderingParameters& renderParams =
            _parametersManager.getRenderingParameters();
        if (renderParams.getSamplesPerRay() >= 4)
            renderParams.setSamplesPerRay(renderParams.getSamplesPerRay() / 2);
    }

    void _toggleLightEmittingMaterials()
    {
        RenderingParameters& renderParams =
            _parametersManager.getRenderingParameters();
        renderParams.setLightEmittingMaterials(
            !renderParams.getLightEmittingMaterials());
    }

    void _toggleLoadBalancer()
    {
        RenderingParameters& renderParams =
            _parametersManager.getRenderingParameters();
        renderParams.setDynamicLoadBalancer(
            !renderParams.getDynamicLoadBalancer());
    }

    void _decreaseFieldOfView()
    {
        _fieldOfView -= 1.f;
        //_fieldOfView = std::max(1.f, _fieldOfView);
        _engine->getCamera().setFieldOfView(_fieldOfView);
        BRAYNS_INFO << "Field of view: " << _fieldOfView << std::endl;
    }

    void _increaseFieldOfView()
    {
        _fieldOfView += 1.f;
        //    _fieldOfView = std::min(179.f, _fieldOfView);
        _engine->getCamera().setFieldOfView(_fieldOfView);
        BRAYNS_INFO << "Field of view: " << _fieldOfView << std::endl;
    }

    void _decreaseEyeSeparation()
    {
        _eyeSeparation -= 0.01f;
        //_eyeSeparation = std::max(0.1f, _eyeSeparation);
        _engine->getCamera().setEyeSeparation(_eyeSeparation);
        BRAYNS_INFO << "Eye separation: " << _eyeSeparation << std::endl;
    }

    void _increaseEyeSeparation()
    {
        _eyeSeparation += 0.01f;
        //_eyeSeparation = std::min(1.0f, _eyeSeparation);
        _engine->getCamera().setEyeSeparation(_eyeSeparation);
        BRAYNS_INFO << "Eye separation: " << _eyeSeparation << std::endl;
    }

    void _gradientMaterials()
    {
        _engine->initializeMaterials(MaterialsColorMap::gradient);
    }

    void _pastelMaterials()
    {
        _engine->initializeMaterials(MaterialsColorMap::pastel);
    }

    void _randomMaterials()
    {
        _engine->initializeMaterials(MaterialsColorMap::random);
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
    EngineFactory _engineFactory;
    EnginePtr _engine;
    KeyboardHandler _keyboardHandler;
    AbstractManipulatorPtr _cameraManipulator;
    MeshLoader _meshLoader;

    float _fieldOfView{45.f};
    float _eyeSeparation{0.0635f};

    std::future<void> _dataLoadingFuture;

    // protect render() vs preRender() when doing all the commit()
    std::mutex _renderMutex;

    Timer _renderTimer;
    int64_t _fpsUpdateElapsed{0};

#ifdef BRAYNS_USE_LUNCHBOX
    // it is important to perform loading and unloading in the same thread,
    // otherwise we leak memory from within ospray/embree. So we don't use
    // std::async(std::launch::async), but rather a thread pool with one thread
    // that remains for the entire application lifetime.
    lunchbox::ThreadPool _loadingThread{1};
#endif

    ExtensionPluginFactory _extensionPluginFactory;
    std::shared_ptr<ActionInterface> _actionInterface;
};

// -------------------------------------------------------------------------------------------------

Brayns::Brayns(int argc, const char** argv)
    : _impl(new Impl(argc, argv))
{
}

Brayns::~Brayns()
{
}

void Brayns::render(const RenderInput& renderInput, RenderOutput& renderOutput)
{
    if (_impl->preRender(renderInput))
    {
        _impl->render();
        _impl->postRender(renderOutput);
    }
}

bool Brayns::render()
{
    if (_impl->preRender())
    {
        _impl->render();
        _impl->postRender();
    }
    return _impl->getEngine().getKeepRunning();
}

void Brayns::loadPlugins()
{
    _impl->addPlugins();
    _impl->loadPlugins();
}

bool Brayns::preRender()
{
    return _impl->preRender();
}

void Brayns::renderOnly()
{
    return _impl->render();
}

void Brayns::postRender()
{
    _impl->postRender();
}

void Brayns::buildScene()
{
    _impl->buildScene();
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

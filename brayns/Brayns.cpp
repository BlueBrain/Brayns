/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
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

#include <brayns/common/camera/Camera.h>
#include <brayns/common/camera/FlyingModeManipulator.h>
#include <brayns/common/camera/InspectCenterManipulator.h>
#include <brayns/common/engine/Engine.h>
#include <brayns/common/input/KeyboardHandler.h>
#include <brayns/common/light/DirectionalLight.h>
#include <brayns/common/log.h>
#include <brayns/common/renderer/FrameBuffer.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/common/utils/Utils.h>
#include <brayns/common/volume/VolumeHandler.h>

#include <brayns/parameters/ParametersManager.h>

#include <brayns/io/ProteinLoader.h>
#include <brayns/io/TransferFunctionLoader.h>
#include <brayns/io/XYZBLoader.h>
#include <brayns/io/simulation/SpikeSimulationHandler.h>
#if (BRAYNS_USE_ASSIMP)
#include <brayns/io/MeshLoader.h>
#include <brayns/io/MolecularSystemReader.h>
#endif

#if (BRAYNS_USE_MAGICKPP)
#include <Magick++.h>
#endif

#include <plugins/engines/EngineFactory.h>
#if (BRAYNS_USE_DEFLECT || BRAYNS_USE_NETWORKING)
#include <plugins/extensions/ExtensionPluginFactory.h>
#endif

#if (BRAYNS_USE_BRION)
#include <brayns/io/MorphologyLoader.h>
#include <brayns/io/NESTLoader.h>
#include <brayns/io/SceneLoader.h>
#include <servus/uri.h>
#endif

namespace
{
const float DEFAULT_TEST_TIMESTAMP = 10000.f;
const float DEFAULT_MOTION_ACCELERATION = 1.5f;
}

namespace brayns
{
struct Brayns::Impl
{
    Impl(int argc, const char** argv)
        : _engine(nullptr)
    {
        BRAYNS_INFO << "     ____                             " << std::endl;
        BRAYNS_INFO << "    / __ )_________ ___  ______  _____" << std::endl;
        BRAYNS_INFO << "   / __  / ___/ __ `/ / / / __ \\/ ___/" << std::endl;
        BRAYNS_INFO << "  / /_/ / /  / /_/ / /_/ / / / (__  ) " << std::endl;
        BRAYNS_INFO << " /_____/_/   \\__,_/\\__, /_/ /_/____/  " << std::endl;
        BRAYNS_INFO << "                  /____/              " << std::endl;
        BRAYNS_INFO << std::endl;
        BRAYNS_INFO << "Parsing command line options" << std::endl;
        _parametersManager.reset(new ParametersManager());
        _parametersManager->parse(argc, argv);
        _parametersManager->print();

        // Initialize keyboard handler
        _keyboardHandler.reset(new KeyboardHandler());
        _registerKeyboardShortcuts();

        // Get rendering engine
        _engineFactory.reset(
            new EngineFactory(argc, argv, *_parametersManager));
        createEngine();

#if (BRAYNS_USE_DEFLECT || BRAYNS_USE_NETWORKING)
        _extensionPluginFactory.reset(
            new ExtensionPluginFactory(*_parametersManager));
#endif
    }

    void createEngine()
    {
        if (_engine)
        {
            _engineFactory->remove(_engine);
            _engine.reset();
        }

        const auto& engineName =
            _parametersManager->getRenderingParameters().getEngine();
        _engine = _engineFactory->get(engineName);
        if (!_engine)
            throw std::runtime_error("Unsupported engine: " + engineName);

        // Default sun light
        DirectionalLightPtr sunLight(
            new DirectionalLight(DEFAULT_SUN_DIRECTION, DEFAULT_SUN_COLOR,
                                 DEFAULT_SUN_INTENSITY));
        _engine->getScene().addLight(sunLight);

        // Load data and build geometry
        buildScene();

        _engine->recreate = std::bind(&Impl::createEngine, this);

        BRAYNS_INFO << "Now rendering..." << std::endl;
    }

    void buildScene()
    {
        _meshLoader.clear();
        Scene& scene = _engine->getScene();
        _loadData();

        if (scene.empty() && !scene.getVolumeHandler())
        {
            BRAYNS_INFO << "Building default scene" << std::endl;
            scene.buildDefault();
        }

        scene.buildMaterials();
        scene.commitVolumeData();
        scene.commitSimulationData();
        scene.buildEnvironment();
        scene.buildGeometry();
        scene.commit();

        // Set default camera according to scene bounding box
        _setupCameraManipulator(CameraMode::inspect);
        _engine->setDefaultCamera();

        // Set default epsilon according to scene bounding box
        _engine->setDefaultEpsilon();

        // Commit changes to the rendering engine
        _engine->commit();
    }

#if (BRAYNS_USE_MAGICKPP)
    void _writeFrameToFolder()
    {
        const auto& frameExportFolder =
            _parametersManager->getApplicationParameters()
                .getFrameExportFolder();
        if (frameExportFolder.empty())
            return;
        try
        {
            char str[7];
            snprintf(str, 7, "%06d", int(_engine->getFrameNumber()));

            const std::string filename = frameExportFolder + "/" + str + ".png";
            FrameBuffer& frameBuffer = _engine->getFrameBuffer();
            std::string format;
            switch (frameBuffer.getFrameBufferFormat())
            {
            case FrameBufferFormat::rgba_i8:
                format = "RGBA";
                break;
            case FrameBufferFormat::rgb_i8:
                format = "RGB";
                break;
            default:
                BRAYNS_ERROR
                    << "Unsupported frame buffer format. Cannot export "
                       "frame to file as PNG image"
                    << std::endl;
                return;
            }
            uint8_t* colorBuffer = frameBuffer.getColorBuffer();
            const auto& size = frameBuffer.getSize();
            Magick::Image image(size.x(), size.y(), format, Magick::CharPixel,
                                colorBuffer);
            image.flip();
            image.write(filename);
        }
        catch (Magick::Warning& warning)
        {
            BRAYNS_WARN << warning.what() << std::endl;
            return;
        }
        catch (Magick::Error& error)
        {
            BRAYNS_ERROR << error.what() << std::endl;
            return;
        }
    }
#else
    void _writeFrameToFolder()
    {
        BRAYNS_DEBUG << "ImageMagick is required to export frames as PNG files"
                     << std::endl;
    }
#endif

#if (BRAYNS_USE_DEFLECT || BRAYNS_USE_NETWORKING)
    void _executePlugins(const Vector2ui& size)
    {
        auto oldEngine = _engine.get();
        _extensionPluginFactory->execute(_engine, *_keyboardHandler,
                                         *_cameraManipulator);

        if (!_engine)
            throw std::runtime_error("No valid engine found, aborting");

        // the ZeroEQ plugin can create a new engine
        if (_engine.get() != oldEngine)
        {
            _engine->reshape(size);
            _engine->preRender();
        }
    }
#else
    void _executePlugins(const Vector2ui&) {}
#endif

    void render(const RenderInput& renderInput, RenderOutput& renderOutput)
    {
        _engine->getCamera().set(renderInput.position, renderInput.target,
                                 renderInput.up);
        _engine->reshape(renderInput.windowSize);
        _engine->preRender();

#if (BRAYNS_USE_DEFLECT || BRAYNS_USE_NETWORKING)
        _executePlugins(renderInput.windowSize);
#endif
        auto& sceneParams = _parametersManager->getSceneParameters();
        if (sceneParams.getAnimationDelta() != 0)
            _engine->commit();

        Camera& camera = _engine->getCamera();
        camera.commit();

        Scene& scene = _engine->getScene();
        FrameBuffer& frameBuffer = _engine->getFrameBuffer();
        const Vector2i& frameSize = frameBuffer.getSize();

        if (_parametersManager->getRenderingParameters().getHeadLight())
        {
            LightPtr sunLight = scene.getLight(0);
            DirectionalLight* sun =
                dynamic_cast<DirectionalLight*>(sunLight.get());
            if (sun)
            {
                sun->setDirection(camera.getTarget() - camera.getPosition());
                scene.commitLights();
            }
        }

        _render();

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

        _engine->postRender();
    }

    void render()
    {
        const Vector2ui windowSize =
            _parametersManager->getApplicationParameters().getWindowSize();

        _engine->reshape(windowSize);
        _engine->preRender();

#if (BRAYNS_USE_DEFLECT || BRAYNS_USE_NETWORKING)
        _executePlugins(windowSize);
#endif
        Scene& scene = _engine->getScene();
        Camera& camera = _engine->getCamera();

        auto& sceneParams = _parametersManager->getSceneParameters();
        if (sceneParams.getAnimationDelta() != 0)
            _engine->commit();

        if (_parametersManager->getRenderingParameters().getHeadLight())
        {
            LightPtr sunLight = scene.getLight(0);
            DirectionalLight* sun =
                dynamic_cast<DirectionalLight*>(sunLight.get());
            if (sun)
            {
                sun->setDirection(camera.getTarget() - camera.getPosition());
                scene.commitLights();
            }
        }

        camera.commit();
        _render();

        _engine->postRender();
    }

    Engine& getEngine() { return *_engine; }
    ParametersManager& getParametersManager() { return *_parametersManager; }
    KeyboardHandler& getKeyboardHandler() { return *_keyboardHandler; }
    AbstractManipulator& getCameraManipulator() { return *_cameraManipulator; }
private:
    void _render()
    {
        _engine->setActiveRenderer(
            _parametersManager->getRenderingParameters().getRenderer());
        _engine->render();
        _writeFrameToFolder();
    }

    void _loadData()
    {
        auto& geometryParameters = _parametersManager->getGeometryParameters();
        auto& volumeParameters = _parametersManager->getVolumeParameters();
        auto& sceneParameters = _parametersManager->getSceneParameters();
        auto& scene = _engine->getScene();

        // set environment map if applicable
        const std::string& environmentMap =
            _parametersManager->getSceneParameters().getEnvironmentMap();
        if (!environmentMap.empty())
        {
            auto& material = scene.getMaterials()[MATERIAL_SKYBOX];
            material.setTexture(TT_DIFFUSE, environmentMap);
        }

        if (!geometryParameters.getSplashSceneFolder().empty())
            _loadMeshFolder(geometryParameters.getSplashSceneFolder());

        const std::string& colorMapFilename =
            sceneParameters.getColorMapFilename();
        if (!colorMapFilename.empty())
        {
            TransferFunctionLoader transferFunctionLoader;
            transferFunctionLoader.loadFromFile(colorMapFilename, scene);
        }
        scene.commitTransferFunctionData();

        if (!geometryParameters.getPDBFile().empty())
            _loadPDBFile(geometryParameters.getPDBFile());

        if (!geometryParameters.getPDBFolder().empty())
            _loadPDBFolder();

        if (!geometryParameters.getMeshFolder().empty())
            _loadMeshFolder(geometryParameters.getMeshFolder());

        if (!geometryParameters.getMeshFile().empty())
            _loadMeshFile(geometryParameters.getMeshFile());

#if (BRAYNS_USE_BRION)
        if (!geometryParameters.getSceneFile().empty())
            _loadSceneFile(geometryParameters.getSceneFile());

        if (!geometryParameters.getNESTCircuit().empty())
            _loadNESTCircuit();

        if (!geometryParameters.getMorphologyFolder().empty())
            _loadMorphologyFolder();

        if (!geometryParameters.getCircuitConfiguration().empty() &&
            geometryParameters.getLoadCacheFile().empty())
            _loadCircuitConfiguration();
#endif

        if (!geometryParameters.getXYZBFile().empty())
            _loadXYZBFile();

        if (!geometryParameters.getMolecularSystemConfig().empty())
            _loadMolecularSystem();

        if (scene.getVolumeHandler())
        {
            scene.getVolumeHandler()->setTimestamp(0.f);
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
    }

    /**
        Loads data from a PDB file (command line parameter --pdb-file)
    */
    void _loadPDBFolder()
    {
        // Load PDB File
        auto& geometryParameters = _parametersManager->getGeometryParameters();
        const std::string& folder = geometryParameters.getPDBFolder();
        const strings filters = {".pdb", ".pdb1"};
        const strings files = parseFolder(folder, filters);
        Progress progress("Loading PDB folder " + folder, files.size());
        for (const auto& file : files)
        {
            _loadPDBFile(file);
            ++progress;
        }
    }

    /**
        Loads data from a PDB file (command line parameter --pdb-file)
    */
    void _loadPDBFile(const std::string& filename)
    {
        // Load PDB File
        auto& geometryParameters = _parametersManager->getGeometryParameters();
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
    void _loadXYZBFile()
    {
        // Load XYZB File
        auto& geometryParameters = _parametersManager->getGeometryParameters();
        auto& scene = _engine->getScene();
        BRAYNS_INFO << "Loading XYZB file " << geometryParameters.getXYZBFile()
                    << std::endl;
        XYZBLoader xyzbLoader(geometryParameters);
        if (!xyzbLoader.importFromBinaryFile(geometryParameters.getXYZBFile(),
                                             scene))
            BRAYNS_ERROR << "Failed to import "
                         << geometryParameters.getXYZBFile() << std::endl;
    }

    /**
        Loads data from mesh files located in the folder specified in
       the geometry parameters (command line parameter --mesh-folder)
    */
    void _loadMeshFolder(const std::string& folder)
    {
#if (BRAYNS_USE_ASSIMP)
        auto& geometryParameters = _parametersManager->getGeometryParameters();
        auto& scene = _engine->getScene();

        strings filters = {".obj", ".dae", ".fbx", ".ply", ".lwo",
                           ".stl", ".3ds", ".ase", ".ifc", ".off"};
        strings files = parseFolder(folder, filters);
        size_t i = 0;
        Progress progress("Loading meshes from " + folder, files.size());
        for (const auto& file : files)
        {
            size_t material =
                geometryParameters.getColorScheme() == ColorScheme::neuron_by_id
                    ? NB_SYSTEM_MATERIALS + i
                    : NO_MATERIAL;

            if (!_meshLoader.importMeshFromFile(
                    file, scene, geometryParameters.getGeometryQuality(),
                    Matrix4f(), material))
                BRAYNS_ERROR << "Failed to import " << file << std::endl;
            ++i;
            ++progress;
        }
#else
        BRAYNS_ERROR << "Assimp library is required to load meshes from "
                     << folder << std::endl;
#endif
    }

    /**
        Loads data from mesh file (command line parameter --mesh-file)
    */
    void _loadMeshFile(const std::string& filename)
    {
#if (BRAYNS_USE_ASSIMP)
        auto& geometryParameters = _parametersManager->getGeometryParameters();
        auto& scene = _engine->getScene();

        strings filters = {".obj", ".dae", ".fbx", ".ply", ".lwo",
                           ".stl", ".3ds", ".ase", ".ifc", ".off"};
        size_t material =
            geometryParameters.getColorScheme() == ColorScheme::neuron_by_id
                ? NB_SYSTEM_MATERIALS
                : NO_MATERIAL;

        if (!_meshLoader.importMeshFromFile(
                filename, scene, geometryParameters.getGeometryQuality(),
                Matrix4f(), material))
            BRAYNS_ERROR << "Failed to import " << filename << std::endl;
#else
        BRAYNS_ERROR << "Assimp library is required to load meshes from "
                     << filename << std::endl;
#endif
    }

#if (BRAYNS_USE_BRION)
    /**
     * Loads data from a scene description file (command line parameter
     * --scene-file)
     */
    void _loadSceneFile(const std::string& filename)
    {
        auto& geometryParameters = _parametersManager->getGeometryParameters();
        auto& scene = _engine->getScene();
        SceneLoader sceneLoader(geometryParameters);
        sceneLoader.importFromFile(filename, scene, _meshLoader);
    }

    /**
     * Loads data from a NEST circuit file (command line parameter
     * --nest-circuit)
     */
    void _loadNESTCircuit()
    {
        auto& geometryParameters = _parametersManager->getGeometryParameters();
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
                        _parametersManager->getGeometryParameters()));
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
                scene.commitTransferFunctionData();
            }
        }
    }

    /**
        Loads data from SWC and H5 files located in the folder specified
       in the geometry parameters (command line parameter --morphology-folder)
    */
    void _loadMorphologyFolder()
    {
        auto& geometryParameters = _parametersManager->getGeometryParameters();
        auto& scene = _engine->getScene();
        const auto& folder = geometryParameters.getMorphologyFolder();
        MorphologyLoader morphologyLoader(geometryParameters, scene);

        const strings filters = {".swc", ".h5"};
        const strings files = parseFolder(folder, filters);
        uint64_t morphologyIndex = 0;
        Progress progress("Loading morphologies from " + folder, files.size());
        for (const auto& file : files)
        {
            ++progress;
            servus::URI uri(file);
            if (!morphologyLoader.importMorphology(uri, morphologyIndex,
                                                   NO_MATERIAL))
                BRAYNS_ERROR << "Failed to import " << file << std::endl;
            ++morphologyIndex;
        }
    }

    /**
        Loads morphologies from circuit configuration (command line
       parameter --circuit-configuration)
    */
    void _loadCircuitConfiguration()
    {
        auto& geometryParameters = _parametersManager->getGeometryParameters();
        auto& scene = _engine->getScene();
        const std::string& filename =
            geometryParameters.getCircuitConfiguration();
        const std::string& target = geometryParameters.getCircuitTarget();

        BRAYNS_INFO << "Loading circuit configuration from " << filename
                    << std::endl;
        const std::string& report = geometryParameters.getCircuitReport();
        MorphologyLoader morphologyLoader(geometryParameters, scene);
        const servus::URI uri(filename);
        morphologyLoader.importCircuit(uri, target, report, scene, _meshLoader);
    }

#endif // BRAYNS_USE_BRION

    /**
        Loads molecular system from configuration (command line
       parameter
        --molecular-system-config )
    */
    void _loadMolecularSystem()
    {
        auto& geometryParameters = _parametersManager->getGeometryParameters();
        auto& scene = _engine->getScene();
        MolecularSystemReader molecularSystemReader(geometryParameters);
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
                                          *_keyboardHandler));
            break;
        case CameraMode::inspect:
            _cameraManipulator.reset(
                new InspectCenterManipulator(_engine->getCamera(),
                                             *_keyboardHandler));
            break;
        };
    }

    void _registerKeyboardShortcuts()
    {
        _keyboardHandler->registerKeyboardShortcut(
            '0', "Black background",
            std::bind(&Brayns::Impl::_blackBackground, this));
        _keyboardHandler->registerKeyboardShortcut(
            '1', "Gray background",
            std::bind(&Brayns::Impl::_grayBackground, this));
        _keyboardHandler->registerKeyboardShortcut(
            '2', "White background",
            std::bind(&Brayns::Impl::_whiteBackground, this));
        _keyboardHandler->registerKeyboardShortcut(
            '3', "Set gradient materials",
            std::bind(&Brayns::Impl::_gradientMaterials, this));
        _keyboardHandler->registerKeyboardShortcut(
            '4', "Set pastel materials",
            std::bind(&Brayns::Impl::_pastelMaterials, this));
        _keyboardHandler->registerKeyboardShortcut(
            '5', "Set random materials",
            std::bind(&Brayns::Impl::_randomMaterials, this));
        _keyboardHandler->registerKeyboardShortcut(
            '6', "Default renderer",
            std::bind(&Brayns::Impl::_defaultRenderer, this));
        _keyboardHandler->registerKeyboardShortcut(
            '7', "Particle renderer",
            std::bind(&Brayns::Impl::_particleRenderer, this));
        _keyboardHandler->registerKeyboardShortcut(
            '8', "Proximity renderer",
            std::bind(&Brayns::Impl::_proximityRenderer, this));
        _keyboardHandler->registerKeyboardShortcut(
            '9', "Simulation renderer",
            std::bind(&Brayns::Impl::_simulationRenderer, this));
        _keyboardHandler->registerKeyboardShortcut(
            '[', "Decrease timestamp by 1",
            std::bind(&Brayns::Impl::_decreaseTimestamp, this));
        _keyboardHandler->registerKeyboardShortcut(
            ']', "Increase timestamp by 1",
            std::bind(&Brayns::Impl::_increaseTimestamp, this));
        _keyboardHandler->registerKeyboardShortcut(
            'e', "Enable eletron shading",
            std::bind(&Brayns::Impl::_electronShading, this));
        _keyboardHandler->registerKeyboardShortcut(
            'f', "Enable fly mode", [this]() {
                Brayns::Impl::_setupCameraManipulator(CameraMode::flying);
            });
        _keyboardHandler->registerKeyboardShortcut(
            'i', "Enable inspect mode", [this]() {
                Brayns::Impl::_setupCameraManipulator(CameraMode::inspect);
            });
        _keyboardHandler->registerKeyboardShortcut(
            'o', "Decrease ambient occlusion strength",
            std::bind(&Brayns::Impl::_decreaseAmbientOcclusionStrength, this));
        _keyboardHandler->registerKeyboardShortcut(
            'O', "Increase ambient occlusion strength",
            std::bind(&Brayns::Impl::_increaseAmbientOcclusionStrength, this));
        _keyboardHandler->registerKeyboardShortcut(
            'p', "Enable diffuse shading",
            std::bind(&Brayns::Impl::_diffuseShading, this));
        _keyboardHandler->registerKeyboardShortcut(
            'P', "Disable shading",
            std::bind(&Brayns::Impl::_disableShading, this));
        _keyboardHandler->registerKeyboardShortcut(
            'r', "Set timestamp to 0",
            std::bind(&Brayns::Impl::_resetTimestamp, this));
        _keyboardHandler->registerKeyboardShortcut(
            'R', "Set timestamp to infinity",
            std::bind(&Brayns::Impl::_infiniteTimestamp, this));
        _keyboardHandler->registerKeyboardShortcut(
            'u', "Enable/Disable shadows",
            std::bind(&Brayns::Impl::_toggleShadows, this));
        _keyboardHandler->registerKeyboardShortcut(
            'U', "Enable/Disable soft shadows",
            std::bind(&Brayns::Impl::_toggleSoftShadows, this));
        _keyboardHandler->registerKeyboardShortcut(
            't', "Multiply samples per ray by 2",
            std::bind(&Brayns::Impl::_increaseSamplesPerRay, this));
        _keyboardHandler->registerKeyboardShortcut(
            'T', "Divide samples per ray by 2",
            std::bind(&Brayns::Impl::_decreaseSamplesPerRay, this));
        _keyboardHandler->registerKeyboardShortcut(
            'y', "Enable/Disable light emitting materials",
            std::bind(&Brayns::Impl::_toggleLightEmittingMaterials, this));
        _keyboardHandler->registerKeyboardShortcut(
            'l', "Toggle load dynamic/static load balancer",
            std::bind(&Brayns::Impl::_toggleLoadBalancer, this));
        _keyboardHandler->registerKeyboardShortcut(
            'g', "Enable/Disable timestamp auto-increment",
            std::bind(&Brayns::Impl::_toggleIncrementalTimestamp, this));
        _keyboardHandler->registerKeyboardShortcut(
            'x', "Set timestamp to " + std::to_string(DEFAULT_TEST_TIMESTAMP),
            std::bind(&Brayns::Impl::_defaultTimestamp, this));
        _keyboardHandler->registerKeyboardShortcut(
            '|', "Create cache file ",
            std::bind(&Brayns::Impl::_saveSceneToCacheFile, this));
        _keyboardHandler->registerKeyboardShortcut(
            '{', "Decrease eye separation",
            std::bind(&Brayns::Impl::_decreaseEyeSeparation, this));
        _keyboardHandler->registerKeyboardShortcut(
            '}', "Increase eye separation",
            std::bind(&Brayns::Impl::_increaseEyeSeparation, this));
        _keyboardHandler->registerKeyboardShortcut(
            '<', "Decrease field of view",
            std::bind(&Brayns::Impl::_decreaseFieldOfView, this));
        _keyboardHandler->registerKeyboardShortcut(
            '>', "Increase field of view",
            std::bind(&Brayns::Impl::_increaseFieldOfView, this));
        _keyboardHandler->registerKeyboardShortcut(
            ' ', "Camera reset to initial state",
            std::bind(&Brayns::Impl::_resetCamera, this));
        _keyboardHandler->registerKeyboardShortcut(
            '+', "Increase motion speed",
            std::bind(&Brayns::Impl::_increaseMotionSpeed, this));
        _keyboardHandler->registerKeyboardShortcut(
            '-', "Decrease motion speed",
            std::bind(&Brayns::Impl::_decreaseMotionSpeed, this));
        _keyboardHandler->registerKeyboardShortcut(
            'c', "Display current camera information",
            std::bind(&Brayns::Impl::_displayCameraInformation, this));
    }

    void _blackBackground()
    {
        RenderingParameters& renderParams =
            _parametersManager->getRenderingParameters();
        renderParams.setBackgroundColor(Vector3f(0.f, 0.f, 0.f));
    }

    void _grayBackground()
    {
        RenderingParameters& renderParams =
            _parametersManager->getRenderingParameters();
        renderParams.setBackgroundColor(Vector3f(0.5f, 0.5f, 0.5f));
    }

    void _whiteBackground()
    {
        RenderingParameters& renderParams =
            _parametersManager->getRenderingParameters();
        renderParams.setBackgroundColor(Vector3f(1.f, 1.f, 1.f));
    }

    void _defaultRenderer()
    {
        RenderingParameters& renderParams =
            _parametersManager->getRenderingParameters();
        renderParams.setRenderer(RendererType::basic);
    }

    void _particleRenderer()
    {
        RenderingParameters& renderParams =
            _parametersManager->getRenderingParameters();
        renderParams.setRenderer(RendererType::particle);
    }

    void _proximityRenderer()
    {
        RenderingParameters& renderParams =
            _parametersManager->getRenderingParameters();
        renderParams.setRenderer(RendererType::proximity);
    }

    void _simulationRenderer()
    {
        RenderingParameters& renderParams =
            _parametersManager->getRenderingParameters();
        renderParams.setRenderer(RendererType::simulation);
    }

    void _increaseTimestamp()
    {
        SceneParameters& sceneParams = _parametersManager->getSceneParameters();
        float ts = sceneParams.getTimestamp();
        sceneParams.setTimestamp(ts + 1.f);
    }

    void _decreaseTimestamp()
    {
        SceneParameters& sceneParams = _parametersManager->getSceneParameters();
        float ts = sceneParams.getTimestamp();
        if (ts > 0.f)
            sceneParams.setTimestamp(ts - 1.f);
    }

    void _diffuseShading()
    {
        RenderingParameters& renderParams =
            _parametersManager->getRenderingParameters();
        renderParams.setShading(ShadingType::diffuse);
    }

    void _electronShading()
    {
        RenderingParameters& renderParams =
            _parametersManager->getRenderingParameters();
        renderParams.setShading(ShadingType::electron);
    }

    void _disableShading()
    {
        RenderingParameters& renderParams =
            _parametersManager->getRenderingParameters();
        renderParams.setShading(ShadingType::none);
    }

    void _increaseAmbientOcclusionStrength()
    {
        RenderingParameters& renderParams =
            _parametersManager->getRenderingParameters();
        float aaStrength = renderParams.getAmbientOcclusionStrength();
        aaStrength += 0.1f;
        if (aaStrength > 1.f)
            aaStrength = 1.f;
        renderParams.setAmbientOcclusionStrength(aaStrength);
    }

    void _decreaseAmbientOcclusionStrength()
    {
        RenderingParameters& renderParams =
            _parametersManager->getRenderingParameters();
        float aaStrength = renderParams.getAmbientOcclusionStrength();
        aaStrength -= 0.1f;
        if (aaStrength < 0.f)
            aaStrength = 0.f;
        renderParams.setAmbientOcclusionStrength(aaStrength);
    }

    void _resetTimestamp()
    {
        SceneParameters& sceneParams = _parametersManager->getSceneParameters();
        sceneParams.setTimestamp(0.f);
    }

    void _infiniteTimestamp()
    {
        SceneParameters& sceneParams = _parametersManager->getSceneParameters();
        sceneParams.setTimestamp(std::numeric_limits<size_t>::max());
    }

    void _toggleShadows()
    {
        RenderingParameters& renderParams =
            _parametersManager->getRenderingParameters();
        renderParams.setShadows(renderParams.getShadows() == 0.f ? 1.f : 0.f);
    }

    void _toggleSoftShadows()
    {
        RenderingParameters& renderParams =
            _parametersManager->getRenderingParameters();
        renderParams.setSoftShadows(renderParams.getSoftShadows() == 0.f ? 0.1f
                                                                         : 0.f);
    }

    void _increaseSamplesPerRay()
    {
        VolumeParameters& volumeParams =
            _parametersManager->getVolumeParameters();
        volumeParams.setSamplesPerRay(volumeParams.getSamplesPerRay() * 2);
        _engine->getScene().commitVolumeData();
    }

    void _decreaseSamplesPerRay()
    {
        VolumeParameters& volumeParams =
            _parametersManager->getVolumeParameters();
        if (volumeParams.getSamplesPerRay() >= 4)
            volumeParams.setSamplesPerRay(volumeParams.getSamplesPerRay() / 2);
        _engine->getScene().commitVolumeData();
    }

    void _toggleLightEmittingMaterials()
    {
        RenderingParameters& renderParams =
            _parametersManager->getRenderingParameters();
        renderParams.setLightEmittingMaterials(
            !renderParams.getLightEmittingMaterials());
    }

    void _toggleLoadBalancer()
    {
        RenderingParameters& renderParams =
            _parametersManager->getRenderingParameters();
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
        _engine->initializeMaterials(MaterialType::gradient);
    }

    void _pastelMaterials()
    {
        _engine->initializeMaterials(MaterialType::pastel);
    }

    void _randomMaterials()
    {
        _engine->initializeMaterials(MaterialType::random);
    }

    void _toggleIncrementalTimestamp()
    {
        auto& sceneParams = _parametersManager->getSceneParameters();
        sceneParams.setAnimationDelta(sceneParams.getAnimationDelta() == 0 ? 1
                                                                           : 0);
    }

    void _defaultTimestamp()
    {
        auto& sceneParams = _parametersManager->getSceneParameters();
        sceneParams.setTimestamp(DEFAULT_TEST_TIMESTAMP);
    }

    void _saveSceneToCacheFile()
    {
        auto& scene = _engine->getScene();
        scene.saveSceneToCacheFile();
    }

    void _resetCamera()
    {
        auto& camera = _engine->getCamera();
        camera.reset();
        camera.commit();
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

    std::unique_ptr<EngineFactory> _engineFactory;
    ParametersManagerPtr _parametersManager;
    EnginePtr _engine;
    KeyboardHandlerPtr _keyboardHandler;
    AbstractManipulatorPtr _cameraManipulator;
    MeshLoader _meshLoader;

    float _fieldOfView{45.f};
    float _eyeSeparation{0.0635f};

#if (BRAYNS_USE_DEFLECT || BRAYNS_USE_NETWORKING)
    ExtensionPluginFactoryPtr _extensionPluginFactory;
#endif
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
    _impl->render(renderInput, renderOutput);
}

void Brayns::render()
{
    _impl->render();
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

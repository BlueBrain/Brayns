/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
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

#include <brayns/common/log.h>
#include <brayns/common/engine/Engine.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/common/camera/Camera.h>
#include <brayns/common/camera/FlyingModeManipulator.h>
#include <brayns/common/camera/InspectCenterManipulator.h>
#include <brayns/common/renderer/FrameBuffer.h>
#include <brayns/common/volume/VolumeHandler.h>
#include <brayns/common/light/DirectionalLight.h>
#include <brayns/common/simulation/CircuitSimulationHandler.h>
#include <brayns/common/simulation/SpikeSimulationHandler.h>
#include <brayns/common/input/KeyboardHandler.h>

#include <brayns/parameters/ParametersManager.h>

#include <brayns/io/MorphologyLoader.h>
#include <brayns/io/NESTLoader.h>
#include <brayns/io/ProteinLoader.h>
#include <brayns/io/MeshLoader.h>
#include <brayns/io/TransferFunctionLoader.h>
#include <brayns/io/XYZBLoader.h>

#include <plugins/engines/EngineFactory.h>
#include <plugins/extensions/ExtensionPluginFactory.h>

#include <boost/filesystem.hpp>
#include <servus/uri.h>

namespace brayns
{

struct Brayns::Impl
{
    Impl( int argc, const char **argv )
    : _engine( nullptr )
    {
        BRAYNS_INFO << "Parsing command line options" << std::endl;
        _parametersManager.reset( new ParametersManager( ));
        _parametersManager->parse( argc, argv );
        _parametersManager->print( );

        // Get rendering engine
        EngineFactory engineFactory( argc, argv, *_parametersManager );
        std::string engineName = _parametersManager->getRenderingParameters().getEngine();
        _engine = engineFactory.get( engineName );
        if( !_engine )
            throw std::runtime_error( "Unsupported engine: " + engineName );

        // Initialize keyboard handler
        _keyboardHandler.reset( new KeyboardHandler( ));
        _registerKeyboardShortcuts();

        // Default sun light
        DirectionalLightPtr sunLight( new DirectionalLight(
            DEFAULT_SUN_DIRECTION, DEFAULT_SUN_COLOR, DEFAULT_SUN_INTENSITY ));
        _engine->getScene().addLight( sunLight );

        // Load data and build geometry
        buildScene();
    }

    void buildScene()
    {
        _loadData();
        Scene& scene = _engine->getScene();
        scene.commitVolumeData();
        scene.buildEnvironment();
        scene.buildGeometry();

        if( scene.empty() && !scene.getVolumeHandler( ))
        {
            BRAYNS_INFO << "Building default scene" << std::endl;
            scene.buildDefault();
            scene.buildGeometry();
        }

        scene.commit();

        // Set default camera according to scene bounding box
        _setupCameraManipulator( CameraMode::inspect );
        _engine->setDefaultCamera();

        // Set default epsilon according to scene bounding box
        _engine->setDefaultEpsilon( );

        // Commit changes to the rendering engine
        _engine->commit();
    }

    void render( const RenderInput& renderInput,
                 RenderOutput& renderOutput )
    {
        Camera& camera = _engine->getCamera();
        camera.set( renderInput.position, renderInput.target, renderInput.up );

        _engine->reshape( renderInput.windowSize );
        _engine->preRender();

#if(BRAYNS_USE_DEFLECT || BRAYNS_USE_REST)
        if( !_extensionPluginFactory )
            _intializeExtensionPluginFactory( );
        _extensionPluginFactory->execute( );
        if( _engine->isDirty( ))
        {
            _engine->getScene().reset();
            _engine->initializeMaterials();
            buildScene();
        }
#endif

        camera.commit();

        Scene& scene = _engine->getScene();
        FrameBuffer& frameBuffer = _engine->getFrameBuffer();
        const Vector2i& frameSize = frameBuffer.getSize();

        if( _parametersManager->getRenderingParameters().getHeadLight() )
        {
            LightPtr sunLight = scene.getLight( 0 );
            DirectionalLight* sun = dynamic_cast< DirectionalLight* > ( sunLight.get() );
            if( sun )
            {
                sun->setDirection( camera.getTarget() - camera.getPosition() );
                scene.commitLights();
            }
        }

        _render( );

        uint8_t* colorBuffer = frameBuffer.getColorBuffer( );
        if( colorBuffer )
        {
            const size_t size = frameSize.x( ) * frameSize.y( ) * frameBuffer.getColorDepth( );
            renderOutput.colorBuffer.assign( colorBuffer, colorBuffer + size );
        }

        float* depthBuffer = frameBuffer.getDepthBuffer( );
        if( depthBuffer )
        {
            const size_t size = frameSize.x( ) * frameSize.y( );
            renderOutput.depthBuffer.assign( depthBuffer, depthBuffer + size );
        }

        _engine->postRender();
    }

    void render()
    {
        Scene& scene = _engine->getScene();
        Camera& camera = _engine->getCamera();
        const Vector2ui windowSize = _parametersManager->getApplicationParameters().getWindowSize();
        _engine->reshape( windowSize );

        _engine->preRender();

#if(BRAYNS_USE_DEFLECT || BRAYNS_USE_REST)
        if( !_extensionPluginFactory )
            _intializeExtensionPluginFactory( );
        _extensionPluginFactory->execute( );
        if( _engine->isDirty( ))
        {
            _engine->getScene().reset();
            _engine->initializeMaterials();
            buildScene();
        }
#endif

        if( _parametersManager->getRenderingParameters().getHeadLight() )
        {
            LightPtr sunLight = scene.getLight( 0 );
            DirectionalLight* sun =
                dynamic_cast< DirectionalLight* > ( sunLight.get() );
            if( sun )
            {
                sun->setDirection( camera.getTarget() - camera.getPosition( ));
                scene.commitLights();
            }
        }

        camera.commit();
        _render( );

        _engine->postRender();
    }

    Engine& getEngine( )
    {
        return *_engine;
    }

    ParametersManager& getParametersManager()
    {
        return *_parametersManager;
    }

    KeyboardHandler& getKeyboardHandler()
    {
        return *_keyboardHandler;
    }

    AbstractManipulator& getCameraManipulator()
    {
        return *_cameraManipulator;
    }

private:

#if(BRAYNS_USE_DEFLECT || BRAYNS_USE_REST)
    void _intializeExtensionPluginFactory( )
    {
        _extensionParameters.parametersManager = _parametersManager;
        _extensionParameters.engine = _engine;

        _extensionPluginFactory.reset( new ExtensionPluginFactory(
            *_engine, *_parametersManager, *_keyboardHandler, *_cameraManipulator ));
    }
#endif

    void _render( )
    {
        _engine->setActiveRenderer( _parametersManager->getRenderingParameters().getRenderer( ));
        _engine->render();
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
        if( !environmentMap.empty() )
            scene.getMaterial(MATERIAL_SKYBOX)->setTexture(TT_DIFFUSE, environmentMap);

        if(!geometryParameters.getSplashSceneFolder().empty())
            _loadMeshFolder( geometryParameters.getSplashSceneFolder( ));

        const std::string& colorMapFilename = sceneParameters.getColorMapFilename();
        if( !colorMapFilename.empty() )
        {
            TransferFunctionLoader transferFunctionLoader( DEFAULT_TRANSFER_FUNCTION_RANGE );
            transferFunctionLoader.loadFromFile( colorMapFilename, scene );
        }
        scene.commitTransferFunctionData();

        if(!geometryParameters.getMorphologyFolder().empty())
            _loadMorphologyFolder();

        if(!geometryParameters.getNESTCircuit().empty())
            _loadNESTCircuit();

        if(!geometryParameters.getPDBFile().empty())
            _loadPDBFile( geometryParameters.getPDBFile( ));

        if(!geometryParameters.getPDBFolder().empty())
            _loadPDBFolder();

        if(!geometryParameters.getMeshFolder().empty())
            _loadMeshFolder( geometryParameters.getMeshFolder( ));

        if(!geometryParameters.getReport().empty())
            _loadCompartmentReport();

        if(!geometryParameters.getCircuitConfiguration().empty() &&
            geometryParameters.getLoadCacheFile().empty())
            _loadCircuitConfiguration();

        if(!geometryParameters.getXYZBFile().empty())
            _loadXYZBFile();

        if(!volumeParameters.getFilename().empty() || !volumeParameters.getFolder().empty())
        {
            scene.getVolumeHandler()->setTimestamp( 0.f );
            const Vector3ui& volumeDimensions = scene.getVolumeHandler()->getDimensions();
            const Vector3f& volumeOffset = scene.getVolumeHandler()->getOffset();
            const Vector3f& volumeElementSpacing = volumeParameters.getElementSpacing();
            Boxf& worldBounds = scene.getWorldBounds();
            worldBounds.merge( Vector3f( 0.f, 0.f, 0.f ));
            worldBounds.merge( volumeOffset + Vector3f( volumeDimensions ) * volumeElementSpacing );
        }
    }

    /**
        Loads data from SWC and H5 files located in the folder specified in the
        geometry parameters (command line parameter --morphology-folder)
    */
    void _loadMorphologyFolder()
    {
        auto& geometryParameters = _parametersManager->getGeometryParameters();
        auto& scene = _engine->getScene();
        const boost::filesystem::path& folder = geometryParameters.getMorphologyFolder( );
        BRAYNS_INFO << "Loading morphologies from " << folder << std::endl;
        MorphologyLoader morphologyLoader( geometryParameters );

        size_t fileIndex = 0;
        boost::filesystem::directory_iterator endIter;
        if( boost::filesystem::is_directory(folder))
        {
            for( boost::filesystem::directory_iterator dirIter( folder );
                 dirIter != endIter; ++dirIter )
            {
                if( boost::filesystem::is_regular_file(dirIter->status( )))
                {
                    boost::filesystem::path fileExtension =
                        dirIter->path( ).extension( );
                    if( fileExtension==".swc" || fileExtension==".h5" )
                    {
                        const std::string& filename = dirIter->path( ).string( );
                        servus::URI uri( filename );
                        if( !morphologyLoader.importMorphology(
                            uri, fileIndex++, scene ))
                        {
                            BRAYNS_ERROR << "Failed to import " <<
                                filename << std::endl;
                        }
                    }
                }
            }
        }
    }

    /**
     * Loads data from a NEST circuit file (command line parameter --nest-circuit)
     */
    void _loadNESTCircuit()
    {
        auto& geometryParameters = _parametersManager->getGeometryParameters();
        auto& scene = _engine->getScene();

        const std::string& circuit( geometryParameters.getNESTCircuit( ));
        if( !circuit.empty( ))
        {
            size_t nbMaterials;
            NESTLoader loader( geometryParameters );
            loader.importCircuit( circuit, scene, nbMaterials );
            loader.importSpikeReport( geometryParameters.getNESTReport(), scene );

            const std::string& cacheFile( geometryParameters.getNESTCacheFile( ));
            if( !cacheFile.empty( ))
            {
                SpikeSimulationHandlerPtr simulationHandler(
                    new SpikeSimulationHandler( _parametersManager->getGeometryParameters( )));
                simulationHandler->attachSimulationToCacheFile( cacheFile );
                scene.setSimulationHandler( simulationHandler );
            }

            auto& sceneParameters = _parametersManager->getSceneParameters();
            const std::string& colorMapFilename =
                sceneParameters.getColorMapFilename();
            if( !colorMapFilename.empty() )
            {
                TransferFunctionLoader transferFunctionLoader( brayns::Vector2f( 0, nbMaterials ));
                transferFunctionLoader.loadFromFile( colorMapFilename, scene );
                scene.commitTransferFunctionData();
            }

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
        BRAYNS_INFO << "Loading PDB folder " << folder << std::endl;
        boost::filesystem::directory_iterator endIter;
        if( boost::filesystem::is_directory( folder ))
        {
            for( boost::filesystem::directory_iterator dirIter( folder );
                 dirIter != endIter; ++dirIter )
            {
                if( boost::filesystem::is_regular_file(dirIter->status( )))
                {
                    boost::filesystem::path fileExtension =
                        dirIter->path( ).extension( );
                    if( fileExtension==".pdb" || fileExtension==".pdb1" )
                    {
                        const std::string& filename = dirIter->path().string();
                        BRAYNS_INFO << "- " << filename << std::endl;
                        _loadPDBFile( filename );
                    }
                }
            }
        }
    }

    /**
        Loads data from a PDB file (command line parameter --pdb-file)
    */
    void _loadPDBFile( const std::string& filename )
    {
        // Load PDB File
        auto& geometryParameters = _parametersManager->getGeometryParameters();
        auto& scene = _engine->getScene();
        std::string pdbFile = filename;
        if( pdbFile == "" )
        {
            pdbFile = geometryParameters.getPDBFile();
            BRAYNS_INFO << "Loading PDB file " << pdbFile << std::endl;
        }
        ProteinLoader proteinLoader( geometryParameters );
        if( !proteinLoader.importPDBFile( pdbFile, Vector3f( 0, 0, 0 ), 0, scene ))
            BRAYNS_ERROR << "Failed to import " << pdbFile << std::endl;

        for( size_t i = 0; i < scene.getMaterials().size( ); ++i )
        {
            float r,g,b;
            proteinLoader.getMaterialKd( i, r, g, b );
            MaterialPtr material = scene.getMaterials()[i];
            material->setColor( Vector3f( r, g, b ));
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
        BRAYNS_INFO << "Loading XYZB file " << geometryParameters.getXYZBFile() << std::endl;
        XYZBLoader xyzbLoader( geometryParameters );
        if( !xyzbLoader.importFromBinaryFile( geometryParameters.getXYZBFile(), scene ))
            BRAYNS_ERROR << "Failed to import " << geometryParameters.getXYZBFile() << std::endl;
    }

    /**
        Loads data from mesh files located in the folder specified in the
        geometry parameters (command line parameter --mesh-folder)
    */
    void _loadMeshFolder( const std::string& folder )
    {
    #ifdef BRAYNS_USE_ASSIMP
        BRAYNS_INFO << "Loading meshes from " << folder << std::endl;
        MeshLoader meshLoader;
        size_t meshIndex = 0;

        auto& geometryParameters = _parametersManager->getGeometryParameters();
        auto& scene = _engine->getScene();

        boost::filesystem::directory_iterator endIter;
        if( boost::filesystem::is_directory(folder))
        {
            for( boost::filesystem::directory_iterator dirIter( folder );
                 dirIter != endIter; ++dirIter )
            {
                if( boost::filesystem::is_regular_file(dirIter->status( )))
                {
                    const std::string& filename = dirIter->path( ).string( );
                    BRAYNS_INFO << "- " << filename << std::endl;
                    MeshContainer MeshContainer =
                    {
                        scene.getTriangleMeshes(), scene.getMaterials(),
                        scene.getWorldBounds()
                    };

                    size_t material =
                        geometryParameters.getColorScheme() == ColorScheme::neuron_by_id ?
                        meshIndex % (NB_MAX_MATERIALS - NB_SYSTEM_MATERIALS) :
                        NO_MATERIAL;

                    MeshQuality quality;
                    switch( geometryParameters.getGeometryQuality() )
                    {
                    case GeometryQuality::medium : quality = MQ_QUALITY; break;
                    case GeometryQuality::high : quality = MQ_MAX_QUALITY; break;
                    default: quality = MQ_FAST ; break;
                    }

                    if(!meshLoader.importMeshFromFile(
                        filename, MeshContainer, quality, material ))
                    {
                        BRAYNS_ERROR << "Failed to import " <<
                        filename << std::endl;
                    }
                    ++meshIndex;
                }
            }
        }
    #else
        BRAYNS_ERROR << "Assimp library is required to load meshes from " << folder << std::endl;
    #endif
    }

    /**
        Loads morphologies from circuit configuration (command line parameter
        --circuit-configuration)
    */
    void _loadCircuitConfiguration()
    {
        auto& geometryParameters = _parametersManager->getGeometryParameters();
        auto& scene = _engine->getScene();
        const std::string& filename = geometryParameters.getCircuitConfiguration( );
        const std::string& target = geometryParameters.getTarget( );

        BRAYNS_INFO << "Loading circuit configuration from " << filename << std::endl;
        const std::string& report = geometryParameters.getReport( );
        MorphologyLoader morphologyLoader( geometryParameters );
        const servus::URI uri( filename );
        if( report.empty() )
            morphologyLoader.importCircuit( uri, target, scene );
        else
            morphologyLoader.importCircuit( uri, target, report, scene );
    }

    /**
        Loads compartment report from circuit configuration (command line
        parameter --report)
        @return the number of simulation frames loaded
    */
    void _loadCompartmentReport()
    {
        auto& geometryParameters = _parametersManager->getGeometryParameters();
        auto& scene = _engine->getScene();
        const std::string& filename = geometryParameters.getCircuitConfiguration( );
        const std::string& target = geometryParameters.getTarget( );
        const std::string& report = geometryParameters.getReport( );
        BRAYNS_INFO << "Loading compartment report from " << filename << std::endl;
        MorphologyLoader morphologyLoader( geometryParameters );
        const servus::URI uri( filename );
        if( morphologyLoader.importSimulationData( uri, target, report, scene ))
        {
            auto& sceneParameters = _parametersManager->getSceneParameters();
            const std::string& colorMapFilename =
                sceneParameters.getColorMapFilename();
            if( !colorMapFilename.empty() )
            {
                TransferFunctionLoader transferFunctionLoader( DEFAULT_TRANSFER_FUNCTION_RANGE );
                transferFunctionLoader.loadFromFile( colorMapFilename, scene );
                scene.commitTransferFunctionData();
            }
        }
    }

    void _buildDefaultScene()
    {
        auto& scene = _engine->getScene();
        scene.buildDefault();
        scene.buildGeometry();
    }

    void _setupCameraManipulator( const CameraMode mode )
    {
        _cameraManipulator.reset(); // deregister previous keyboard handlers

        switch( mode )
        {
        case CameraMode::flying:
            _cameraManipulator.reset( new FlyingModeManipulator(
                _engine->getCamera(), *_keyboardHandler ));
            break;
        case CameraMode::inspect:
            _cameraManipulator.reset( new InspectCenterManipulator(
                _engine->getCamera(), *_keyboardHandler ));
            break;
        };
    }

    void _registerKeyboardShortcuts()
    {
        _keyboardHandler->registerKeyboardShortcut(
            '0', "Black background",
             std::bind( &Brayns::Impl::_blackBackground, this ));
        _keyboardHandler->registerKeyboardShortcut(
            '1', "Gray background",
            std::bind( &Brayns::Impl::_grayBackground, this ));
        _keyboardHandler->registerKeyboardShortcut(
            '2', "White background",
            std::bind( &Brayns::Impl::_whiteBackground, this ));
        _keyboardHandler->registerKeyboardShortcut(
            '6', "Default renderer",
            std::bind( &Brayns::Impl::_defaultRenderer, this ));
        _keyboardHandler->registerKeyboardShortcut(
            '7', "Particle renderer",
            std::bind( &Brayns::Impl::_particleRenderer, this ));
        _keyboardHandler->registerKeyboardShortcut(
            '8', "Proximity renderer",
            std::bind( &Brayns::Impl::_proximityRenderer, this ));
        _keyboardHandler->registerKeyboardShortcut(
            '9', "Simulation renderer",
            std::bind( &Brayns::Impl::_simulationRenderer, this ));
        _keyboardHandler->registerKeyboardShortcut(
            '[', "Decrease timestamp by 1",
            std::bind( &Brayns::Impl::_decreaseTimestamp, this ));
        _keyboardHandler->registerKeyboardShortcut(
            ']', "Increase timestamp by 1",
            std::bind( &Brayns::Impl::_increaseTimestamp, this ));
        _keyboardHandler->registerKeyboardShortcut(
            'e', "Enable eletron shading",
            std::bind( &Brayns::Impl::_electronShading, this ));
        _keyboardHandler->registerKeyboardShortcut(
            'f', "Enable fly mode",
            [this](){ Brayns::Impl::_setupCameraManipulator( CameraMode::flying ); });
        _keyboardHandler->registerKeyboardShortcut(
            'i', "Enable inspect mode",
            [this](){ Brayns::Impl::_setupCameraManipulator( CameraMode::inspect ); });
        _keyboardHandler->registerKeyboardShortcut(
            'o', "Decrease ambient occlusion strength",
            std::bind( &Brayns::Impl::_decreaseAmbientOcclusionStrength, this ));
        _keyboardHandler->registerKeyboardShortcut(
            'O', "Increase ambient occlusion strength",
            std::bind( &Brayns::Impl::_increaseAmbientOcclusionStrength, this ));
        _keyboardHandler->registerKeyboardShortcut(
            'p', "Enable diffuse shading",
            std::bind( &Brayns::Impl::_diffuseShading, this ));
        _keyboardHandler->registerKeyboardShortcut(
            'P', "Disable shading",
            std::bind( &Brayns::Impl::_disableShading, this ));
        _keyboardHandler->registerKeyboardShortcut(
            'r', "Set timestamp to 0",
            std::bind( &Brayns::Impl::_resetTimestamp, this ));
        _keyboardHandler->registerKeyboardShortcut(
            'R', "Set timestamp to infinity",
            std::bind( &Brayns::Impl::_infiniteTimestamp, this ));
        _keyboardHandler->registerKeyboardShortcut(
            'u', "Enable/Disable shadows",
            std::bind( &Brayns::Impl::_toggleShadows, this ));
        _keyboardHandler->registerKeyboardShortcut(
            'U', "Enable/Disable soft shadows",
            std::bind( &Brayns::Impl::_toggleSoftShadows, this ));
        _keyboardHandler->registerKeyboardShortcut(
            't', "Multiply samples per ray by 2",
            std::bind( &Brayns::Impl::_increaseSamplesPerRay, this ));
        _keyboardHandler->registerKeyboardShortcut(
            'T', "Divide samples per ray by 2",
            std::bind( &Brayns::Impl::_decreaseSamplesPerRay, this ));
        _keyboardHandler->registerKeyboardShortcut(
            'y', "Enable/Disable light emitting materials",
            std::bind( &Brayns::Impl::_toggleLightEmittingMaterials, this ));
    }

    void _blackBackground()
    {
        RenderingParameters& renderParams = _parametersManager->getRenderingParameters();
        renderParams.setBackgroundColor( Vector3f( 0.f, 0.f, 0.f ));
    }

    void _grayBackground()
    {
        RenderingParameters& renderParams = _parametersManager->getRenderingParameters();
        renderParams.setBackgroundColor( Vector3f( 0.5f, 0.5f, 0.5f ));
    }

    void _whiteBackground()
    {
        RenderingParameters& renderParams = _parametersManager->getRenderingParameters();
        renderParams.setBackgroundColor( Vector3f( 1.f, 1.f, 1.f ));
    }

    void _defaultRenderer()
    {
        RenderingParameters& renderParams = _parametersManager->getRenderingParameters();
        renderParams.setRenderer( RendererType::basic );
    }

    void _particleRenderer()
    {
        RenderingParameters& renderParams = _parametersManager->getRenderingParameters();
        renderParams.setRenderer( RendererType::particle );
    }

    void _proximityRenderer()
    {
        RenderingParameters& renderParams = _parametersManager->getRenderingParameters();
        renderParams.setRenderer( RendererType::proximity );
    }

    void _simulationRenderer()
    {
        RenderingParameters& renderParams = _parametersManager->getRenderingParameters();
        renderParams.setRenderer( RendererType::simulation );
    }

    void _increaseTimestamp()
    {
        SceneParameters& sceneParams = _parametersManager->getSceneParameters();
        float ts = sceneParams.getTimestamp();
        sceneParams.setTimestamp( ts + 1.f );
    }

    void _decreaseTimestamp()
    {
        SceneParameters& sceneParams = _parametersManager->getSceneParameters();
        float ts = sceneParams.getTimestamp();
        if( ts > 0.f )
            sceneParams.setTimestamp( ts - 1.f );
    }

    void _diffuseShading()
    {
        RenderingParameters& renderParams = _parametersManager->getRenderingParameters();
        renderParams.setShading( ShadingType::diffuse );
    }

    void _electronShading()
    {
        RenderingParameters& renderParams = _parametersManager->getRenderingParameters();
        renderParams.setShading( ShadingType::electron );
    }

    void _disableShading()
    {
        RenderingParameters& renderParams = _parametersManager->getRenderingParameters();
        renderParams.setShading( ShadingType::none );
    }

    void _increaseAmbientOcclusionStrength()
    {
        RenderingParameters& renderParams = _parametersManager->getRenderingParameters();
        float aaStrength = renderParams.getAmbientOcclusionStrength();
        aaStrength += 0.1f;
        if( aaStrength>1.f )
            aaStrength = 1.f;
        renderParams.setAmbientOcclusionStrength( aaStrength );
    }

    void _decreaseAmbientOcclusionStrength()
    {
        RenderingParameters& renderParams = _parametersManager->getRenderingParameters();
        float aaStrength = renderParams.getAmbientOcclusionStrength( );
        aaStrength -= 0.1f;
        if( aaStrength < 0.f )
            aaStrength = 0.f;
        renderParams.setAmbientOcclusionStrength( aaStrength );
    }

    void _resetTimestamp()
    {
        SceneParameters& sceneParams = _parametersManager->getSceneParameters();
        sceneParams.setTimestamp( 0.f );
    }

    void _infiniteTimestamp()
    {
        SceneParameters& sceneParams = _parametersManager->getSceneParameters();
        sceneParams.setTimestamp( std::numeric_limits< size_t >::max() );
    }

    void _toggleShadows()
    {
        RenderingParameters& renderParams = _parametersManager->getRenderingParameters();
        renderParams.setShadows( !renderParams.getShadows() );
    }

    void _toggleSoftShadows()
    {
        RenderingParameters& renderParams = _parametersManager->getRenderingParameters();
        renderParams.setSoftShadows( !renderParams.getSoftShadows() );
    }

    void _increaseSamplesPerRay()
    {
        VolumeParameters& volumeParams = _parametersManager->getVolumeParameters();
        volumeParams.setSamplesPerRay( volumeParams.getSamplesPerRay() * 2 );
        _engine->getScene().commitVolumeData();
    }

    void _decreaseSamplesPerRay()
    {
        VolumeParameters& volumeParams = _parametersManager->getVolumeParameters();
        if( volumeParams.getSamplesPerRay() >= 4 )
            volumeParams.setSamplesPerRay( volumeParams.getSamplesPerRay() / 2 );
        _engine->getScene().commitVolumeData();
    }

    void _toggleLightEmittingMaterials()
    {
        RenderingParameters& renderParams = _parametersManager->getRenderingParameters();
        renderParams.setLightEmittingMaterials( !renderParams.getLightEmittingMaterials() );
    }

    ParametersManagerPtr _parametersManager;
    EnginePtr _engine;
    KeyboardHandlerPtr _keyboardHandler;
    AbstractManipulatorPtr _cameraManipulator;

#if(BRAYNS_USE_DEFLECT || BRAYNS_USE_REST)
    ExtensionPluginFactoryPtr _extensionPluginFactory;
    ExtensionParameters _extensionParameters;
#endif
};

// -------------------------------------------------------------------------------------------------

Brayns::Brayns( int argc, const char **argv )
    : _impl( new Impl( argc, argv ))
{}

Brayns::~Brayns( )
{}

void Brayns::render( const RenderInput& renderInput,
                     RenderOutput& renderOutput )
{
    _impl->render( renderInput, renderOutput );
}

void Brayns::render()
{
    _impl->render();
}

Engine& Brayns::getEngine()
{
    return _impl->getEngine();
}

void Brayns::buildScene()
{
    return _impl->buildScene();
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

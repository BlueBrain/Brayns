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
#include <brayns/common/scene/Scene.h>
#include <brayns/common/renderer/FrameBuffer.h>
#include <brayns/common/renderer/Renderer.h>
#include <brayns/common/camera/Camera.h>
#include <brayns/common/camera/FlyingModeManipulator.h>
#include <brayns/common/camera/InspectCenterManipulator.h>
#include <brayns/common/light/DirectionalLight.h>
#include <brayns/common/simulation/CircuitSimulationHandler.h>
#include <brayns/common/simulation/SpikeSimulationHandler.h>
#include <brayns/common/volume/VolumeHandler.h>
#include <brayns/common/input/KeyboardHandler.h>

#include <plugins/engines/EngineFactory.h>
#include <plugins/engines/common/Engine.h>
#include <plugins/extensions/ExtensionPluginFactory.h>
#include <brayns/parameters/ParametersManager.h>
#include <brayns/io/MorphologyLoader.h>
#include <brayns/io/NESTLoader.h>
#include <brayns/io/ProteinLoader.h>
#include <brayns/io/MeshLoader.h>
#include <brayns/io/TransferFunctionLoader.h>
#include <brayns/io/XYZBLoader.h>

#include <boost/filesystem.hpp>
#include <servus/uri.h>

namespace brayns
{

struct Brayns::Impl
{
    Impl( int argc, const char **argv, Brayns* brayns )
    : _brayns( brayns )
    {
        BRAYNS_INFO << "Parsing command line options" << std::endl;
        _parametersManager.reset( new ParametersManager( ));
        _parametersManager->parse( argc, argv );
        _parametersManager->print( );

        // Get rendering engine
        EngineFactory engineFactory( argc, argv, _parametersManager );
        std::string engineName =
            _parametersManager->getRenderingParameters( ).getEngine();
        _engine = engineFactory.get( engineName );
        if( !_engine )
            throw std::runtime_error( "Unsupported engine: " + engineName );

        // Keyboard handler
        KeyboardHandlerPtr keyboardHandler =
            _engine->getKeyboardHandler();

        // set HDRI skybox if applicable
        const std::string& hdri =
            _parametersManager->getRenderingParameters().getHDRI();
        ScenePtr scene = _engine->getScene();
        if( !hdri.empty() )
            scene->getMaterial(MATERIAL_SKYBOX)->setTexture(TT_DIFFUSE, hdri);

        // Default sun light
        DirectionalLightPtr sunLight( new DirectionalLight(
            DEFAULT_SUN_DIRECTION, DEFAULT_SUN_COLOR, DEFAULT_SUN_INTENSITY ));
        scene->addLight( sunLight );

        // Load data and build geometry
        buildScene();

        // Register keyboard shortcuts
        _registerKeyboardShortcuts();
    }

    void buildScene()
    {
        loadData();
        ScenePtr scene = _engine->getScene();
        scene->commitVolumeData();
        scene->buildEnvironment( );
        scene->buildGeometry( );
        scene->commit( );

        // Set default camera according to scene bounding box
        _setDefaultCamera( );
        setupCameraManipulator( CameraMode::INSPECT_CENTER );

        // Set default epsilon according to scene bounding box
        _setDefaultEpsilon( );

        // Commit changes to the rendering engine
        _engine->commit();
    }

    void loadData()
    {
        ScenePtr scene = _engine->getScene();
        GeometryParameters& geometryParameters = _parametersManager->getGeometryParameters();
        VolumeParameters& volumeParameters = _parametersManager->getVolumeParameters();
        SceneParameters& sceneParameters = _parametersManager->getSceneParameters();

        const std::string& transferFunctionFilename = sceneParameters.getTransferFunctionFilename();
        if( !transferFunctionFilename.empty() )
        {
            TransferFunctionLoader transferFunctionLoader( DEFAULT_TRANSFER_FUNCTION_RANGE );
            transferFunctionLoader.loadFromFile(
                transferFunctionFilename, *scene );
        }
        scene->commitTransferFunctionData();

        if(!geometryParameters.getMorphologyFolder().empty())
            _loadMorphologyFolder();

        if(!geometryParameters.getNESTCircuit().empty())
            _loadNESTCircuit();

        if(!geometryParameters.getPDBFile().empty())
            _loadPDBFile();

        if(!geometryParameters.getPDBFolder().empty())
            _loadPDBFolder();

        if(!geometryParameters.getMeshFolder().empty())
            _loadMeshFolder();

        if(!geometryParameters.getReport().empty())
            _loadCompartmentReport();

        if(!geometryParameters.getCircuitConfiguration().empty() &&
            geometryParameters.getLoadCacheFile().empty())
            _loadCircuitConfiguration();

        if(!geometryParameters.getXYZBFile().empty())
            _loadXYZBFile();

        if(!volumeParameters.getFilename().empty() || !volumeParameters.getFolder().empty())
        {
            scene->getVolumeHandler()->setTimestamp( 0.f );
            const Vector3ui& volumeDimensions = scene->getVolumeHandler()->getDimensions();
            const Vector3f& volumeOffset = scene->getVolumeHandler()->getOffset();
            const Vector3f& volumeElementSpacing = volumeParameters.getElementSpacing();
            Boxf& worldBounds = scene->getWorldBounds();
            worldBounds.merge( Vector3f( 0.f, 0.f, 0.f ));
            worldBounds.merge( volumeOffset + Vector3f( volumeDimensions ) * volumeElementSpacing );
        }

        if( scene->isEmpty() && !scene->getVolumeHandler() )
            scene->buildDefault();
    }

    void render( const RenderInput& renderInput,
                 RenderOutput& renderOutput )
    {
        reshape( renderInput.windowSize );

        _engine->preRender();

        CameraPtr camera = _engine->getCamera();
        camera->set( renderInput.position, renderInput.target, renderInput.up );
        camera->commit();

#if(BRAYNS_USE_DEFLECT || BRAYNS_USE_REST)
        if( !_extensionPluginFactory )
            _intializeExtensionPluginFactory( );
        _extensionPluginFactory->execute( );
#endif

        ScenePtr scene = _engine->getScene();
        FrameBufferPtr frameBuffer = _engine->getFrameBuffer();
        const Vector2i& frameSize = frameBuffer->getSize();

        if( _parametersManager->getRenderingParameters().getHeadLight() )
        {
            LightPtr sunLight = scene->getLight( 0 );
            DirectionalLight* sun =
                dynamic_cast< DirectionalLight* > ( sunLight.get() );
            if( sun )
            {
                sun->setDirection( camera->getTarget() - camera->getPosition() );
                scene->commitLights();
            }
        }

        _render( );

        uint8_t* colorBuffer = frameBuffer->getColorBuffer( );
        if( colorBuffer )
        {
            const size_t size = frameSize.x( ) * frameSize.y( ) * frameBuffer->getColorDepth( );
            renderOutput.colorBuffer.assign( colorBuffer, colorBuffer + size );
        }

        float* depthBuffer = frameBuffer->getDepthBuffer( );
        if( depthBuffer )
        {
            const size_t size = frameSize.x( ) * frameSize.y( );
            renderOutput.depthBuffer.assign( depthBuffer, depthBuffer + size );
        }

        _engine->postRender();
    }

    void render()
    {
        ScenePtr scene = _engine->getScene();
        CameraPtr camera = _engine->getCamera();
        FrameBufferPtr frameBuffer = _engine->getFrameBuffer();
        const Vector2i& frameSize = frameBuffer->getSize();

        _engine->preRender();

#if(BRAYNS_USE_DEFLECT || BRAYNS_USE_REST)
        if( !_extensionPluginFactory )
            _intializeExtensionPluginFactory( );
        _extensionPluginFactory->execute( );
#endif
        if( _parametersManager->getRenderingParameters().getHeadLight() )
        {
            LightPtr sunLight = scene->getLight( 0 );
            DirectionalLight* sun =
                dynamic_cast< DirectionalLight* > ( sunLight.get() );
            if( sun )
            {
                sun->setDirection( camera->getTarget() - camera->getPosition() );
                scene->commitLights();
            }
        }

        camera->commit();
        _render( );

        _engine->postRender();

        const Vector2ui windowSize = _parametersManager
            ->getApplicationParameters()
            .getWindowSize();
        if( windowSize != frameSize )
            reshape(windowSize);
    }

#if(BRAYNS_USE_DEFLECT || BRAYNS_USE_REST)
    void _intializeExtensionPluginFactory( )
    {
        _extensionParameters.parametersManager = _parametersManager;
        _extensionParameters.engine = _engine;

        _extensionPluginFactory.reset( new ExtensionPluginFactory( *_brayns ));
    }
#endif

    void reshape( const Vector2ui& frameSize )
    {
        CameraPtr camera = _engine->getCamera();
        FrameBufferPtr frameBuffer = _engine->getFrameBuffer();

        if( frameBuffer->getSize() == frameSize )
            return;

        frameBuffer->resize( frameSize );
        camera->setAspectRatio(
            static_cast< float >( frameSize.x()) /
            static_cast< float >( frameSize.y()));
    }

    void setMaterials(
        const MaterialType materialType,
        const size_t nbMaterials )
    {
        ScenePtr scene = _engine->getScene();
        scene->setMaterials( materialType, nbMaterials );
        scene->commit( );
    }

    void commit( )
    {
        _engine->getFrameBuffer()->clear();
        _engine->commit();
    }

    ParametersManager& getParametersManager( )
    {
        return *_parametersManager;
    }

    Scene& getScene( )
    {
        return *_engine->getScene();
    }

    Engine& getEngine( )
    {
        return *_engine;
    }

    Camera& getCamera( )
    {
        return *_engine->getCamera();
    }

    AbstractManipulator& getCameraManipulator()
    {
        return *_cameraManipulator;
    }

    Renderer& getRenderer( )
    {
        return *_engine->getRenderer();
    }

    FrameBuffer& getFrameBuffer( )
    {
        return *_engine->getFrameBuffer();
    }

    KeyboardHandler& getKeyboardHandler()
    {
        return *_engine->getKeyboardHandler();
    }

    void _registerKeyboardShortcuts()
    {
        KeyboardHandler& keyboardHandler =  *_engine->getKeyboardHandler();
        keyboardHandler.registerKeyboardShortcut(
            '0', "Black background",
             std::bind( &Brayns::Impl::_blackBackground, this ));
        keyboardHandler.registerKeyboardShortcut(
            '1', "Gray background",
            std::bind( &Brayns::Impl::_grayBackground, this ));
        keyboardHandler.registerKeyboardShortcut(
            '2', "White background",
            std::bind( &Brayns::Impl::_whiteBackground, this ));
        keyboardHandler.registerKeyboardShortcut(
            '6', "Default renderer",
            std::bind( &Brayns::Impl::_defaultRenderer, this ));
        keyboardHandler.registerKeyboardShortcut(
            '7', "Particle renderer",
            std::bind( &Brayns::Impl::_particleRenderer, this ));
        keyboardHandler.registerKeyboardShortcut(
            '8', "Proximity renderer",
            std::bind( &Brayns::Impl::_proximityRenderer, this ));
        keyboardHandler.registerKeyboardShortcut(
            '9', "Simulation renderer",
            std::bind( &Brayns::Impl::_simulationRenderer, this ));
        keyboardHandler.registerKeyboardShortcut(
            '[', "Decrease timestamp by 1",
            std::bind( &Brayns::Impl::_decreaseTimestamp, this ));
        keyboardHandler.registerKeyboardShortcut(
            ']', "Increase timestamp by 1",
            std::bind( &Brayns::Impl::_increaseTimestamp, this ));
        keyboardHandler.registerKeyboardShortcut(
            'e', "Enable eletron shading",
            std::bind( &Brayns::Impl::_electronShading, this ));
        keyboardHandler.registerKeyboardShortcut(
            'f', "Enable fly mode",
            [this](){ setupCameraManipulator( CameraMode::FLYING ); });
        keyboardHandler.registerKeyboardShortcut(
            'i', "Enable inspect mode",
            [this](){ setupCameraManipulator( CameraMode::INSPECT_CENTER ); });
        keyboardHandler.registerKeyboardShortcut(
            'o', "Decrease ambient occlusion strength",
            std::bind( &Brayns::Impl::_decreaseAmbientOcclusionStrength, this ));
        keyboardHandler.registerKeyboardShortcut(
            'O', "Increase ambient occlusion strength",
            std::bind( &Brayns::Impl::_increaseAmbientOcclusionStrength, this ));
        keyboardHandler.registerKeyboardShortcut(
            'p', "Enable diffuse shading",
            std::bind( &Brayns::Impl::_diffuseShading, this ));
        keyboardHandler.registerKeyboardShortcut(
            'P', "Disable shading",
            std::bind( &Brayns::Impl::_disableShading, this ));
        keyboardHandler.registerKeyboardShortcut(
            'r', "Set timestamp to 0",
            std::bind( &Brayns::Impl::_resetTimestamp, this ));
        keyboardHandler.registerKeyboardShortcut(
            'R', "Set timestamp to infinity",
            std::bind( &Brayns::Impl::_infiniteTimestamp, this ));
        keyboardHandler.registerKeyboardShortcut(
            'u', "Enable/Disable shadows",
            std::bind( &Brayns::Impl::_toggleShadows, this ));
        keyboardHandler.registerKeyboardShortcut(
            'U', "Enable/Disable soft shadows",
            std::bind( &Brayns::Impl::_toggleSoftShadows, this ));
        keyboardHandler.registerKeyboardShortcut(
            't', "Multiply samples per ray by 2",
            std::bind( &Brayns::Impl::_increaseSamplesPerRay, this ));
        keyboardHandler.registerKeyboardShortcut(
            'T', "Divide samples per ray by 2",
            std::bind( &Brayns::Impl::_decreaseSamplesPerRay, this ));
        keyboardHandler.registerKeyboardShortcut(
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
        renderParams.setRenderer( "exobj" );
    }

    void _particleRenderer()
    {
        RenderingParameters& renderParams = _parametersManager->getRenderingParameters();
        renderParams.setRenderer( "particlerenderer" );
    }

    void _proximityRenderer()
    {
        RenderingParameters& renderParams = _parametersManager->getRenderingParameters();
        renderParams.setRenderer( "proximityrenderer" );
    }

    void _simulationRenderer()
    {
        RenderingParameters& renderParams = _parametersManager->getRenderingParameters();
        renderParams.setRenderer( "simulationrenderer" );
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
        _engine->getScene()->commitVolumeData();
    }

    void _decreaseSamplesPerRay()
    {
        VolumeParameters& volumeParams = _parametersManager->getVolumeParameters();
        if( volumeParams.getSamplesPerRay() >= 4 )
            volumeParams.setSamplesPerRay( volumeParams.getSamplesPerRay() / 2 );
        _engine->getScene()->commitVolumeData();
    }

    void _toggleLightEmittingMaterials()
    {
        RenderingParameters& renderParams = _parametersManager->getRenderingParameters();
        renderParams.setLightEmittingMaterials( !renderParams.getLightEmittingMaterials() );
    }

    void setupCameraManipulator( const CameraMode mode )
    {
        _cameraManipulator.reset(); // deregister previous keyboard handlers

        auto& camera = getCamera();
        auto& handler = getKeyboardHandler();

        switch( mode )
        {
        case CameraMode::FLYING:
            _cameraManipulator.reset( new FlyingModeManipulator( camera,
                                                                 handler ));
            break;
        case CameraMode::INSPECT_CENTER:
            _cameraManipulator.reset( new InspectCenterManipulator( camera,
                                                                    handler ));
            break;
        };
    }

private:

    void _render( )
    {

        _engine->setActiveRenderer(
            _parametersManager->getRenderingParameters().getRenderer());
        _engine->render();
    }

    void _setDefaultCamera()
    {
        ScenePtr scene = _engine->getScene();
        CameraPtr camera = _engine->getCamera();
        FrameBufferPtr frameBuffer = _engine->getFrameBuffer();
        const Vector2i& frameSize = frameBuffer->getSize();

        const Boxf& worldBounds = scene->getWorldBounds();
        const Vector3f& target = worldBounds.getCenter();
        const Vector3f& diag = worldBounds.getSize();
        Vector3f position = target;
        position.z() -= 1.5f * diag.find_max();

        const Vector3f up = Vector3f( 0.f, 1.f, 0.f );
        camera->setInitialState( position, target, up );
        camera->setAspectRatio(
            static_cast< float >( frameSize.x()) /
            static_cast< float >( frameSize.y()));

        BRAYNS_INFO << "World bounding box: " << worldBounds << std::endl;
        BRAYNS_INFO << "World center      : " << worldBounds.getCenter() << std::endl;
    }

    void _setDefaultEpsilon()
    {
        float epsilon = _parametersManager->getRenderingParameters().getEpsilon();
        if( epsilon == 0.f )
        {
            ScenePtr scene = _engine->getScene();
            const Vector3f& worldBoundsSize = scene->getWorldBounds().getSize();
            epsilon = worldBoundsSize.length() / 1e6f;
            BRAYNS_INFO << "Default epsilon: " << epsilon << std::endl;
            _parametersManager->getRenderingParameters().setEpsilon( epsilon );
        }
    }

    /**
        Loads data from SWC and H5 files located in the folder specified in the
        geometry parameters (command line parameter --morphology-folder)
    */
    void _loadMorphologyFolder()
    {
        GeometryParameters& geometryParameters =
            _parametersManager->getGeometryParameters();
        const boost::filesystem::path& folder =
            geometryParameters.getMorphologyFolder( );
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
                            uri, fileIndex++, *_engine->getScene()))
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
        const GeometryParameters& geometryParameters =
            _parametersManager->getGeometryParameters();
        const std::string& circuit( geometryParameters.getNESTCircuit( ));
        if( !circuit.empty( ))
        {
            size_t nbMaterials;
            NESTLoader loader( geometryParameters );
            loader.importCircuit( circuit, *_engine->getScene(), nbMaterials );

            loader.importSpikeReport(
                geometryParameters.getNESTReport(), *_engine->getScene( ));

            const std::string& cacheFile( geometryParameters.getNESTCacheFile( ));
            if( !cacheFile.empty( ))
            {
                SpikeSimulationHandlerPtr simulationHandler( new SpikeSimulationHandler( ));
                simulationHandler->attachSimulationToCacheFile( cacheFile );
                _engine->getScene()->setSimulationHandler( simulationHandler );
            }

            SceneParameters& sceneParameters =
                            _parametersManager->getSceneParameters();
            const std::string& transferFunctionFilename =
                sceneParameters.getTransferFunctionFilename();
            if( !transferFunctionFilename.empty() )
            {
                TransferFunctionLoader transferFunctionLoader( brayns::Vector2f( 0, nbMaterials ));
                transferFunctionLoader.loadFromFile(
                    transferFunctionFilename, *_engine->getScene( ));
                _engine->getScene()->commitTransferFunctionData();
            }

        }
    }

    /**
        Loads data from a PDB file (command line parameter --pdb-file)
    */
    void _loadPDBFolder()
    {
        // Load PDB File
        GeometryParameters& geometryParameters =
            _parametersManager->getGeometryParameters();
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
    void _loadPDBFile( const std::string& filename = "" )
    {
        // Load PDB File
        GeometryParameters& geometryParameters = _parametersManager->getGeometryParameters();
        std::string pdbFile = filename;
        if( pdbFile == "" )
        {
            pdbFile = geometryParameters.getPDBFile();
            BRAYNS_INFO << "Loading PDB file " << pdbFile << std::endl;
        }
        ProteinLoader proteinLoader( geometryParameters );
        if( !proteinLoader.importPDBFile( pdbFile,
                                          Vector3f( 0, 0, 0 ), 0,
                                          *_engine->getScene()))
        {
            BRAYNS_ERROR << "Failed to import " << pdbFile << std::endl;
        }

        ScenePtr scene = _engine->getScene();
        for( size_t i = 0; i < scene->getMaterials().size( ); ++i )
        {
            float r,g,b;
            proteinLoader.getMaterialKd( i, r, g, b );
            MaterialPtr material = scene->getMaterials()[i];
            material->setColor( Vector3f( r, g, b ));
        }
    }

    /**
        Loads data from a XYZR file (command line parameter --xyzr-file)
    */
    void _loadXYZBFile()
    {
        // Load XYZB File
        GeometryParameters& geometryParameters =
            _parametersManager->getGeometryParameters();
        BRAYNS_INFO << "Loading XYZB file " << geometryParameters.getXYZBFile()
                    << std::endl;
        XYZBLoader xyzbLoader( geometryParameters );
        if( !xyzbLoader.importFromBinaryFile(
            geometryParameters.getXYZBFile(), *_engine->getScene()))
            BRAYNS_ERROR << "Failed to import " << geometryParameters.getXYZBFile() << std::endl;
    }

    /**
        Loads data from mesh files located in the folder specified in the
        geometry parameters (command line parameter --mesh-folder)
    */
    void _loadMeshFolder()
    {
#ifdef BRAYNS_USE_ASSIMP
        GeometryParameters& geometryParameters =
            _parametersManager->getGeometryParameters();
        const boost::filesystem::path& folder =
            geometryParameters.getMeshFolder( );
        BRAYNS_INFO << "Loading meshes from " << folder << std::endl;
        MeshLoader meshLoader;
        size_t meshIndex = 0;

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
                    ScenePtr scene = _engine->getScene();
                    MeshContainer MeshContainer =
                    {
                        scene->getTriangleMeshes(), scene->getMaterials(),
                        scene->getWorldBounds()
                    };

                    size_t material =
                        geometryParameters.getColorScheme() == CS_NEURON_BY_ID ?
                        meshIndex % (NB_MAX_MATERIALS - NB_SYSTEM_MATERIALS) :
                        NO_MATERIAL;

                    MeshQuality quality;
                    switch( geometryParameters.getGeometryQuality() )
                    {
                    case GQ_QUALITY: quality = MQ_QUALITY; break;
                    case GQ_MAX_QUALITY: quality = MQ_MAX_QUALITY; break;
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
#endif
    }

    /**
        Loads morphologies from circuit configuration (command line parameter
        --circuit-configuration)
    */
    void _loadCircuitConfiguration()
    {
        GeometryParameters& geometryParameters =
            _parametersManager->getGeometryParameters();
        const std::string& filename =
            geometryParameters.getCircuitConfiguration( );
        const std::string& target =
            geometryParameters.getTarget( );

        BRAYNS_INFO << "Loading circuit configuration from " <<
            filename << std::endl;
        const std::string& report =
            geometryParameters.getReport( );
        MorphologyLoader morphologyLoader( geometryParameters );
        const servus::URI uri( filename );
        if( report.empty() )
            morphologyLoader.importCircuit( uri, target, *_engine->getScene());
        else
            morphologyLoader.importCircuit(
                uri, target, report, *_engine->getScene());
    }

    /**
        Loads compartment report from circuit configuration (command line
        parameter --report)
        @return the number of simulation frames loaded
    */
    void _loadCompartmentReport()
    {
        GeometryParameters& geometryParameters =
            _parametersManager->getGeometryParameters();
        const std::string& filename =
            geometryParameters.getCircuitConfiguration( );
        const std::string& target =
            geometryParameters.getTarget( );
        const std::string& report =
            geometryParameters.getReport( );
        BRAYNS_INFO << "Loading compartment report from " << filename << std::endl;
        MorphologyLoader morphologyLoader( geometryParameters );
        const servus::URI uri( filename );
        if( morphologyLoader.importSimulationData( uri, target, report,
                                                   *_engine->getScene( )))
        {
            SceneParameters& sceneParameters =
                _parametersManager->getSceneParameters();
            const std::string& transferFunctionFilename =
                sceneParameters.getTransferFunctionFilename();
            if( !transferFunctionFilename.empty() )
            {
                TransferFunctionLoader transferFunctionLoader( DEFAULT_TRANSFER_FUNCTION_RANGE );
                transferFunctionLoader.loadFromFile(
                    transferFunctionFilename, *_engine->getScene() );
                _engine->getScene()->commitTransferFunctionData();
            }
        }
    }

    void _buildDefaultScene()
    {
        ScenePtr scene = _engine->getScene();
        scene->buildDefault();
        scene->buildGeometry();
    }

    ParametersManagerPtr _parametersManager;
    EnginePtr _engine;
    Brayns* _brayns;

#if(BRAYNS_USE_DEFLECT || BRAYNS_USE_REST)
    ExtensionPluginFactoryPtr _extensionPluginFactory;
    ExtensionParameters _extensionParameters;
#endif

    std::unique_ptr<AbstractManipulator> _cameraManipulator;
};

// -------------------------------------------------------------------------------------------------

Brayns::Brayns( int argc, const char **argv )
    : _impl( new Impl( argc, argv, this ))
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

void Brayns::reshape( const Vector2ui& size )
{
    _impl->reshape( size );
}

void Brayns::commit( )
{
    _impl->commit( );
}

ParametersManager& Brayns::getParametersManager( )
{
    return _impl->getParametersManager( );
}

void Brayns::setMaterials(
    const MaterialType materialType,
    const size_t nbMaterials )
{
    _impl->setMaterials( materialType, nbMaterials );
}

Engine& Brayns::getEngine( )
{
    return _impl->getEngine( );
}

Scene& Brayns::getScene( )
{
    return _impl->getScene( );
}

Renderer& Brayns::getRenderer( )
{
    return _impl->getRenderer( );
}

Camera& Brayns::getCamera( )
{
    return _impl->getCamera( );
}

AbstractManipulator& Brayns::getCameraManipulator()
{
    return _impl->getCameraManipulator();
}

FrameBuffer& Brayns::getFrameBuffer( )
{
    return _impl->getFrameBuffer( );
}

KeyboardHandler& Brayns::getKeyboardHandler()
{
    return _impl->getKeyboardHandler( );
}

void Brayns::buildScene()
{
    _impl->buildScene();
}

}

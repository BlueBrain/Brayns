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
#include <brayns/common/light/DirectionalLight.h>

#include <plugins/extensions/ExtensionPluginFactory.h>
#include <brayns/parameters/ParametersManager.h>
#include <brayns/io/MorphologyLoader.h>
#include <brayns/io/ProteinLoader.h>
#include <brayns/io/MeshLoader.h>

// OSPray specific -> Must be changed to a dynamic plugin
#include <plugins/renderers/ospray/render/OSPRayRenderer.h>
#include <plugins/renderers/ospray/render/OSPRayScene.h>
#include <plugins/renderers/ospray/render/OSPRayFrameBuffer.h>
#include <plugins/renderers/ospray/render/OSPRayCamera.h>

#include <boost/filesystem.hpp>
#include <servus/uri.h>

namespace brayns
{

struct Brayns::Impl
{
    Impl( int argc, const char **argv )
         : _frameSize( 0, 0 )
         , _sceneModified( true )
    {
        // Ignore errors from multiple calls to init from same process,
        // aka from unit tests. Remove this once ospShutdown is provided.
        try { ospInit( &argc, argv ); }
        catch( const std::runtime_error& ) {}

        BRAYNS_INFO << "Parsing command line options" << std::endl;
        _parametersManager.reset( new ParametersManager( ));
        _parametersManager->parse( argc, argv );
        _parametersManager->print( );

        _frameSize =
            _parametersManager->getApplicationParameters( ).getWindowSize( );

        BRAYNS_INFO << "Initialize renderers" << std::endl;
        // Should be implemented with a plugin factory
        _activeRenderer =
            _parametersManager->getRenderingParameters().getRenderer();

        const strings& renderers =
            _parametersManager->getRenderingParameters().getRenderers();

        Renderers renderersForScene;
        for( std::string renderer: renderers )
        {
            _renderers[renderer].reset(
                new OSPRayRenderer( renderer, *_parametersManager ));
            renderersForScene.push_back( _renderers[renderer] );
        }

        BRAYNS_INFO << "Initialize scene" << std::endl;
        _scene.reset( new OSPRayScene( renderersForScene,
            _parametersManager->getSceneParameters(),
            _parametersManager->getGeometryParameters()));

        _scene->setMaterials( MT_DEFAULT, NB_MAX_MATERIALS );

        // set HDRI skybox if applicable
        const std::string& hdri =
            _parametersManager->getRenderingParameters().getHDRI();
        if( !hdri.empty() )
            _scene->getMaterial(MATERIAL_SKYBOX)->setTexture(TT_DIFFUSE, hdri);

        // Default sun light
        DirectionalLightPtr sunLight( new DirectionalLight(
            Vector3f( 0.f, 0.f, 1.f ), Vector3f( 1.f, 1.f, 1.f ), 1.f ));
        _scene->addLight( sunLight );

        loadData( );
        _scene->buildEnvironment( );
        _scene->buildGeometry( );
        _scene->commit( );

        _frameBuffer.reset( new OSPRayFrameBuffer( _frameSize, FBF_RGBA_I8 ));
        _camera.reset( new OSPRayCamera(
            _parametersManager->getRenderingParameters().getCameraType( )));
        _setDefaultCamera( );

        for( std::string renderer: renderers )
        {
            _renderers[renderer]->setScene( _scene );
            _renderers[renderer]->setCamera( _camera );
            _renderers[renderer]->commit( );
        }
    }

    ~Impl( )
    {
    }

    void loadData()
    {
        GeometryParameters& geometryParameters =
            _parametersManager->getGeometryParameters();
        if(!geometryParameters.getMorphologyFolder().empty())
            _loadMorphologyFolder();

        if(!geometryParameters.getPDBFile().empty())
            _loadPDBFile();

        if(!geometryParameters.getMeshFolder().empty())
            _loadMeshFolder();

        if(!geometryParameters.getReport().empty())
            _loadCompartmentReport();

        if(!geometryParameters.getCircuitConfiguration().empty() &&
            geometryParameters.getLoadCacheFile().empty())
            _loadCircuitConfiguration();

    }

    void render( const RenderInput& renderInput,
                 RenderOutput& renderOutput )
    {
        reshape( renderInput.windowSize );

        _frameBuffer->map( );
        _camera->set(
            renderInput.position, renderInput.target, renderInput.up );

#if(BRAYNS_USE_DEFLECT || BRAYNS_USE_REST)
        if( !_extensionPluginFactory )
            _intializeExtensionPluginFactory( );
        _extensionPluginFactory->execute( );
#endif

        if( _parametersManager->getRenderingParameters().getSunOnCamera() )
        {
            LightPtr sunLight = _scene->getLight( 0 );
            DirectionalLight* sun =
                dynamic_cast< DirectionalLight* > ( sunLight.get() );
            if( sun )
            {
                sun->setDirection( _camera->getTarget() - _camera->getPosition() );
                _scene->commitLights();
            }
        }

        _camera->commit();
        _render( );

        uint8_t* colorBuffer = _frameBuffer->getColorBuffer( );
        size_t size =
            _frameSize.x( ) * _frameSize.y( ) * _frameBuffer->getColorDepth( );
        renderOutput.colorBuffer.assign( colorBuffer, colorBuffer + size );

        float* depthBuffer = _frameBuffer->getDepthBuffer( );
        size = _frameSize.x( ) * _frameSize.y( );
        renderOutput.depthBuffer.assign( depthBuffer, depthBuffer + size );

        _frameBuffer->unmap( );
    }

    void render()
    {
        _frameBuffer->map( );

#if(BRAYNS_USE_DEFLECT || BRAYNS_USE_REST)
        if( !_extensionPluginFactory )
            _intializeExtensionPluginFactory( );
        _extensionPluginFactory->execute( );
#endif
        if( _parametersManager->getRenderingParameters().getSunOnCamera() )
        {
            LightPtr sunLight = _scene->getLight( 0 );
            DirectionalLight* sun =
                dynamic_cast< DirectionalLight* > ( sunLight.get() );
            if( sun )
            {
                sun->setDirection( _camera->getTarget() - _camera->getPosition() );
                _scene->commitLights();
            }
        }

        _camera->commit();
        _render( );

        _frameBuffer->unmap( );

        const Vector2ui windowSize = _parametersManager
            ->getApplicationParameters()
            .getWindowSize();
        if( windowSize != _frameBuffer->getSize( ))
            reshape(windowSize);
    }

#if(BRAYNS_USE_DEFLECT || BRAYNS_USE_REST)
    void _intializeExtensionPluginFactory( )
    {
        _extensionParameters.parametersManager = _parametersManager;
        _extensionParameters.scene = _scene;
        _extensionParameters.renderer = _renderers[_activeRenderer];
        _extensionParameters.camera = _camera;
        _extensionParameters.frameBuffer = _frameBuffer;

        _extensionPluginFactory.reset( new ExtensionPluginFactory(
            _parametersManager->getApplicationParameters( ),
            _extensionParameters ));
    }
#endif

    void reshape( const Vector2ui& frameSize )
    {
        if( _frameBuffer->getSize() == frameSize )
            return;

        _frameSize = frameSize;
        _frameBuffer->resize( _frameSize );
        _camera->setAspectRatio(
            static_cast< float >( _frameSize.x()) /
            static_cast< float >( _frameSize.y()));
    }

    void setMaterials(
        const MaterialType materialType,
        const size_t nbMaterials )
    {
        _scene->setMaterials( materialType, nbMaterials );
        _scene->commit( );
    }

    void commit( )
    {
        _frameBuffer->clear( );
        const strings& renderers =
            _parametersManager->getRenderingParameters().getRenderers();
        for( std::string renderer: renderers )
            _renderers[renderer]->commit( );
        _camera->commit( );
    }

    ParametersManager& getParametersManager( )
    {
        return *_parametersManager;
    }

    Scene& getScene( )
    {
        return *_scene;
    }

    Camera& getCamera( )
    {
        return *_camera;
    }

    FrameBuffer& getFrameBuffer( )
    {
        return *_frameBuffer;
    }

private:
    void _render( )
    {
        const std::string& renderer =
            _parametersManager->getRenderingParameters().getRenderer();
        if( _activeRenderer != renderer )
        {
            _activeRenderer = renderer;
#if(BRAYNS_USE_DEFLECT || BRAYNS_USE_REST)
            _extensionParameters.renderer = _renderers[_activeRenderer];
#endif
        }
        _renderers[_activeRenderer]->render( _frameBuffer );
    }

    void _setDefaultCamera()
    {
        const Boxf worldBounds = _scene->getWorldBounds( );
        const Vector3f target = worldBounds.getCenter( );
        const Vector3f diag   = worldBounds.getSize( );
        Vector3f position = target;
        position.z( ) -= diag.z( );

        const Vector3f up  = Vector3f(0.f,1.f,0.f);
        _camera->setInitialState(position, target, up);
        _camera->setAspectRatio(
            static_cast< float >( _frameSize.x()) /
            static_cast< float >( _frameSize.y()));
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
        if( boost::filesystem::exists(folder) &&
            boost::filesystem::is_directory(folder))
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
                            uri, fileIndex++, *_scene))
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
        Loads data from a PDB file (command line parameter --pdb-file)
    */
    void _loadPDBFile()
    {
        // Load PDB File
        GeometryParameters& geometryParameters =
            _parametersManager->getGeometryParameters();
        BRAYNS_INFO << "Loading PDB file " << geometryParameters.getPDBFile()
                    << std::endl;
        ProteinLoader proteinLoader( geometryParameters );
        if( !proteinLoader.importPDBFile( geometryParameters.getPDBFile(),
                                          Vector3f( 0, 0, 0 ), 0, *_scene ))
        {
            BRAYNS_ERROR << "Failed to import "
                         << geometryParameters.getPDBFile() << std::endl;
        }

        for( size_t i = 0; i < _scene->getMaterials().size( ); ++i )
        {
            float r,g,b;
            proteinLoader.getMaterialKd( i, r, g, b );
            MaterialPtr material = _scene->getMaterials()[i];
            material->setColor( Vector3f( r, g, b ));
        }
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
        if( boost::filesystem::exists(folder) &&
            boost::filesystem::is_directory(folder))
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
                        _scene->getTriangleMeshes(), _scene->getMaterials(),
                        _scene->getWorldBounds()
                    };
                    if(!meshLoader.importMeshFromFile(
                        filename, MeshContainer, MQ_FAST, NO_MATERIAL ))
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
            morphologyLoader.importCircuit( uri, target, *_scene );
        else
            morphologyLoader.importCircuit(
                uri, target, report, *_scene );
    }

    /**
        Loads compartment report from circuit configuration (command line
        parameter --report)
        @return the number of simulation frames loaded
    */
    size_t _loadCompartmentReport()
    {
        GeometryParameters& geometryParameters =
            _parametersManager->getGeometryParameters();
        const std::string& filename =
            geometryParameters.getCircuitConfiguration( );
        const std::string& target =
            geometryParameters.getTarget( );
        const std::string& report =
            geometryParameters.getReport( );
        BRAYNS_INFO << "Loading compartment report from " <<
            filename << std::endl;
        MorphologyLoader morphologyLoader( geometryParameters );
        const servus::URI uri( filename );
        return morphologyLoader.importSimulationIntoTexture(
            uri, target, report, *_scene );
    }

    ParametersManagerPtr _parametersManager;

    ScenePtr _scene;
    CameraPtr _camera;
    std::string _activeRenderer;
    RendererMap _renderers;
    FrameBufferPtr _frameBuffer;

    Vector2i _frameSize;
    float _timestamp;

    bool _rendering;
    bool _sceneModified;

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

Scene& Brayns::getScene( )
{
    return _impl->getScene( );
}

Camera& Brayns::getCamera( )
{
    return _impl->getCamera( );
}

FrameBuffer& Brayns::getFrameBuffer( )
{
    return _impl->getFrameBuffer( );
}

}

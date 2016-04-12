/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Jafet Villafranca <jafet.villafrancadiaz@epfl.ch>
 *
 * This file is part of BRayns
 */

#include <brayns/Brayns.h>

#include <brayns/common/log.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/common/light/DirectionalLight.h>

#include <plugins/extensions/ExtensionPluginFactory.h>
#include <brayns/common/parameters/ParametersManager.h>

// OSPray specific -> Must be changed to a dynamic plugin
#include <plugins/renderers/ospray/render/OSPRayRenderer.h>
#include <plugins/renderers/ospray/render/OSPRayScene.h>
#include <plugins/renderers/ospray/render/OSPRayFrameBuffer.h>
#include <plugins/renderers/ospray/render/OSPRayCamera.h>

namespace brayns
{

struct Brayns::Impl
{
    Impl( int argc, const char **argv )
         : _frameSize( 0, 0 )
         , _sceneModified( true )
    {
        ospInit( &argc, argv );

        _parametersManager.reset( new ParametersManager( ));
        _parametersManager->parse( argc, argv );
        _parametersManager->print( );

        _frameSize =
            _parametersManager->getApplicationParameters( ).getWindowSize( );

        // Should be implemented with a plugin factory
        _activeRenderer =
            _parametersManager->getRenderingParameters().getRenderer();

        const strings& renderers =
            _parametersManager->getRenderingParameters().getRenderers();

        for( std::string renderer: renderers )
            _renderers[renderer].reset(
                new OSPRayRenderer( renderer, *_parametersManager ));

        _scene.reset( new OSPRayScene( _renderers,
            _parametersManager->getGeometryParameters( )));

        _scene->setMaterials( MT_DEFAULT, DEFAULT_NB_MATERIALS );

        // Default sun light
        DirectionalLightPtr sunLight( new DirectionalLight(
            Vector3f( 1.f, -1.f, 1.f ), Vector3f( 1.f, 1.f, 1.f ), 1.f ));
        _scene->addLight( sunLight );

        _scene->loadData( );
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

    void render( const RenderInput& renderInput,
                 RenderOutput& renderOutput )
    {
        reshape( renderInput.windowSize );

        _frameBuffer->map( );
        _camera->set(
            renderInput.position, renderInput.target, renderInput.up );

        if( !_extensionPluginFactory )
            _intializeExtensionPluginFactory( );
        _extensionPluginFactory->execute( );

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

        if( !_extensionPluginFactory )
            _intializeExtensionPluginFactory( );
        _extensionPluginFactory->execute( );

        _camera->commit();
        _render( );

        _frameBuffer->unmap( );
    }

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
        _renderers[_activeRenderer]->commit( );
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
            _extensionParameters.renderer = _renderers[_activeRenderer];
        }
        _renderers[_activeRenderer]->render( _frameBuffer );
    }

    void _setDefaultCamera()
    {
        Boxf worldBounds = _scene->getWorldBounds( );
        Vector3f target = worldBounds.getCenter( );
        Vector3f diag   = worldBounds.getSize( );
        diag = max(diag,Vector3f(0.3f*diag.length( )));
        Vector3f position = target;
        position.z( ) -= diag.z( );

        Vector3f up  = Vector3f(0.f,1.f,0.f);
        _camera->set(position, target, up);
        _camera->setAspectRatio(
            static_cast< float >( _frameSize.x()) /
            static_cast< float >( _frameSize.y()));
    }

    ParametersManagerPtr _parametersManager;

    ScenePtr _scene;
    CameraPtr _camera;
    std::string _activeRenderer;
    RendererMap _renderers;
    FrameBufferPtr _frameBuffer;

    Vector2i _frameSize;

    bool _rendering;
    bool _sceneModified;

    ExtensionPluginFactoryPtr _extensionPluginFactory;
    ExtensionParameters _extensionParameters;
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

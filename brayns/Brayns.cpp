/* Copyright (c) 2011-2014, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Jafet Villafranca <jafet.villafrancadiaz@epfl.ch>
 *
 * This file is part of BRayns
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

// Plugins
#include <plugins/extensions/ExtensionController.h>

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
         , _rendering( false )
         , _sceneModified( true )
    {
        _parametersManager.parse( argc, argv );
        _parametersManager.print( );

        _frameSize =
            _parametersManager.getApplicationParameters( ).getWindowSize( );

        // Should be implemented with a plugin factory
        _renderer.reset(
            new OSPRayRenderer( _parametersManager.getRenderingParameters( )));

        _scene.reset( new OSPRayScene( _renderer,
            _parametersManager.getGeometryParameters( )));

        _scene->setMaterials( MT_DEFAULT, 200 );

        // Default sun light
        DirectionalLightPtr sunLight( new DirectionalLight(
            Vector3f( 1.f, -1.f, 1.f ), Vector3f( 1.f, 1.f, 1.f ), 1.f ));
        _scene->addLight( sunLight );

        _scene->loadData( );
        _scene->buildEnvironment( );
        _scene->buildGeometry( );

        _scene->commit( );

        _renderer->setScene( _scene );

        _frameBuffer.reset( new OSPRayFrameBuffer( _frameSize, FBF_RGBA_I8 ));
        _camera.reset( new OSPRayCamera( CT_PERSPECTIVE ));
        _renderer->setCamera( _camera );
        _renderer->commit( );
    }

    ~Impl( )
    {
    }

    void render( const RenderInput& renderInput,
                 RenderOutput& renderOutput )
    {
        _camera->set(
            renderInput.position, renderInput.target, renderInput.up );

#if(BRAYNS_USE_REST || BRAYNS_USE_DEFLECT)
        if( !_extensionController )
        {
            ExtensionParameters extensionParameters = {
                _scene, _renderer, _camera, _frameBuffer };

            _extensionController.reset( new ExtensionController(
                _parametersManager.getApplicationParameters( ),
                extensionParameters ));
        }
        _extensionController->execute( );
#endif
        _render( );

        uint8_t* colorBuffer = _frameBuffer->getColorBuffer( );
        size_t size =
            _frameSize.x( ) * _frameSize.y( ) *
            sizeof(uint8_t) * _frameBuffer->getColorDepth( );
        renderOutput.colorBuffer.assign( colorBuffer, colorBuffer + size );

        float* depthBuffer = _frameBuffer->getDepthBuffer( );
        size = _frameSize.x( )*_frameSize.y( )*sizeof( float );
        renderOutput.depthBuffer.assign( depthBuffer, depthBuffer + size );
    }

    void reshape( const Vector2ui& frameSize )
    {
        _frameSize = frameSize;
        _frameBuffer->resize( _frameSize );
        _camera->setAspectRatio( _frameSize[1] / _frameSize[0] );
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
        _renderer->commit( );
        _camera->commit( );
    }

    ParametersManager& getParametersManager( )
    {
        return _parametersManager;
    }

    Scene& getScene( )
    {
        return *_scene;
    }

    Camera& getCamera( )
    {
        return *_camera;
    }

private:
    void _render( )
    {
        _rendering = true;

        _frameBuffer->unmap( );
        _renderer->render( _frameBuffer );
        _frameBuffer->map( );

        _rendering = false;
    }

    ParametersManager _parametersManager;

    ScenePtr _scene;
    CameraPtr _camera;
    RendererPtr _renderer;
    FrameBufferPtr _frameBuffer;

    Vector2i _frameSize;
    float _timestamp;

    bool _rendering;
    bool _sceneModified;

    ExtensionControllerPtr _extensionController;
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

}

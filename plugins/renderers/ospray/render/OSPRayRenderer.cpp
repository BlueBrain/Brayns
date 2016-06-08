/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#include <brayns/common/log.h>

#include "OSPRayFrameBuffer.h"
#include "OSPRayRenderer.h"
#include "OSPRayScene.h"
#include "OSPRayCamera.h"

#ifndef __APPLE__
// GCC automtically removes the library if the application does not
// make an explicit use of one of its classes. In the case of OSPRay
// classes are loaded dynamicaly. The following line is only to make
// sure that the hbpKernel library is loaded.
#  include <plugins/renderers/ospray/render/ExtendedOBJRenderer.h>
#  include <plugins/renderers/ospray/render/ProximityRenderer.h>
brayns::ExtendedOBJRenderer extendedObjRenderer;
brayns::ProximityRenderer proximityRenderer;
#endif

namespace brayns
{

OSPRayRenderer::OSPRayRenderer(
    const std::string& name,
    ParametersManager& parametersManager )
    : Renderer( parametersManager )
    , _camera( 0 )
{
    RenderingParameters& rp = _parametersManager.getRenderingParameters();
    if( rp.getModule( ) != "" )
        ospLoadModule( rp.getModule( ).c_str( ));

    _renderer = ospNewRenderer( name.c_str( ));
    assert( _renderer );
}

void OSPRayRenderer::render( FrameBufferPtr frameBuffer )
{
    OSPRayFrameBuffer* osprayFrameBuffer =
        dynamic_cast< OSPRayFrameBuffer* >( frameBuffer.get( ));
    ospRenderFrame(
        osprayFrameBuffer->impl( ), _renderer,
        OSP_FB_COLOR | OSP_FB_DEPTH | OSP_FB_ACCUM );
}

void OSPRayRenderer::commit()
{
    RenderingParameters& rp = _parametersManager.getRenderingParameters();
    SceneParameters& sp = _parametersManager.getSceneParameters();
    MaterialType mt = rp.getMaterialType();

    Vector3f color = rp.getBackgroundColor( );
    ospSet3f( _renderer, "bgColor", color.x( ), color.y( ), color.z( ));
    ospSet1i( _renderer, "shadowsEnabled", rp.getShadows( ));
    ospSet1i( _renderer, "softShadowsEnabled",
        rp.getSoftShadows( ));
    ospSet1f( _renderer, "ambientOcclusionStrength",
        rp.getAmbientOcclusionStrength( ));

    ospSet1i( _renderer, "shadingEnabled", ( mt == MT_DIFFUSE ));
    ospSet1f( _renderer, "timestamp", sp.getTimestamp( ));
    ospSet1i( _renderer, "randomNumber", rand() % 1000 );
    ospSet1i( _renderer, "spp", rp.getSamplesPerPixel( ));
    ospSet1i( _renderer, "electronShading", ( mt == MT_ELECTRON ));
    ospSet1f( _renderer, "epsilon", rp.getEpsilon( ));
    ospSet1i( _renderer, "moving", false );
    ospSet1f( _renderer, "detectionDistance",
        rp.getDetectionDistance( ));
    ospSet1i( _renderer, "detectionOnDifferentMaterial",
        rp.getDetectionOnDifferentMaterial( ));
    color = rp.getDetectionNearColor( );
    ospSet3f( _renderer, "detectionNearColor",
        color.x( ), color.y( ), color.z( ));
    color = rp.getDetectionFarColor( );
    ospSet3f( _renderer, "detectionFarColor",
        color.x( ), color.y( ), color.z( ));

    OSPRayScene* osprayScene = static_cast< OSPRayScene* >( _scene.get( ));
    assert( osprayScene );

    const size_t ts = _scene->getSceneParameters().getTimestamp();
    OSPModel* model = osprayScene->modelImpl( ts );
    if( model )
    {
        ospSetObject( _renderer, "world", *model );
        ospCommit( _renderer );
    }
    else
        BRAYNS_ERROR << "No model found for timestamp " << ts << std::endl;

}

void OSPRayRenderer::setCamera( CameraPtr camera )
{
    _camera = static_cast<OSPRayCamera*>( camera.get( ));
    assert( _camera );
    ospSetObject( _renderer, "camera", _camera->ospImpl( ));
}

}

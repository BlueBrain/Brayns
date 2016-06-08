/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#include "ProximityRenderer.h"
#include "ProximityRenderer_ispc.h"

#include <ospray/common/Data.h>
#include <ospray/camera/Camera.h>
#include <ospray/lights/Light.h>

using namespace ospray;

namespace brayns
{

void ProximityRenderer::commit( )
{
    Renderer::commit( );

    lightData = ( ospray::Data* )getParamData( "lights" );
    lightArray.clear( );

    if( lightData )
        for( size_t i = 0; i < lightData->size( ); ++i )
            lightArray.push_back(((Light**)lightData->data)[i]->getIE());

    void **lightPtr = lightArray.empty( ) ? NULL : &lightArray[0];

    vec3f bgColor = getParam3f( "bgColor", vec3f( 0.f ));
    vec3f nearColor = getParam3f( "detectionNearColor", vec3f( 0.f, 1.f, 0.f ));
    vec3f farColor = getParam3f( "detectionFarColor", vec3f( 1.f, 0.f, 0.f ));
    detectionDistance = getParam1f( "detectionDistance", 1.f );
    detectionOnDifferentMaterial =
        bool( getParam1i( "detectionOnDifferentMaterial", 0 ));
    randomNumber = getParam1i( "randomNumber", 0 );
    timestamp = getParam1f( "timestamp", 0.f );
    spp = getParam1i( "spp", 1 );
    electronShadingEnabled = bool( getParam1i( "electronShading", 1 ));

    // Those materials are used for skybox mapping only
    materialData = ( ospray::Data* )getParamData( "materials" );
    materialArray.clear( );
    if( materialData )
        for( size_t i = 0; i < materialData->size( ); ++i )
            materialArray.push_back(
                ( ( ospray::Material** )materialData->data )[i]->getIE( ));
    void **materialArrayPtr =
        materialArray.empty( ) ? nullptr : &materialArray[0];

    ispc::ProximityRenderer_set(
                getIE( ),
                ( ispc::vec3f& )bgColor,
                ( ispc::vec3f& )nearColor,
                ( ispc::vec3f& )farColor,
                detectionDistance,
                detectionOnDifferentMaterial,
                randomNumber,
                timestamp,
                spp,
                electronShadingEnabled,
                lightPtr, lightArray.size( ),
                materialArrayPtr, materialArray.size( ));
}

ProximityRenderer::ProximityRenderer( )
{
    ispcEquivalent = ispc::ProximityRenderer_create( this );
}

OSP_REGISTER_RENDERER( ProximityRenderer, PROXIMITYRENDERER );
OSP_REGISTER_RENDERER( ProximityRenderer, proximityrenderer );
}

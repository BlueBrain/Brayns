/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * Based on OSPRay implementation
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

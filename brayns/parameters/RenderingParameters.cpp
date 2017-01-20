/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include "RenderingParameters.h"
#include <brayns/common/log.h>
#include <brayns/common/exceptions.h>

#include <boost/lexical_cast.hpp>

namespace
{

const std::string DEFAULT_ENGINE = "ospray";
const std::string DEFAULT_RENDERER = "exobj";

const std::string PARAM_ENGINE = "engine";
const std::string PARAM_MODULE = "module";
const std::string PARAM_RENDERER = "renderer";
const std::string PARAM_SPP = "samples-per-pixel";
const std::string PARAM_AMBIENT_OCCLUSION = "ambient-occlusion";
const std::string PARAM_SHADOWS = "shadows";
const std::string PARAM_SOFT_SHADOWS = "soft-shadows";
const std::string PARAM_SHADING = "shading";
const std::string PARAM_RADIANCE = "radiance";
const std::string PARAM_BACKGROUND_COLOR = "background-color";
const std::string PARAM_DETECTION_DISTANCE = "detection-distance";
const std::string PARAM_DETECTION_ON_DIFFERENT_MATERIAL = "detection-on-different-material";
const std::string PARAM_DETECTION_NEAR_COLOR = "detection-near-color";
const std::string PARAM_DETECTION_FAR_COLOR = "detection-far-color";
const std::string PARAM_EPSILON = "epsilon";
const std::string PARAM_CAMERA_TYPE = "camera-type";
const std::string PARAM_HEAD_LIGHT = "head-light";

const std::string RENDERERS[4] = {
    "exobj", "proximityrenderer", "simulationrenderer", "particlerenderer"
};

const std::string CAMERA_TYPES[5] = {
    "perspective", "stereo", "orthographic", "panoramic", "clipped"
};

const std::string SHADING_TYPES[3] = {
    "none", "diffuse", "electron"
};

}

namespace brayns
{

RenderingParameters::RenderingParameters()
    : AbstractParameters( "Rendering" )
    , _engine(DEFAULT_ENGINE)
    , _renderer( RendererType::basic )
    , _ambientOcclusionStrength( 0.f )
    , _shading( ShadingType::diffuse )
    , _lightEmittingMaterials( false )
    , _spp( 1 )
    , _shadows( false )
    , _softShadows( false )
    , _backgroundColor( Vector3f( 0.f, 0.f, 0.f ))
    , _detectionDistance( 1.f )
    , _detectionOnDifferentMaterial( true )
    , _detectionNearColor( 1.f, 0.f, 0.f )
    , _detectionFarColor( 0.f, 1.f, 0.f )
    , _epsilon( 0.f )
    , _cameraType( CameraType::perspective )
    , _headLight( false )
{
    _parameters.add_options()
        (PARAM_ENGINE.c_str(), po::value< std::string >(),
            "Engine name [ospray|optix|livre]")
        (PARAM_MODULE.c_str(), po::value< std::string >(),
            "OSPRay module name [string]")
        (PARAM_RENDERER.c_str(), po::value< std::string >(),
            "OSPRay active renderer [basic|simulation|proximity|particle]")
        (PARAM_SPP.c_str(), po::value< size_t >(),
            "Number of samples per pixel [int]")
        (PARAM_AMBIENT_OCCLUSION.c_str(), po::value< float >(),
            "Ambient occlusion strength [float]")
        (PARAM_SHADOWS.c_str(), po::value< bool >(),
            "Enable/Disable shadows [bool]")
        (PARAM_SOFT_SHADOWS.c_str(), po::value< bool >(),
            "Enable/Disable soft shadows [bool]")
        (PARAM_SHADING.c_str(), po::value< std::string >(),
            "Shading type [none|diffuse|electron]")
        (PARAM_RADIANCE.c_str(), po::value< bool >(),
            "Enable/Disable radiance [bool]")
        (PARAM_BACKGROUND_COLOR.c_str(), po::value< floats >()->multitoken(),
            "Background color [float float float]")
        (PARAM_DETECTION_DISTANCE.c_str(), po::value< float >(),
            "Detection distance in model units [float]")
        (PARAM_DETECTION_ON_DIFFERENT_MATERIAL.c_str(), po::value< bool >(),
            "Enable/Disable detection on different materials only [bool]")
        (PARAM_DETECTION_NEAR_COLOR.c_str(),
            po::value< floats >()->multitoken(), "Detection near color [float float float]")
        (PARAM_DETECTION_FAR_COLOR.c_str(),
            po::value< floats >()->multitoken(), "Detection far color [float float float]")
        (PARAM_EPSILON.c_str(),
            po::value< float >(), "All intersection distances less than the "
            "epsilon value are ignored by the ray-tracer [float]")
        (PARAM_CAMERA_TYPE.c_str(),
            po::value< std::string >(), "Camera type [perspective|stereo|orthographic|panoramic]")
        (PARAM_HEAD_LIGHT.c_str(),
            po::value< bool >(), "Enable/Disable light source attached to camera origin [bool]");

    // Add default renderers
    _renderers.push_back( RendererType::basic );
    _renderers.push_back( RendererType::simulation );
    _renderers.push_back( RendererType::particle );
    _renderers.push_back( RendererType::proximity );
}

bool RenderingParameters::_parse( const po::variables_map& vm )
{
    if( vm.count( PARAM_ENGINE ))
        _engine = vm[ PARAM_ENGINE ].as< std::string >();
    if( vm.count( PARAM_MODULE ))
        _module = vm[ PARAM_MODULE ].as< std::string >();
    if( vm.count( PARAM_RENDERER ))
    {
        _renderer = RendererType::basic;
        const std::string& renderer = vm[ PARAM_RENDERER ].as< std::string >();
        for( size_t i = 0; i < sizeof( RENDERERS ) / sizeof( RENDERERS[0] ); ++i )
            if( renderer == RENDERERS[i])
                _renderer = static_cast< RendererType >( i );
    }
    if( vm.count( PARAM_SPP ))
        _spp = vm[PARAM_SPP].as< size_t >();
    if( vm.count( PARAM_AMBIENT_OCCLUSION ))
        _ambientOcclusionStrength = vm[ PARAM_AMBIENT_OCCLUSION ].as< float >();
    if( vm.count( PARAM_SHADOWS ))
        _shadows = vm[ PARAM_SHADOWS ].as< bool >();
    if( vm.count( PARAM_SOFT_SHADOWS ))
        _softShadows = vm[ PARAM_SOFT_SHADOWS ].as< bool >();
    if( vm.count( PARAM_SHADING ))
    {
        _shading = ShadingType::diffuse;
        const std::string& shading = vm[PARAM_SHADING].as< std::string >();
        for( size_t i = 0;
             i < sizeof( SHADING_TYPES ) / sizeof( SHADING_TYPES[0] );
             ++i )
            if( shading == SHADING_TYPES[i])
                _shading = static_cast< ShadingType >( i );
    }
    if( vm.count( PARAM_BACKGROUND_COLOR ))
    {
        floats values = vm[ PARAM_BACKGROUND_COLOR ].as< floats >();
        if( values.size() == 3 )
            _backgroundColor = Vector3f( values[0], values[1], values[2] );
    }
    if( vm.count( PARAM_DETECTION_DISTANCE ))
        _detectionDistance = vm[ PARAM_DETECTION_DISTANCE ].as< float >();
    if( vm.count( PARAM_DETECTION_ON_DIFFERENT_MATERIAL ))
        _detectionOnDifferentMaterial =
            vm[ PARAM_DETECTION_ON_DIFFERENT_MATERIAL ].as< bool >();
    if( vm.count( PARAM_DETECTION_NEAR_COLOR ))
    {
        floats values = vm[ PARAM_DETECTION_NEAR_COLOR ].as< floats >();
        if( values.size() == 3 )
            _detectionNearColor  = Vector3f( values[0], values[1], values[2] );
    }
    if( vm.count( PARAM_DETECTION_FAR_COLOR ))
    {
        floats values = vm[ PARAM_DETECTION_FAR_COLOR ].as< floats >();
        if( values.size() == 3 )
            _detectionFarColor  = Vector3f( values[0], values[1], values[2] );
    }
    if( vm.count( PARAM_EPSILON ))
        _epsilon = vm[ PARAM_EPSILON ].as< float >();
    if( vm.count( PARAM_CAMERA_TYPE ))
    {
        _cameraType = CameraType::perspective;
        const std::string& cameraType = vm[ PARAM_CAMERA_TYPE ].as< std::string >();
        for( size_t i = 0; i < sizeof( CAMERA_TYPES ) / sizeof( CAMERA_TYPES[0] ); ++i )
            if( cameraType == CAMERA_TYPES[i])
                _cameraType = static_cast< CameraType >( i );
    }
    if( vm.count( PARAM_HEAD_LIGHT ))
        _headLight = vm[ PARAM_HEAD_LIGHT ].as< bool >();
    return true;
}

void RenderingParameters::print()
{
    AbstractParameters::print();
    BRAYNS_INFO << "Engine                            :" <<
        _engine << std::endl;
    BRAYNS_INFO << "Module                            :" <<
        _module << std::endl;
    BRAYNS_INFO << "Supported renderers               :" << std::endl;
    for( const auto& renderer: _renderers )
        BRAYNS_INFO << "- " << getRendererAsString( renderer ) << std::endl;
    BRAYNS_INFO << "Renderer                          :" <<
        getRendererAsString( _renderer ) << std::endl;
    BRAYNS_INFO << "Samples per pixel                 :" <<
        _spp << std::endl;
    BRAYNS_INFO << "Ambient occlusion strength        :" <<
        _ambientOcclusionStrength << std::endl;
    BRAYNS_INFO << "Shadows                           :" <<
        ( _shadows ? "on" : "off" ) << std::endl;
    BRAYNS_INFO << "Soft shadows                      :" <<
        ( _softShadows ? "on" : "off" ) << std::endl;
    BRAYNS_INFO << "Shading                           :" <<
        getShadingAsString( _shading ) << std::endl;
    BRAYNS_INFO << "Background color                  :" <<
        _backgroundColor << std::endl;
    BRAYNS_INFO << "Detection: " << std::endl;
    BRAYNS_INFO << "- Detection distance              : " <<
       _detectionDistance << std::endl;
    BRAYNS_INFO << "- Detection on different material : " <<
       ( _detectionOnDifferentMaterial ? "on" : "off" ) << std::endl;
    BRAYNS_INFO << "- Detection near color            : " <<
       _detectionNearColor << std::endl;
    BRAYNS_INFO << "- Detection far color             : " <<
       _detectionFarColor << std::endl;
    BRAYNS_INFO << "Epsilon                           : " <<
       _epsilon << std::endl;
    BRAYNS_INFO << "Camera type                       : " <<
       getCameraTypeAsString( _cameraType ) << std::endl;
}

const std::string& RenderingParameters::getRendererAsString(
     const RendererType value ) const
{
    return RENDERERS[ static_cast< size_t >( value )];
}

const std::string& RenderingParameters::getCameraTypeAsString(
    const CameraType value ) const
{
    return CAMERA_TYPES[ static_cast< size_t >( value )];
}

const std::string& RenderingParameters::getShadingAsString(
    const ShadingType value ) const
{
    return SHADING_TYPES[ static_cast< size_t >( value )];
}

}

/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#include "RenderingParameters.h"
#include <brayns/common/log.h>
#include <brayns/common/exceptions.h>

#include <boost/lexical_cast.hpp>

namespace
{
const std::string DEFAULT_RENDERER = "raycast_eyelight";

const std::string PARAM_MODULE = "module";
const std::string PARAM_RENDERERS = "renderers";
const std::string PARAM_RENDERER = "renderer";
const std::string PARAM_SPP = "spp";
const std::string PARAM_DOF = "dof";
const std::string PARAM_AMBIENT_OCCLUSION = "ambient-occlusion";
const std::string PARAM_NO_LIGHT_SHADING = "no-light-shading";
const std::string PARAM_SHADOWS = "shadows";
const std::string PARAM_SOFT_SHADOWS = "soft-shadows";
const std::string PARAM_ELECTRON_SHADING = "electron-shading";
const std::string PARAM_GRADIENT_BACKGROUND = "gradient-background";
const std::string PARAM_RADIANCE = "radiance";
const std::string PARAM_BACKGROUND_COLOR = "background-color";
const std::string PARAM_DETECTION_DISTANCE = "detection-distance";
const std::string PARAM_DETECTION_ON_DIFFERENT_MATERIAL =
    "detection-on-different-material";
const std::string PARAM_DETECTION_NEAR_COLOR = "detection-near-color";
const std::string PARAM_DETECTION_FAR_COLOR = "detection-far-color";
const std::string PARAM_EPSILON = "epsilon";
const std::string PARAM_CAMERA_TYPE = "camera-type";

}

namespace brayns
{

RenderingParameters::RenderingParameters( )
    : AbstractParameters( "Rendering" )
    , _renderer( DEFAULT_RENDERER )
    , _ambientOcclusionStrength( 0.f )
    , _dof( false )
    , _dofStrength( 0.f )
    , _electronShading( false )
    , _gradientBackground( false )
    , _lightShading( true )
    , _lightEmittingMaterials( false )
    , _spp( 1 )
    , _shadows( false )
    , _softShadows( false )
    , _backgroundColor( Vector3f( 0.f, 0.f, 0.f ))
    , _detectionDistance( 1.f )
    , _detectionOnDifferentMaterial( false )
    , _detectionNearColor( 1.f, 0.f, 0.f )
    , _detectionFarColor( 0.f, 1.f, 0.f )
    , _epsilon( 1.e-4f )
    , _cameraType( CT_PERSPECTIVE )
{
    _parameters.add_options()
        (PARAM_MODULE.c_str(), po::value< std::string >( ),
            "OSPRay module name")
        (PARAM_RENDERER.c_str(), po::value< std::string >( ),
            "Active renderer")
        (PARAM_RENDERERS.c_str(), po::value< strings >( )->multitoken(),
            "Renderers")
        (PARAM_SPP.c_str(), po::value< size_t >( ),
            "Number of samples per pixel")
        (PARAM_DOF.c_str(), po::value< float >( ),
            "Depth of field strength")
        (PARAM_AMBIENT_OCCLUSION.c_str(), po::value< float >( ),
            "Ambient occlusion strength")
        (PARAM_NO_LIGHT_SHADING.c_str(), po::value< bool >( ),
            "Light shading enabled")
        (PARAM_SHADOWS.c_str(), po::value< bool >( ),
            "Shadows enabled")
        (PARAM_SOFT_SHADOWS.c_str(), po::value< bool >( ),
            "Soft shadows enabled")
        (PARAM_ELECTRON_SHADING.c_str(), po::value< bool >( ),
            "Electron shading enabled")
        (PARAM_GRADIENT_BACKGROUND.c_str(), po::value< bool >( ),
            "Gradient background enabled")
        (PARAM_RADIANCE.c_str(), po::value< bool >( ),
            "Radiance enabled")
        (PARAM_BACKGROUND_COLOR.c_str(), po::value< floats >( )->multitoken(),
            "Background color")
        (PARAM_DETECTION_DISTANCE.c_str(), po::value< float >( ),
            "Detection distance")
        (PARAM_DETECTION_ON_DIFFERENT_MATERIAL.c_str(), po::value< bool >( ),
            "Detection considered for different materials only")
        (PARAM_DETECTION_NEAR_COLOR.c_str(),
            po::value< floats >( )->multitoken(), "Detection near color")
        (PARAM_DETECTION_FAR_COLOR.c_str(),
            po::value< floats >( )->multitoken(), "Detection far color")
        (PARAM_EPSILON.c_str(),
            po::value< float >( ), "All intersection distances less than the "
            "epsilon value are ignored by the raytracer")
        (PARAM_CAMERA_TYPE.c_str(),
            po::value< size_t >( ), "Camera type (0: perspective, "
            "1: perspective stereo, 2: orthographic, 3: panoramic)");

    // Add default renderers
    _renderers.push_back("exobj");
    _renderers.push_back("proximityrenderer");
    _renderers.push_back("simulationrenderer");
}

bool RenderingParameters::_parse( const po::variables_map& vm )
{
    if( vm.count( PARAM_MODULE ))
        _module = vm[PARAM_MODULE].as< std::string >( );
    if( vm.count( PARAM_RENDERER ))
        _renderer = vm[PARAM_RENDERER].as< std::string >( );
    if( vm.count( PARAM_SPP ))
        _spp = vm[PARAM_SPP].as< size_t >( );
    if( vm.count( PARAM_DOF ))
    {
        _dof = true;
        _dofStrength = vm[PARAM_DOF].as< float >( );
    }
    if( vm.count( PARAM_AMBIENT_OCCLUSION ))
        _ambientOcclusionStrength = vm[PARAM_AMBIENT_OCCLUSION].as< float >( );
    if( vm.count( PARAM_NO_LIGHT_SHADING ))
        _lightShading = !vm[PARAM_NO_LIGHT_SHADING].as< bool >( );
    if( vm.count( PARAM_SHADOWS ))
        _shadows = vm[PARAM_SHADOWS].as< bool >( );
    if( vm.count( PARAM_SOFT_SHADOWS ))
        _softShadows = vm[PARAM_SOFT_SHADOWS].as< bool >( );
    if( vm.count( PARAM_ELECTRON_SHADING ))
        _electronShading = vm[PARAM_ELECTRON_SHADING].as< bool >( );
    if( vm.count( PARAM_GRADIENT_BACKGROUND ))
        _gradientBackground = vm[PARAM_GRADIENT_BACKGROUND].as< bool >( );
    if( vm.count( PARAM_BACKGROUND_COLOR ))
    {
        floats values = vm[PARAM_BACKGROUND_COLOR].as< floats >( );
        if( values.size() == 3 )
            _backgroundColor = Vector3f( values[0], values[1], values[2] );
    }
    if( vm.count( PARAM_DETECTION_DISTANCE ))
        _detectionDistance = vm[PARAM_DETECTION_DISTANCE].as< float >( );
    if( vm.count( PARAM_DETECTION_ON_DIFFERENT_MATERIAL ))
        _detectionOnDifferentMaterial =
            vm[PARAM_DETECTION_ON_DIFFERENT_MATERIAL].as< bool >( );
    if( vm.count( PARAM_DETECTION_NEAR_COLOR ))
    {
        floats values = vm[PARAM_DETECTION_NEAR_COLOR].as< floats >( );
        if( values.size() == 3 )
            _detectionNearColor  = Vector3f( values[0], values[1], values[2] );
    }
    if( vm.count( PARAM_DETECTION_FAR_COLOR ))
    {
        floats values = vm[PARAM_DETECTION_FAR_COLOR].as< floats >( );
        if( values.size() == 3 )
            _detectionFarColor  = Vector3f( values[0], values[1], values[2] );
    }
    if( vm.count( PARAM_EPSILON ))
        _epsilon = vm[PARAM_EPSILON].as< float >( );
    if( vm.count( PARAM_CAMERA_TYPE ))
        _cameraType = static_cast< CameraType > (
            vm[PARAM_CAMERA_TYPE].as< size_t >( ));
    return true;
}

void RenderingParameters::print( )
{
    AbstractParameters::print( );
    BRAYNS_INFO << "Module                  :" <<
        _module << std::endl;
    BRAYNS_INFO << "Supported renderers     :" << std::endl;
    for( std::string renderer: _renderers )
        BRAYNS_INFO << "- " << renderer << std::endl;
    BRAYNS_INFO << "Renderer                :" <<
        _renderer << std::endl;
    BRAYNS_INFO << "Samples per pixel       :" <<
        _spp << std::endl;
    BRAYNS_INFO << "Depth of field strength :" <<
        _dofStrength << std::endl;
    BRAYNS_INFO << "AO strength             :" <<
        _ambientOcclusionStrength << std::endl;
    BRAYNS_INFO << "Shadows                 :" <<
        ( _shadows ? "on" : "off" ) << std::endl;
    BRAYNS_INFO << "Soft shadows            :" <<
        ( _softShadows ? "on" : "off" ) << std::endl;
    BRAYNS_INFO << "Electron shading        :" <<
        ( _electronShading ? "on" : "off" ) << std::endl;
    BRAYNS_INFO << "Gradient background     :" <<
        ( _gradientBackground ? "on" : "off" ) << std::endl;
    BRAYNS_INFO << "Background color        :" <<
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
       static_cast< size_t > (_cameraType) << std::endl;
}

}

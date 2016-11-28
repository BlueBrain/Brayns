/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
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

#include "ZeroEQPlugin.h"

#include <brayns/Brayns.h>
#include <plugins/engines/common/Engine.h>
#include <brayns/common/camera/Camera.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/common/renderer/Renderer.h>
#include <brayns/common/renderer/FrameBuffer.h>
#include <brayns/parameters/ParametersManager.h>
#include <zerobuf/render/camera.h>
#include <brayns/common/simulation/AbstractSimulationHandler.h>
#include <brayns/common/simulation/SpikeSimulationHandler.h>

#include <brayns/version.h>


namespace brayns
{

ZeroEQPlugin::ZeroEQPlugin( Brayns& brayns )
    : ExtensionPlugin( brayns )
    , _compressor( tjInitCompress() )
    , _processingImageJpeg( false )
{
    _setupHTTPServer();
    _setupRequests();
    _setupSubscriber();
    _initializeDataSource();
    _initializeSettings();
}

ZeroEQPlugin::~ZeroEQPlugin( )
{
    if( _compressor )
        tjDestroy( _compressor );

    if( _httpServer )
    {
        _httpServer->remove( *_brayns.getCamera().getSerializable() );
        _httpServer->remove( _remoteImageJPEG );
    }
}

void ZeroEQPlugin::run()
{
    while( _subscriber.receive( 1 )) {}
}

bool ZeroEQPlugin::operator ! () const
{
    return !_httpServer;
}

::zeroeq::http::Server* ZeroEQPlugin::operator->()
{
    return _httpServer.get();
}

void ZeroEQPlugin::_setupHTTPServer()
{
    const strings& arguments =
        _brayns.getParametersManager().getApplicationParameters().arguments();
    char** argv = new char*[arguments.size()];
    for( size_t i = 0; i < arguments.size( ); ++i )
        argv[ i ] = const_cast< char* >( arguments[ i ].c_str( ));

    _httpServer = ::zeroeq::http::Server::parse(
        arguments.size(), const_cast< const char** >( argv ), _subscriber );
    delete [] argv;

    if( !_httpServer )
    {
        BRAYNS_ERROR << "HTTP could not be initialized" << std::endl;
        return;
    }

    BRAYNS_INFO << "Registering handlers on " <<
        _httpServer->getURI() << std::endl;

    _httpServer->handleGET( "brayns/version", brayns::Version::getSchema(),
                            &brayns::Version::toJSON );
    servus::Serializable& cam = *_brayns.getCamera().getSerializable();
    _httpServer->handle( cam );
    cam.registerDeserializedCallback( std::bind( &ZeroEQPlugin::_cameraUpdated, this ));

    _httpServer->handleGET( _remoteImageJPEG );
    _remoteImageJPEG.registerSerializeCallback(
        std::bind( &ZeroEQPlugin::_requestImageJPEG, this ));

    _httpServer->handleGET( _remoteFrameBuffers );
    _remoteFrameBuffers.registerSerializeCallback(
        std::bind( &ZeroEQPlugin::_requestFrameBuffers, this ));

    _httpServer->handlePUT( _remoteAttribute );
    _remoteAttribute.registerDeserializedCallback(
        std::bind( &ZeroEQPlugin::_attributeUpdated, this ));

    _httpServer->handlePUT( _remoteResetCamera );
    _remoteResetCamera.registerDeserializedCallback(
        std::bind( &ZeroEQPlugin::_resetCameraUpdated, this ));

    _httpServer->handlePUT( _remoteResetScene );
    _remoteResetScene.registerDeserializedCallback(
        std::bind( &ZeroEQPlugin::_resetSceneUpdated, this ));

    _httpServer->handle( _remoteScene );
    _remoteScene.registerDeserializedCallback(
        std::bind( &ZeroEQPlugin::_sceneUpdated, this ));
    _remoteScene.registerSerializeCallback(
        std::bind( &ZeroEQPlugin::_requestScene, this ));

    _httpServer->handle( _remoteTransferFunction1D );
    _remoteTransferFunction1D.registerDeserializedCallback(
        std::bind( &ZeroEQPlugin::_transferFunction1DUpdated, this ));
    _remoteTransferFunction1D.registerSerializeCallback(
        std::bind( &ZeroEQPlugin::_requestTransferFunction1D, this ));

    _httpServer->handle( _remoteSpikes );
    _remoteSpikes.registerDeserializedCallback(
        std::bind( &ZeroEQPlugin::_spikesUpdated, this ));
    _remoteSpikes.registerSerializeCallback(
        std::bind( &ZeroEQPlugin::_requestSpikes, this ));

    _remoteLookupTable1D.registerDeserializedCallback(
        std::bind( &ZeroEQPlugin::_LookupTable1DUpdated, this ));
    _remoteLookupTable1D.registerSerializeCallback(
        std::bind( &ZeroEQPlugin::_requestLookupTable1D, this ));

    _httpServer->handle( _remoteColormap );
    _remoteColormap.registerDeserializedCallback(
        std::bind( &ZeroEQPlugin::_colormapUpdated, this ));
    _remoteColormap.registerSerializeCallback(
        std::bind( &ZeroEQPlugin::_requestColormap, this ));

    _httpServer->handle( _remoteDataSource );
    _remoteDataSource.registerDeserializedCallback(
        std::bind( &ZeroEQPlugin::_dataSourceUpdated, this ));

    _httpServer->handle( _remoteSettings );
    _remoteSettings.registerDeserializedCallback(
        std::bind( &ZeroEQPlugin::_settingsUpdated, this ));

    _httpServer->handle( _remoteFrame );
    _remoteFrame.registerSerializeCallback(
        std::bind( &ZeroEQPlugin::_requestFrame, this ));
    _remoteFrame.registerDeserializedCallback(
        std::bind( &ZeroEQPlugin::_frameUpdated, this ));

    _httpServer->handle( _remoteViewport );
    _remoteViewport.registerSerializeCallback(
        std::bind( &ZeroEQPlugin::_requestViewport, this ));
    _remoteViewport.registerDeserializedCallback(
        std::bind( &ZeroEQPlugin::_viewportUpdated, this ));
}

void ZeroEQPlugin::_setupRequests()
{
    ::zerobuf::render::Camera camera;
    _requests[ camera.getTypeIdentifier() ] =
        [&]{ return _publisher.publish( *_brayns.getCamera().getSerializable() ); };

    ::lexis::render::ImageJPEG imageJPEG;
    _requests[ imageJPEG.getTypeIdentifier() ] =
        std::bind( &ZeroEQPlugin::_requestImageJPEG, this );

    ::zerobuf::render::FrameBuffers frameBuffers;
    _requests[ frameBuffers.getTypeIdentifier() ] =
        std::bind( &ZeroEQPlugin::_requestFrameBuffers, this );

    ::zerobuf::render::TransferFunction1D transferFunction1D;
    _requests[ transferFunction1D.getTypeIdentifier() ] =
        std::bind( &ZeroEQPlugin::_requestTransferFunction1D, this );

    ::zerobuf::data::Spikes spikes;
    _requests[ spikes.getTypeIdentifier() ] =
        std::bind( &ZeroEQPlugin::_requestSpikes, this );
}

void ZeroEQPlugin::_setupSubscriber()
{
    _subscriber.subscribe( _remoteLookupTable1D );
}

void ZeroEQPlugin::_cameraUpdated()
{
    _brayns.getFrameBuffer().clear();
    _brayns.getCamera().commit();
}

void ZeroEQPlugin::_attributeUpdated( )
{
    BRAYNS_INFO << _remoteAttribute.getKeyString() << " = " <<
        _remoteAttribute.getValueString() << std::endl;
    _brayns.getParametersManager().set(
        _remoteAttribute.getKeyString(), _remoteAttribute.getValueString());
    _brayns.getScene().commitVolumeData();
    _brayns.getRenderer().commit();
    _brayns.getFrameBuffer().clear();
}

void ZeroEQPlugin::_resetCameraUpdated()
{
    BRAYNS_INFO << "Resetting camera" << std::endl;
    SceneParameters& sceneParameters =
        _brayns.getParametersManager().getSceneParameters();
    _brayns.getCamera().setEnvironmentMap( !sceneParameters.getEnvironmentMap().empty( ));
    _brayns.getCamera().reset();
    _brayns.getCamera().commit();
    _brayns.getFrameBuffer().clear();
}

void ZeroEQPlugin::_resetSceneUpdated()
{
    BRAYNS_INFO << "Resetting scene" << std::endl;
    _brayns.getScene().reset();
    _brayns.buildScene();
}

bool ZeroEQPlugin::_requestScene()
{
    BRAYNS_INFO << "Getting materials " << std::endl;

    auto& ms = _remoteScene.getMaterials();
    ms.clear();
    Scene& scene = _brayns.getScene();
    const auto& materials = scene.getMaterials();

    for( const auto& material: materials )
    {
        ::zerobuf::render::Material m;
        m.setDiffuse_color( material->getColor( ));
        m.setSpecular_color( material->getSpecularColor( ));
        m.setSpecular_exponent( material->getSpecularExponent( ));
        m.setReflection_index( material->getReflectionIndex( ));
        m.setOpacity( material->getOpacity( ));
        m.setRefraction_index( material->getRefractionIndex( ));
        m.setLight_emission( material->getEmission( ));
        ms.push_back(m);
    }
    return true;
}

void ZeroEQPlugin::_sceneUpdated( )
{
    BRAYNS_INFO << "Setting materials " << std::endl;
    auto& materials = _remoteScene.getMaterials();
    Scene& scene = _brayns.getScene();

    for( size_t materialId = 0; materialId < materials.size(); ++materialId )
    {
        MaterialPtr material = scene.getMaterial( materialId );

        if( material)
        {
            ::zerobuf::render::Material& m = materials[ materialId ];
            const floats& diffuse = m.getDiffuse_colorVector();
            Vector3f kd = { diffuse[0], diffuse[1], diffuse[2] };
            material->setColor( kd );

            const floats& specular = m.getSpecular_colorVector();
            Vector3f ks = { specular[0], specular[1], specular[2] };
            material->setSpecularColor( ks );

            material->setSpecularExponent( m.getSpecular_exponent() );
            material->setReflectionIndex( m.getReflection_index() );
            material->setOpacity( m.getOpacity() );
            material->setRefractionIndex( m.getRefraction_index() );
            material->setEmission( m.getLight_emission() );
        }
    }
    scene.commitMaterials( true );
    _brayns.getRenderer().commit();
    _brayns.getFrameBuffer().clear();
}

void ZeroEQPlugin::_spikesUpdated( )
{
#if 0
    AbstractSimulationHandlerPtr simulationHandler =
        _brayns.getScene().getSimulationHandler();

    SpikeSimulationHandler* spikeSimulationHandler =
        dynamic_cast< SpikeSimulationHandler * >(simulationHandler.get());

    if( spikeSimulationHandler )
    {
        uint64_t ts = _remoteSpikes.getTimestamp();
        float* data = (float*)spikeSimulationHandler->getFrameData( ts );
        for( const auto& gid: _remoteSpikes.getGidsVector() )
            data[gid] = ts;

        _brayns.getFrameBuffer().clear();
        _brayns.getScene().commitSimulationData();
    }
#endif
}


bool ZeroEQPlugin::_requestTransferFunction1D()
{
    Scene& scene = _brayns.getScene();
    TransferFunction& transferFunction = scene.getTransferFunction();
    std::vector< ::zerobuf::render::Point2D > items;

    for( const auto& controlPoint: transferFunction.getControlPoints( TF_RED ) )
        items.push_back( ::zerobuf::render::Point2D(
            controlPoint.x(), controlPoint.y() ));
    _remoteTransferFunction1D.setAttribute( transferFunction.getAttributeAsString( TF_RED ) );
    _remoteTransferFunction1D.setPoints( items );
    return true;
}

void ZeroEQPlugin::_transferFunction1DUpdated( )
{
    Scene& scene = _brayns.getScene();
    TransferFunction& transferFunction = scene.getTransferFunction();
    std::vector< ::zerobuf::render::Point2D > points = _remoteTransferFunction1D.getPointsVector();

    const std::string& attributeName = _remoteTransferFunction1D.getAttributeString();
    BRAYNS_INFO << "Setting "
                << points.size() << " control points for transfer function attribute <"
                << attributeName <<  ">" << std::endl;

    const TransferFunctionAttribute attribute =
        transferFunction.getAttributeFromString( attributeName );

    if( attribute == TF_UNDEFINED )
        return;

    auto& controlPoints = transferFunction.getControlPoints( attribute );
    controlPoints.clear();
    for( const auto& point: points )
        controlPoints.push_back( Vector2f(point.getX(), point.getY()));
    transferFunction.resample();

    scene.commitSimulationData();
    scene.commitTransferFunctionData();
    _brayns.getRenderer().commit();
    _brayns.getFrameBuffer().clear();
}

void ZeroEQPlugin::_LookupTable1DUpdated( )
{
    Scene& scene = _brayns.getScene();
    TransferFunction& transferFunction = scene.getTransferFunction();

    transferFunction.clear();
    Vector4fs& diffuseColors = transferFunction.getDiffuseColors();
    floats& emissionIntensities = transferFunction.getEmissionIntensities();

    const uint8_ts& lut = _remoteLookupTable1D.getLutVector();
    for( size_t i = 0; i < lut.size(); i += 4 )
    {
        Vector4f color = {
            lut[ i ] / 255.f,
            lut[ i + 1 ] / 255.f,
            lut[ i + 2 ] / 255.f,
            lut[ i + 3 ] / 255.f
        };
        diffuseColors.push_back( color );
        emissionIntensities.push_back( 0.f );
    }

    transferFunction.setValuesRange( Vector2f( 0.f, lut.size() / 4 ));
    scene.commitTransferFunctionData();
    _brayns.getFrameBuffer().clear();
}

bool ZeroEQPlugin::_requestLookupTable1D( )
{
    Scene& scene = _brayns.getScene();
    TransferFunction& transferFunction = scene.getTransferFunction();

    Vector4fs& diffuseColors = transferFunction.getDiffuseColors();

    uint8_ts lut;
    lut.clear();
    for( const auto& color: diffuseColors )
    {
        lut.push_back( color.x() * 255.f );
        lut.push_back( color.y() * 255.f );
        lut.push_back( color.z() * 255.f );
        lut.push_back( color.w() * 255.f );
    }
    _remoteLookupTable1D.setLut( lut );
    return true;
}

void ZeroEQPlugin::_colormapUpdated()
{
    Scene& scene = _brayns.getScene();
    TransferFunction& transferFunction = scene.getTransferFunction();

    transferFunction.clear();
    Vector4fs& diffuseColors = transferFunction.getDiffuseColors();
    floats& emissionIntensities = transferFunction.getEmissionIntensities();

    const uint8_ts& rgba = _remoteColormap.getRgbaVector();
    for( size_t i = 0; i < rgba.size(); i += 4 )
    {
        Vector4f color = {
            rgba[ i ] / 255.f,
            rgba[ i + 1 ] / 255.f,
            rgba[ i + 2 ] / 255.f,
            rgba[ i + 3 ] / 255.f
        };
        diffuseColors.push_back( color );
        emissionIntensities.push_back( 0.f );
    }

    const auto& range = _remoteColormap.getRange( );
    transferFunction.setValuesRange( Vector2f(range[0], range[1] ));
    scene.commitTransferFunctionData();
    _brayns.getFrameBuffer().clear();
}

bool ZeroEQPlugin::_requestColormap( )
{
    Scene& scene = _brayns.getScene();
    TransferFunction& transferFunction = scene.getTransferFunction();

    Vector4fs& diffuseColors = transferFunction.getDiffuseColors();

    uint8_ts rgba;
    rgba.clear();
    for( const auto& color: diffuseColors )
    {
        rgba.push_back( color.x() * 255.f );
        rgba.push_back( color.y() * 255.f );
        rgba.push_back( color.z() * 255.f );
        rgba.push_back( color.w() * 255.f );
    }
    _remoteColormap.setRgba( rgba );
    const auto& range = transferFunction.getValuesRange( );
    _remoteColormap.setRange( { range[0], range[1] } );
    return true;
}

void ZeroEQPlugin::_resizeImage(
    unsigned int* srcData,
    const Vector2i& srcSize,
    const Vector2i& dstSize,
    uints& dstData)
{
    dstData.reserve(dstSize.x() * dstSize.y());
    size_t x_ratio =
        static_cast< size_t >(((srcSize.x() << 16) / dstSize.x()) + 1);
    size_t y_ratio =
        static_cast< size_t >(((srcSize.y() << 16) / dstSize.y()) + 1);

    for( int y = 0; y < dstSize.y(); ++y )
    {
        for( int x=0; x < dstSize.x() ; ++x)
        {
            const size_t x2 = ((x*x_ratio) >> 16);
            const size_t y2 = ((y*y_ratio) >> 16);
            dstData[ ( y * dstSize.x() ) + x ] =
                srcData[ ( y2 * srcSize.x() ) + x2 ] ;
        }
    }
}

bool ZeroEQPlugin::_requestImageJPEG()
{
    if(!_processingImageJpeg)
    {
        _processingImageJpeg = true;
        const auto& newFrameSize =
            _brayns.getParametersManager().getApplicationParameters().getJpegSize();
        if( newFrameSize.x() == 0 || newFrameSize.y() == 0 )
        {
            BRAYNS_ERROR << "Encountered invalid size of image JPEG: "
                         << newFrameSize << std::endl;
            _processingImageJpeg = false;
            return false;
        }

        FrameBuffer& frameBuffer = _brayns.getFrameBuffer();
        const auto& frameSize = frameBuffer.getSize();
        unsigned int* colorBuffer =
            (unsigned int*)frameBuffer.getColorBuffer( );
        if( colorBuffer )
        {
            unsigned int* resizedColorBuffer = colorBuffer;

            uints resizedBuffer;
            if( frameSize != newFrameSize )
            {
                _resizeImage( colorBuffer, frameSize,
                              newFrameSize, resizedBuffer);
                resizedColorBuffer = resizedBuffer.data();
            }

            unsigned long jpegSize =
                newFrameSize.x( ) * newFrameSize.y( ) * sizeof( unsigned long );
            uint8_t* jpegData = _encodeJpeg(
                    ( uint32_t )newFrameSize.x( ),
                    ( uint32_t )newFrameSize.y( ),
                    ( uint8_t* )resizedColorBuffer, jpegSize );

            _remoteImageJPEG.setData( jpegData, jpegSize  );
            tjFree(jpegData);
        }
        _processingImageJpeg = false;
    }
    return true;
}

bool ZeroEQPlugin::_requestFrameBuffers()
{
    FrameBuffer& frameBuffer = _brayns.getFrameBuffer();
    const Vector2i frameSize = frameBuffer.getSize( );
    const float* depthBuffer = frameBuffer.getDepthBuffer( );

    _remoteFrameBuffers.setWidth( frameSize.x( ));
    _remoteFrameBuffers.setHeight( frameSize.y( ));
    if( depthBuffer )
    {
        uint16_ts depths;
        const size_t size = frameSize.x( ) * frameSize.y( );
        depths.reserve( size  );
        for( size_t i = 0; i < size; ++i )
            depths.push_back(
                std::numeric_limits< uint16_t >::max() * depthBuffer[ i ] );
        _remoteFrameBuffers.setDepth(
            reinterpret_cast< const uint8_t *>( depths.data( )),
            depths.size() * sizeof( uint16_t ));
    }
    else
        _remoteFrameBuffers.setDepth( 0, 0 );

    const uint8_t* colorBuffer =
        frameBuffer.getColorBuffer();
    if( colorBuffer )
        _remoteFrameBuffers.setDiffuse(
            colorBuffer, frameSize.x() * frameSize.y() * frameBuffer.getColorDepth() );
    else
        _remoteFrameBuffers.setDiffuse( 0, 0 );

    return true;
}

bool ZeroEQPlugin::_requestSpikes()
{
#if 0
    AbstractSimulationHandlerPtr simulationHandler = _brayns.getScene().getSimulationHandler();

    SpikeSimulationHandler* spikeSimulationHandler =
        dynamic_cast< SpikeSimulationHandler * >( simulationHandler.get() );

    std::vector< uint64_t > spikeGids;
    uint64_t ts = 0.f;

    _remoteSpikes.setTimestamp( ts );
    if( spikeSimulationHandler )
    {
        uint64_t frameSize = spikeSimulationHandler->getFrameSize();
        uint64_t* gids = (uint64_t*)spikeSimulationHandler->getFrameData( ts );
        spikeGids.reserve( frameSize );
        spikeGids.assign( gids, gids + frameSize );
    }
    _remoteSpikes.setGids( spikeGids );
#endif
    return true;
}

void ZeroEQPlugin::_initializeDataSource()
{
    SceneParameters& sceneParameters =
        _brayns.getParametersManager().getSceneParameters();
    GeometryParameters& geometryParameters =
        _brayns.getParametersManager().getGeometryParameters();
    VolumeParameters& volumeParameters =
        _brayns.getParametersManager().getVolumeParameters();

    _remoteDataSource.setTransfer_function_file( sceneParameters.getTransferFunctionFilename( ));
    _remoteDataSource.setMorphology_folder( geometryParameters.getMorphologyFolder( ));
    _remoteDataSource.setNest_circuit( geometryParameters.getNESTCircuit( ));
    _remoteDataSource.setNest_report( geometryParameters.getNESTReport( ));
    _remoteDataSource.setPdb_file( geometryParameters.getPDBFile( ));
    _remoteDataSource.setPdb_folder( geometryParameters.getPDBFolder( ));
    _remoteDataSource.setXyzb_file( geometryParameters.getXYZBFile( ));
    _remoteDataSource.setMesh_folder( geometryParameters.getMeshFolder( ));
    _remoteDataSource.setCircuit_config( geometryParameters.getCircuitConfiguration( ));
    _remoteDataSource.setLoad_cache_file( geometryParameters.getLoadCacheFile( ));
    _remoteDataSource.setSave_cache_file( geometryParameters.getSaveCacheFile( ));
    _remoteDataSource.setRadius_multiplier( geometryParameters.getRadiusMultiplier( ));
    _remoteDataSource.setRadius_correction( geometryParameters.getRadiusCorrection( ));
    _remoteDataSource.setColor_scheme( geometryParameters.getColorScheme( ));
    _remoteDataSource.setScene_environment( geometryParameters.getSceneEnvironment( ));
    _remoteDataSource.setGeometry_quality( geometryParameters.getGeometryQuality( ));
    _remoteDataSource.setTarget( geometryParameters.getTarget( ));
    _remoteDataSource.setReport( geometryParameters.getReport( ));
    _remoteDataSource.setNon_simulated_cells( geometryParameters.getNonSimulatedCells( ));
    _remoteDataSource.setStart_simulation_time( geometryParameters.getStartSimulationTime( ));
    _remoteDataSource.setEnd_simulation_time( geometryParameters.getEndSimulationTime( ));
    _remoteDataSource.setSimulation_values_range( geometryParameters.getSimulationValuesRange( ));
    _remoteDataSource.setSimulation_cache_file( geometryParameters.getSimulationCacheFile( ));
    _remoteDataSource.setNest_cache_file( geometryParameters.getNESTCacheFile( ));
    _remoteDataSource.setMorphology_section_types( geometryParameters.getMorphologySectionTypes( ));
    _remoteDataSource.setMorphology_layout( geometryParameters.getMorphologyLayout().type );
    _remoteDataSource.setGenerate_multiple_models( geometryParameters.getGenerateMultipleModels( ));
    _remoteDataSource.setVolume_folder( volumeParameters.getFolder( ));
    _remoteDataSource.setVolume_file( volumeParameters.getFilename( ));
    _remoteDataSource.setVolume_dimensions( Vector3ui(volumeParameters.getDimensions( )));
    _remoteDataSource.setVolume_element_spacing( Vector3f( volumeParameters.getElementSpacing( )));
    _remoteDataSource.setVolume_offset( Vector3f(volumeParameters.getOffset( )));
    _remoteDataSource.setEnvironment_map( sceneParameters.getEnvironmentMap( ));
}

void ZeroEQPlugin::_dataSourceUpdated()
{
    BRAYNS_INFO << "Data source updated" << std::endl;

    _brayns.getParametersManager().set(
        "splash-scene-folder", ""); // Make sure the splash scene is removed
    _brayns.getParametersManager().set(
        "transfer-function-file", _remoteDataSource.getTransfer_function_fileString( ));
    _brayns.getParametersManager().set(
        "morphology-folder", _remoteDataSource.getMorphology_folderString( ));
    _brayns.getParametersManager().set(
        "nest-circuit", _remoteDataSource.getNest_circuitString( ));
    _brayns.getParametersManager().set(
        "nest-report", _remoteDataSource.getNest_reportString( ));
    _brayns.getParametersManager().set(
        "pdb-file", _remoteDataSource.getPdb_fileString( ));
    _brayns.getParametersManager().set(
        "pdb-folder", _remoteDataSource.getPdb_folderString( ));
    _brayns.getParametersManager().set(
        "xyzb-file", _remoteDataSource.getXyzb_fileString( ));
    _brayns.getParametersManager().set(
        "mesh-folder", _remoteDataSource.getMesh_folderString( ));
    _brayns.getParametersManager().set(
        "circuit-config", _remoteDataSource.getCircuit_configString( ));
    _brayns.getParametersManager().set(
        "load-cache-file", _remoteDataSource.getLoad_cache_fileString( ));
    _brayns.getParametersManager().set(
        "save-cache-file", _remoteDataSource.getSave_cache_fileString( ));
    _brayns.getParametersManager().set(
        "radius-multiplier", std::to_string(_remoteDataSource.getRadius_multiplier( )));
    _brayns.getParametersManager().set(
        "radius-correction", std::to_string(_remoteDataSource.getRadius_correction( )));
    _brayns.getParametersManager().set(
        "color-scheme", std::to_string(_remoteDataSource.getColor_scheme( )));
    _brayns.getParametersManager().set(
        "scene-environment", std::to_string(_remoteDataSource.getScene_environment( )));
    _brayns.getParametersManager().set(
        "geometry-quality", std::to_string(_remoteDataSource.getGeometry_quality( )));
    _brayns.getParametersManager().set(
        "target", _remoteDataSource.getTargetString( ));
    _brayns.getParametersManager().set(
        "report", _remoteDataSource.getReportString( ));
    _brayns.getParametersManager().set(
        "non-simulated-cells", std::to_string(_remoteDataSource.getNon_simulated_cells( )));
    _brayns.getParametersManager().set(
        "start-simulation-time", std::to_string(_remoteDataSource.getStart_simulation_time( )));
    _brayns.getParametersManager().set(
        "end-simulation-time", std::to_string(_remoteDataSource.getEnd_simulation_time( )));
    _brayns.getParametersManager().set(
        "simulation-values-range",
        std::to_string(_remoteDataSource.getSimulation_values_range()[0]) + " " +
        std::to_string(_remoteDataSource.getSimulation_values_range()[1]) );
    _brayns.getParametersManager().set(
        "simulation-cache-file", _remoteDataSource.getSimulation_cache_fileString( ));
    _brayns.getParametersManager().set(
        "nest-cache-file", _remoteDataSource.getNest_cache_fileString( ));
    _brayns.getParametersManager().set(
        "morphology-section-types",
        std::to_string(_remoteDataSource.getMorphology_section_types( )));
    _brayns.getParametersManager().set(
        "morphology-layout", std::to_string(_remoteDataSource.getMorphology_layout( )));
    _brayns.getParametersManager().set(
        "generate-multiple-models", (_remoteDataSource.getGenerate_multiple_models( ) ? "1" : "0"));
    _brayns.getParametersManager().set(
        "volume-folder", _remoteDataSource.getVolume_folderString( ));
    _brayns.getParametersManager().set(
        "volume-file", _remoteDataSource.getVolume_fileString( ));
    _brayns.getParametersManager().set(
        "volume-dimensions",
        std::to_string(_remoteDataSource.getVolume_dimensions()[0]) + " " +
        std::to_string(_remoteDataSource.getVolume_dimensions()[1]) + " " +
        std::to_string(_remoteDataSource.getVolume_dimensions()[2]) );
    _brayns.getParametersManager().set(
        "volume-element-spacing",
        std::to_string(_remoteDataSource.getVolume_element_spacing()[0]) + " " +
        std::to_string(_remoteDataSource.getVolume_element_spacing()[1]) + " " +
        std::to_string(_remoteDataSource.getVolume_element_spacing()[2]) );
    _brayns.getParametersManager().set(
        "volume-offset",
        std::to_string(_remoteDataSource.getVolume_offset()[0]) + " " +
        std::to_string(_remoteDataSource.getVolume_offset()[1]) + " " +
        std::to_string(_remoteDataSource.getVolume_offset()[2]) );
    _brayns.getParametersManager().set(
        "environment-map", _remoteDataSource.getEnvironment_mapString( ));

    _brayns.getScene().commitVolumeData();
    _brayns.getRenderer().commit();
    _brayns.getFrameBuffer().clear();
    _brayns.getParametersManager().print();

    _resetSceneUpdated();
    _resetCameraUpdated();
}

void ZeroEQPlugin::_initializeSettings()
{
    SceneParameters& sceneParameters =
        _brayns.getParametersManager().getSceneParameters();
    RenderingParameters& renderingParameters =
        _brayns.getParametersManager().getRenderingParameters();
    VolumeParameters& volumeParameters =
        _brayns.getParametersManager().getVolumeParameters();
    auto& applicationParameters =
        _brayns.getParametersManager().getApplicationParameters();

    _remoteSettings.setTimestamp( sceneParameters.getTimestamp( ));
    _remoteSettings.setVolume_samples_per_ray( volumeParameters.getSamplesPerRay( ));
    if( renderingParameters.getRenderer( ) == "exobj" )
        _remoteSettings.setShader( Shader::basic );
     else if( renderingParameters.getRenderer( ) == "proximityrenderer" )
        _remoteSettings.setShader( Shader::proximity );
     else if( renderingParameters.getRenderer( ) == "particlerenderer" )
        _remoteSettings.setShader( Shader::particle );
     else if( renderingParameters.getRenderer( ) == "simulationrenderer" )
        _remoteSettings.setShader( Shader::simulation );

    switch( renderingParameters.getShading( ) )
    {
        case ShadingType::diffuse: _remoteSettings.setShading( Shading::diffuse ); break;
        case ShadingType::electron: _remoteSettings.setShading( Shading::electron ); break;
        default: _remoteSettings.setShading( Shading::none ); break;
    }
    _remoteSettings.setSamples_per_pixel( renderingParameters.getSamplesPerPixel( ));
    _remoteSettings.setAmbient_occlusion( renderingParameters.getAmbientOcclusionStrength( ));
    _remoteSettings.setShadows( renderingParameters.getShadows( ));
    _remoteSettings.setSoft_shadows( renderingParameters.getSoftShadows( ));
    _remoteSettings.setRadiance( renderingParameters.getLightEmittingMaterials( ));
    Vector3f value = renderingParameters.getBackgroundColor();
    _remoteSettings.setBackground_color( value );
    _remoteSettings.setDetection_distance( renderingParameters.getDetectionDistance( ));
    value = renderingParameters.getDetectionNearColor();
    _remoteSettings.setDetection_near_color( value );
    value = renderingParameters.getDetectionFarColor();
    _remoteSettings.setDetection_far_color( value );
    _remoteSettings.setEpsilon( renderingParameters.getEpsilon( ));
    _remoteSettings.setHead_light( renderingParameters.getHeadLight( ));
    _remoteSettings.setJpeg_compression( applicationParameters.getJpegCompression( ));
    const auto& jpegSize = applicationParameters.getJpegSize();
    _remoteSettings.setJpeg_size( { jpegSize[0], jpegSize[1] } );
}

void ZeroEQPlugin::_settingsUpdated()
{
    BRAYNS_INFO << "Settings updated" << std::endl;

    _brayns.getParametersManager().set(
        "timestamp", std::to_string(_remoteSettings.getTimestamp( )));
    _brayns.getParametersManager().set(
        "volume-samples-per-ray", std::to_string(_remoteSettings.getVolume_samples_per_ray( )));
    switch( _remoteSettings.getShader( ))
    {
        case brayns::Shader::proximity:
            _brayns.getParametersManager().set( "renderer", "proximityrenderer"); break;
        case brayns::Shader::particle:
            _brayns.getParametersManager().set( "renderer", "particlerenderer"); break;
        case brayns::Shader::simulation:
            _brayns.getParametersManager().set( "renderer", "simulationrenderer"); break;
        default:
            _brayns.getParametersManager().set( "renderer", "exobj"); break;
    }
    switch( _remoteSettings.getShading( ))
    {
        case brayns::Shading::diffuse:
            _brayns.getParametersManager().set( "shading", "diffuse"); break;
        case brayns::Shading::electron:
            _brayns.getParametersManager().set( "shading", "electron"); break;
        default:
            _brayns.getParametersManager().set( "shading", "none"); break;
    }
    _brayns.getParametersManager().set(
        "spp", std::to_string(_remoteSettings.getSamples_per_pixel( )));
    _brayns.getParametersManager().set(
        "ambient-occlusion", std::to_string(_remoteSettings.getAmbient_occlusion()));
    _brayns.getParametersManager().set(
        "shadows", (_remoteSettings.getShadows( ) ? "1" : "0"));
    _brayns.getParametersManager().set(
        "soft-shadows", (_remoteSettings.getSoft_shadows( ) ? "1" : "0"));
    _brayns.getParametersManager().set(
        "radiance", (_remoteSettings.getRadiance( ) ? "1" : "0"));
    _brayns.getParametersManager().set(
        "background-color",
        std::to_string(_remoteSettings.getBackground_color()[0]) + " " +
        std::to_string(_remoteSettings.getBackground_color()[1]) + " " +
        std::to_string(_remoteSettings.getBackground_color()[2]) );
    _brayns.getParametersManager().set(
        "detection-distance", std::to_string(_remoteSettings.getDetection_distance( )));
    _brayns.getParametersManager().set(
        "detection-on-different-material",
        (_remoteSettings.getDetection_on_different_material() ? "1" : "0"));
    _brayns.getParametersManager().set(
        "detection-near-color",
        std::to_string(_remoteSettings.getDetection_near_color()[0]) + " " +
        std::to_string(_remoteSettings.getDetection_near_color()[1]) + " " +
        std::to_string(_remoteSettings.getDetection_near_color()[2]) );
    _brayns.getParametersManager().set(
        "detection-far-color",
        std::to_string(_remoteSettings.getDetection_far_color()[0]) + " " +
        std::to_string(_remoteSettings.getDetection_far_color()[1]) + " " +
        std::to_string(_remoteSettings.getDetection_far_color()[2]) );
    _brayns.getParametersManager().set(
        "epsilon", std::to_string(_remoteSettings.getEpsilon( )));
    _brayns.getParametersManager().set(
        "head-light", (_remoteSettings.getHead_light( ) ? "1" : "0"));

    auto& app = _brayns.getParametersManager().getApplicationParameters();
    app.setJpegSize( Vector2ui{ _remoteSettings.getJpeg_size()  } );
    app.setJpegCompression( std::min( _remoteSettings.getJpeg_compression(), 100u ));

    _brayns.getRenderer().commit();
    _brayns.getFrameBuffer().clear();
}

bool ZeroEQPlugin::_requestFrame()
{
    auto simHandler = _brayns.getScene().getSimulationHandler();
    const uint64_t nbFrames = simHandler ? simHandler->getNbFrames() : 0;

    const auto ts = uint64_t(_brayns.getParametersManager().getSceneParameters().getTimestamp());
    _remoteFrame.setCurrent( nbFrames == 0 ? 0 : (ts % nbFrames) );
    _remoteFrame.setDelta( 1 );
    _remoteFrame.setEnd( nbFrames );
    _remoteFrame.setStart( 0 );
    return true;
}

void ZeroEQPlugin::_frameUpdated()
{
    _brayns.getParametersManager().getSceneParameters().setTimestamp( _remoteFrame.getCurrent( ));
    _brayns.commit();
}

bool ZeroEQPlugin::_requestViewport()
{
    const auto& windowSize = _brayns.getParametersManager().getApplicationParameters().getWindowSize();
    _remoteViewport.setSize( { windowSize[0], windowSize[1] } );
    return true;
}

void ZeroEQPlugin::_viewportUpdated()
{
    _brayns.getParametersManager().getApplicationParameters().setWindowSize( Vector2ui{ _remoteViewport.getSize() } );
    _brayns.commit();
}

uint8_t* ZeroEQPlugin::_encodeJpeg(const uint32_t width,
                     const uint32_t height,
                     const uint8_t* rawData,
                     unsigned long& dataSize)
{
    uint8_t* tjSrcBuffer = const_cast< uint8_t* >( rawData );
    const int32_t pixelFormat = TJPF_RGBA;
    const int32_t color_components = 4; // Color Depth
    const int32_t tjPitch = width * color_components;
    const int32_t tjPixelFormat = pixelFormat;

    uint8_t* tjJpegBuf = 0;
    const int32_t tjJpegSubsamp = TJSAMP_444;
    const int32_t tjFlags = TJXOP_ROT180;

    const int32_t success = tjCompress2(
        _compressor, tjSrcBuffer, width, tjPitch, height,
        tjPixelFormat, &tjJpegBuf, &dataSize, tjJpegSubsamp,
        _brayns.getParametersManager().getApplicationParameters().getJpegCompression(),
        tjFlags);

    if(success != 0)
    {
        BRAYNS_ERROR << "libjpeg-turbo image conversion failure" << std::endl;
        return 0;
    }
    return static_cast<uint8_t *>(tjJpegBuf);
}

}

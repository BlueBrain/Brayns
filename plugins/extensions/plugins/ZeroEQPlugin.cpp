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

#include "ZeroEQPlugin.h"

#include <brayns/Brayns.h>
#include <brayns/common/camera/Camera.h>
#include <brayns/common/engine/Engine.h>
#include <brayns/common/renderer/FrameBuffer.h>
#include <brayns/common/renderer/Renderer.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/common/simulation/AbstractSimulationHandler.h>
#include <brayns/common/simulation/SpikeSimulationHandler.h>
#include <brayns/common/volume/VolumeHandler.h>
#include <brayns/parameters/ParametersManager.h>
#include <zerobuf/render/camera.h>

#include <brayns/version.h>


namespace brayns
{

ZeroEQPlugin::ZeroEQPlugin(
    Engine& engine,
    ParametersManager& parametersManager )
    : ExtensionPlugin( engine )
    , _parametersManager( parametersManager )
    , _compressor( tjInitCompress() )
    , _processingImageJpeg( false )
{
    _setupHTTPServer();
    _setupRequests();
    _setupSubscriber();
    _initializeDataSource();
    _initializeSettings();

    engine.extensionInit( *this );
}

ZeroEQPlugin::~ZeroEQPlugin( )
{
    if( _compressor )
        tjDestroy( _compressor );

    if( _httpServer )
    {
        _httpServer->remove( *_engine.getCamera().getSerializable() );
        _httpServer->remove( _remoteImageJPEG );
    }
}

void ZeroEQPlugin::run()
{
    if( _requestVolumeHistogram( ))
        _publisher.publish( _remoteVolumeHistogram );

    if( _requestFrame( ))
        _publisher.publish( _remoteFrame );

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

void ZeroEQPlugin::handle( servus::Serializable& object )
{
    if( _httpServer )
        _httpServer->handle( object );
    _subscriber.subscribe( object );
    _requests[ object.getTypeIdentifier() ] =
        [&]{ return _publisher.publish( object ); };

    // publish updates from HTTP to subscribers, e.g. livreGUI
    object.registerDeserializedCallback( [&] { _publisher.publish( object ); });
}

void ZeroEQPlugin::_setupHTTPServer()
{
    const strings& arguments = _parametersManager.getApplicationParameters().arguments();
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
    servus::Serializable& cam = *_engine.getCamera().getSerializable();
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

    _httpServer->handleGET( "brayns/v1/simulation-histogram", _remoteSimulationHistogram );
    _remoteSimulationHistogram.registerSerializeCallback(
        std::bind( &ZeroEQPlugin::_requestSimulationHistogram, this ));

    _httpServer->handleGET( "brayns/v1/volume-histogram", _remoteVolumeHistogram );
    _remoteVolumeHistogram.registerSerializeCallback(
        std::bind( &ZeroEQPlugin::_requestVolumeHistogram, this ));
}

void ZeroEQPlugin::_setupRequests()
{
    _requests[ ::lexis::render::ImageJPEG::ZEROBUF_TYPE_IDENTIFIER() ] =
        [&]{ _requestImageJPEG(); return _publisher.publish( _remoteImageJPEG ); };

    _requests[ ::lexis::render::Frame::ZEROBUF_TYPE_IDENTIFIER() ] =
        [&]{ _requestFrame(); return _publisher.publish( _remoteFrame ); };

    _requests[ ::brayns::v1::Camera::ZEROBUF_TYPE_IDENTIFIER() ] =
        [&]{ return _publisher.publish( *_engine.getCamera().getSerializable( )); };

    _requests[ v1::FrameBuffers::ZEROBUF_TYPE_IDENTIFIER() ] =
        [&]{ _requestFrameBuffers(); return _publisher.publish( _remoteFrameBuffers ); };

    _requests[ v1::TransferFunction1D::ZEROBUF_TYPE_IDENTIFIER() ] =
        [&]{ _requestTransferFunction1D(); return _publisher.publish( _remoteTransferFunction1D ); };

    _requests[ v1::Spikes::ZEROBUF_TYPE_IDENTIFIER() ] =
        [&]{ _requestSpikes(); return _publisher.publish( _remoteSpikes ); };
}

void ZeroEQPlugin::_setupSubscriber()
{
    _subscriber.subscribe( _remoteLookupTable1D );
    _subscriber.subscribe( _remoteFrame );

    _remoteLookupTable1D.registerDeserializedCallback(
        std::bind( &ZeroEQPlugin::_LookupTable1DUpdated, this ));
    _remoteLookupTable1D.registerSerializeCallback(
        std::bind( &ZeroEQPlugin::_requestLookupTable1D, this ));
}

void ZeroEQPlugin::_cameraUpdated()
{
    _engine.getFrameBuffer().clear();
    _engine.getCamera().commit();
}

void ZeroEQPlugin::_attributeUpdated( )
{
    BRAYNS_INFO << _remoteAttribute.getKeyString() << " = " <<
        _remoteAttribute.getValueString() << std::endl;
    _parametersManager.set( _remoteAttribute.getKeyString(), _remoteAttribute.getValueString( ));
    _engine.getScene().commitVolumeData();
    _engine.getRenderer().commit();
    _engine.getFrameBuffer().clear();
}

void ZeroEQPlugin::_resetCameraUpdated()
{
    auto& sceneParameters = _parametersManager.getSceneParameters();
    _engine.getCamera().setEnvironmentMap( !sceneParameters.getEnvironmentMap().empty( ));
    _engine.getCamera().reset();
    _engine.getCamera().commit();
    _engine.getFrameBuffer().clear();
}

void ZeroEQPlugin::_resetSceneUpdated()
{
    _engine.makeDirty();
}

bool ZeroEQPlugin::_requestScene()
{
    auto& ms = _remoteScene.getMaterials();
    ms.clear();
    auto& scene = _engine.getScene();
    const auto& materials = scene.getMaterials();

    for( const auto& material: materials )
    {
        ::brayns::v1::Material m;
        m.setDiffuseColor( material->getColor( ));
        m.setSpecularColor( material->getSpecularColor( ));
        m.setSpecularExponent( material->getSpecularExponent( ));
        m.setReflectionIndex( material->getReflectionIndex( ));
        m.setOpacity( material->getOpacity( ));
        m.setRefractionIndex( material->getRefractionIndex( ));
        m.setLightEmission( material->getEmission( ));
        ms.push_back(m);
    }
    return true;
}

void ZeroEQPlugin::_sceneUpdated( )
{
    auto& materials = _remoteScene.getMaterials();
    auto& scene = _engine.getScene();

    for( size_t materialId = 0; materialId < materials.size(); ++materialId )
    {
        MaterialPtr material = scene.getMaterial( materialId );

        if( material)
        {
            ::brayns::v1::Material& m = materials[ materialId ];
            const floats& diffuse = m.getDiffuseColorVector();
            Vector3f kd = { diffuse[0], diffuse[1], diffuse[2] };
            material->setColor( kd );

            const floats& specular = m.getSpecularColorVector();
            Vector3f ks = { specular[0], specular[1], specular[2] };
            material->setSpecularColor( ks );

            material->setSpecularExponent( m.getSpecularExponent() );
            material->setReflectionIndex( m.getReflectionIndex() );
            material->setOpacity( m.getOpacity() );
            material->setRefractionIndex( m.getRefractionIndex() );
            material->setEmission( m.getLightEmission() );
        }
    }
    scene.commitMaterials( true );
    _engine.getRenderer().commit();
    _engine.getFrameBuffer().clear();
}

void ZeroEQPlugin::_spikesUpdated( )
{
#if 0
    AbstractSimulationHandlerPtr simulationHandler =
        _engine.getScene().getSimulationHandler();

    SpikeSimulationHandler* spikeSimulationHandler =
        dynamic_cast< SpikeSimulationHandler * >(simulationHandler.get());

    if( spikeSimulationHandler )
    {
        uint64_t ts = _remoteSpikes.getTimestamp();
        float* data = (float*)spikeSimulationHandler->getFrameData( ts );
        for( const auto& gid: _remoteSpikes.getGidsVector() )
            data[gid] = ts;

        _engine.getFrameBuffer().clear();
        _engine.getScene().commitSimulationData();
    }
#endif
}


bool ZeroEQPlugin::_requestTransferFunction1D()
{
    auto& scene = _engine.getScene();
    TransferFunction& transferFunction = scene.getTransferFunction();
    std::vector< ::brayns::v1::Point2D > items;

    for( const auto& controlPoint: transferFunction.getControlPoints( TF_RED ) )
        items.push_back( ::brayns::v1::Point2D(
            controlPoint.x(), controlPoint.y() ));
    _remoteTransferFunction1D.setAttribute( transferFunction.getAttributeAsString( TF_RED ) );
    _remoteTransferFunction1D.setPoints( items );
    return true;
}

void ZeroEQPlugin::_transferFunction1DUpdated( )
{
    auto& scene = _engine.getScene();
    TransferFunction& transferFunction = scene.getTransferFunction();
    std::vector< ::brayns::v1::Point2D > points = _remoteTransferFunction1D.getPointsVector();

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
    _engine.getRenderer().commit();
    _engine.getFrameBuffer().clear();
}

void ZeroEQPlugin::_LookupTable1DUpdated( )
{
    auto& scene = _engine.getScene();
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
    _engine.getFrameBuffer().clear();
}

bool ZeroEQPlugin::_requestLookupTable1D( )
{
    auto& scene = _engine.getScene();
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
    auto& scene = _engine.getScene();
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
    _engine.getFrameBuffer().clear();
}

bool ZeroEQPlugin::_requestColormap( )
{
    auto& scene = _engine.getScene();
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
            _parametersManager.getApplicationParameters().getJpegSize();
        if( newFrameSize.x() == 0 || newFrameSize.y() == 0 )
        {
            BRAYNS_ERROR << "Encountered invalid size of image JPEG: "
                         << newFrameSize << std::endl;
            _processingImageJpeg = false;
            return false;
        }

        FrameBuffer& frameBuffer = _engine.getFrameBuffer();
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

            int32_t pixelFormat = TJPF_RGBA;
            switch( frameBuffer.getFrameBufferFormat( ))
            {
            case FrameBufferFormat::FBF_BGRA_I8:
                pixelFormat = TJPF_BGRA;
                break;
            case FrameBufferFormat::FBF_RGB_I8:
                pixelFormat = TJPF_RGB;
                break;
            default:
                pixelFormat = TJPF_RGBA;
            }

            unsigned long jpegSize =
                newFrameSize.x( ) * newFrameSize.y( ) * sizeof( unsigned long );
            uint8_t* jpegData = _encodeJpeg(
                    ( uint32_t )newFrameSize.x( ),
                    ( uint32_t )newFrameSize.y( ),
                    ( uint8_t* )resizedColorBuffer, pixelFormat, jpegSize );

            _remoteImageJPEG.setData( jpegData, jpegSize  );
            tjFree(jpegData);
        }
        _processingImageJpeg = false;
    }
    return true;
}

bool ZeroEQPlugin::_requestFrameBuffers()
{
    auto& frameBuffer = _engine.getFrameBuffer();
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
    AbstractSimulationHandlerPtr simulationHandler = _engine.getScene().getSimulationHandler();

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
    auto& sceneParameters = _parametersManager.getSceneParameters();
    auto& geometryParameters = _parametersManager.getGeometryParameters();
    auto& volumeParameters = _parametersManager.getVolumeParameters();

    _remoteDataSource.setTransferFunctionFile( sceneParameters.getColorMapFilename( ));
    _remoteDataSource.setMorphologyFolder( geometryParameters.getMorphologyFolder( ));
    _remoteDataSource.setNestCircuit( geometryParameters.getNESTCircuit( ));
    _remoteDataSource.setNestReport( geometryParameters.getNESTReport( ));
    _remoteDataSource.setPdbFile( geometryParameters.getPDBFile( ));
    _remoteDataSource.setPdbFolder( geometryParameters.getPDBFolder( ));
    _remoteDataSource.setXyzbFile( geometryParameters.getXYZBFile( ));
    _remoteDataSource.setMeshFolder( geometryParameters.getMeshFolder( ));
    _remoteDataSource.setCircuitConfig( geometryParameters.getCircuitConfiguration( ));
    _remoteDataSource.setLoadCacheFile( geometryParameters.getLoadCacheFile( ));
    _remoteDataSource.setSaveCacheFile( geometryParameters.getSaveCacheFile( ));
    _remoteDataSource.setRadiusMultiplier( geometryParameters.getRadiusMultiplier( ));
    _remoteDataSource.setRadiusCorrection( geometryParameters.getRadiusCorrection( ));
    _remoteDataSource.setColorScheme(
        ::brayns::v1::ColorScheme(geometryParameters.getColorScheme()));
    _remoteDataSource.setSceneEnvironment(
        ::brayns::v1::SceneEnvironment( geometryParameters.getSceneEnvironment( )));
    _remoteDataSource.setGeometryQuality(
        ::brayns::v1::GeometryQuality( geometryParameters.getGeometryQuality( )));
    _remoteDataSource.setTarget( geometryParameters.getTarget( ));
    _remoteDataSource.setReport( geometryParameters.getReport( ));
    _remoteDataSource.setNonSimulatedCells( geometryParameters.getNonSimulatedCells( ));
    _remoteDataSource.setStartSimulationTime( geometryParameters.getStartSimulationTime( ));
    _remoteDataSource.setEndSimulationTime( geometryParameters.getEndSimulationTime( ));
    _remoteDataSource.setSimulationValuesRange( geometryParameters.getSimulationValuesRange( ));
    _remoteDataSource.setSimulationCacheFile( geometryParameters.getSimulationCacheFile( ));
    _remoteDataSource.setNestCacheFile( geometryParameters.getNESTCacheFile( ));

    const auto mst = geometryParameters.getMorphologySectionTypes();
    std::vector< ::brayns::v1::SectionType > sectionTypes;
    const auto all = mst & (size_t)::brayns::v1::SectionType::all;
    if( all || mst & (size_t)::brayns::v1::SectionType::soma )
        sectionTypes.push_back( ::brayns::v1::SectionType::soma );
    if( all || mst & (size_t)::brayns::v1::SectionType::axon )
        sectionTypes.push_back( ::brayns::v1::SectionType::axon );
    if( all || mst & (size_t)::brayns::v1::SectionType::dendrite )
        sectionTypes.push_back( ::brayns::v1::SectionType::dendrite );
    if( all || mst & (size_t)::brayns::v1::SectionType::apical_dendrite )
        sectionTypes.push_back( ::brayns::v1::SectionType::apical_dendrite );
    _remoteDataSource.setMorphologySectionTypes( sectionTypes );

    const auto& morphologyLayout = geometryParameters.getMorphologyLayout();
    brayns::v1::MorphologyLayout remoteMorphologyLayout(
        morphologyLayout.nbColumns,
        morphologyLayout.verticalSpacing,
        morphologyLayout.horizontalSpacing );
    _remoteDataSource.setMorphologyLayout( remoteMorphologyLayout );

    _remoteDataSource.setGenerateMultipleModels( geometryParameters.getGenerateMultipleModels( ));
    _remoteDataSource.setVolumeFolder( volumeParameters.getFolder( ));
    _remoteDataSource.setVolumeFile( volumeParameters.getFilename( ));
    _remoteDataSource.setVolumeDimensions( Vector3ui(volumeParameters.getDimensions( )));
    _remoteDataSource.setVolumeElementSpacing( Vector3f( volumeParameters.getElementSpacing( )));
    _remoteDataSource.setVolumeOffset( Vector3f(volumeParameters.getOffset( )));
    _remoteDataSource.setEnvironmentMap( sceneParameters.getEnvironmentMap( ));
    _remoteDataSource.setMolecularSystemConfig( geometryParameters.getMolecularSystemConfig( ));
}

void ZeroEQPlugin::_dataSourceUpdated()
{
    auto& geometryParameters = _parametersManager.getGeometryParameters();

    _parametersManager.set(
        "splash-scene-folder", ""); // Make sure the splash scene is removed
    _parametersManager.set(
        "transfer-function-file", _remoteDataSource.getTransferFunctionFileString( ));
    _parametersManager.set(
        "morphology-folder", _remoteDataSource.getMorphologyFolderString( ));
    _parametersManager.set(
        "nest-circuit", _remoteDataSource.getNestCircuitString( ));
    _parametersManager.set(
        "nest-report", _remoteDataSource.getNestReportString( ));
    _parametersManager.set(
        "pdb-file", _remoteDataSource.getPdbFileString( ));
    _parametersManager.set(
        "pdb-folder", _remoteDataSource.getPdbFolderString( ));
    _parametersManager.set(
        "xyzb-file", _remoteDataSource.getXyzbFileString( ));
    _parametersManager.set(
        "mesh-folder", _remoteDataSource.getMeshFolderString( ));
    _parametersManager.set(
        "circuit-config", _remoteDataSource.getCircuitConfigString( ));
    _parametersManager.set(
        "load-cache-file", _remoteDataSource.getLoadCacheFileString( ));
    _parametersManager.set(
        "save-cache-file", _remoteDataSource.getSaveCacheFileString( ));
    _parametersManager.set(
        "radius-multiplier", std::to_string(_remoteDataSource.getRadiusMultiplier( )));
    _parametersManager.set(
        "radius-correction", std::to_string(_remoteDataSource.getRadiusCorrection( )));
    _parametersManager.set(
        "color-scheme", geometryParameters.getColorSchemeAsString(
        static_cast< ColorScheme >( _remoteDataSource.getColorScheme( ))));
    _parametersManager.set(
        "scene-environment", geometryParameters.getSceneEnvironmentAsString(
        static_cast< SceneEnvironment >( _remoteDataSource.getSceneEnvironment( ))));
    _parametersManager.set(
        "geometry-quality", geometryParameters.getGeometryQualityAsString(
        static_cast< GeometryQuality >( _remoteDataSource.getGeometryQuality( ))));
    _parametersManager.set(
        "target", _remoteDataSource.getTargetString( ));
    _parametersManager.set(
        "report", _remoteDataSource.getReportString( ));
    _parametersManager.set(
        "non-simulated-cells", std::to_string(_remoteDataSource.getNonSimulatedCells( )));
    _parametersManager.set(
        "start-simulation-time", std::to_string(_remoteDataSource.getStartSimulationTime( )));
    _parametersManager.set(
        "end-simulation-time", std::to_string(_remoteDataSource.getEndSimulationTime( )));
    _parametersManager.set(
        "simulation-values-range",
        std::to_string(_remoteDataSource.getSimulationValuesRange()[0]) + " " +
        std::to_string(_remoteDataSource.getSimulationValuesRange()[1]) );
    _parametersManager.set(
        "simulation-cache-file", _remoteDataSource.getSimulationCacheFileString( ));
    _parametersManager.set(
        "nest-cache-file", _remoteDataSource.getNestCacheFileString( ));

    uint morphologySectionTypes = MST_UNDEFINED;
    const auto sectionTypes = _remoteDataSource.getMorphologySectionTypesVector();
    for( const auto& sectionType: sectionTypes )
    {
        switch( sectionType )
        {
        case ::brayns::v1::SectionType::soma:
            morphologySectionTypes |= MST_SOMA; break;
        case ::brayns::v1::SectionType::axon:
            morphologySectionTypes |= MST_AXON; break;
        case ::brayns::v1::SectionType::dendrite:
            morphologySectionTypes |= MST_DENDRITE; break;
        case ::brayns::v1::SectionType::apical_dendrite:
            morphologySectionTypes |= MST_APICAL_DENDRITE; break;
        case ::brayns::v1::SectionType::all:
            morphologySectionTypes |= MST_ALL;
        }
    }
    _parametersManager.set( "morphology-section-types", std::to_string( morphologySectionTypes ));

    const auto remoteMorphologyLayout = _remoteDataSource.getMorphologyLayout();
    std::string layoutAsString;
    layoutAsString += std::to_string( remoteMorphologyLayout.getNbColumns( ));
    layoutAsString += " " + std::to_string( remoteMorphologyLayout.getVerticalSpacing( ));
    layoutAsString += " " + std::to_string( remoteMorphologyLayout.getHorizontalSpacing( ));
    BRAYNS_ERROR << "morphology-layout: " << layoutAsString << std::endl;
    _parametersManager.set( "morphology-layout", layoutAsString );

    _parametersManager.set(
        "generate-multiple-models", (_remoteDataSource.getGenerateMultipleModels( ) ? "1" : "0"));
    _parametersManager.set(
        "volume-folder", _remoteDataSource.getVolumeFolderString( ));
    _parametersManager.set(
        "volume-file", _remoteDataSource.getVolumeFileString( ));
    _parametersManager.set(
        "volume-dimensions",
        std::to_string(_remoteDataSource.getVolumeDimensions()[0]) + " " +
        std::to_string(_remoteDataSource.getVolumeDimensions()[1]) + " " +
        std::to_string(_remoteDataSource.getVolumeDimensions()[2]) );
    _parametersManager.set(
        "volume-element-spacing",
        std::to_string(_remoteDataSource.getVolumeElementSpacing()[0]) + " " +
        std::to_string(_remoteDataSource.getVolumeElementSpacing()[1]) + " " +
        std::to_string(_remoteDataSource.getVolumeElementSpacing()[2]) );
    _parametersManager.set(
        "volume-offset",
        std::to_string(_remoteDataSource.getVolumeOffset()[0]) + " " +
        std::to_string(_remoteDataSource.getVolumeOffset()[1]) + " " +
        std::to_string(_remoteDataSource.getVolumeOffset()[2]) );
    _parametersManager.set(
        "environment-map", _remoteDataSource.getEnvironmentMapString( ));

    _parametersManager.set(
        "molecular-system-config", _remoteDataSource.getMolecularSystemConfigString( ));

    _resetSceneUpdated();
    _resetCameraUpdated();

    _engine.getFrameBuffer().clear();
    _engine.getScene().commitSimulationData();
    _engine.getScene().commitVolumeData();
    _engine.getRenderer().commit();
    _parametersManager.print();
}

void ZeroEQPlugin::_initializeSettings()
{
    auto& sceneParameters = _parametersManager.getSceneParameters();
    auto& renderingParameters = _parametersManager.getRenderingParameters();
    auto& volumeParameters = _parametersManager.getVolumeParameters();
    auto& applicationParameters = _parametersManager.getApplicationParameters();

    _remoteSettings.setTimestamp( sceneParameters.getTimestamp( ));
    _remoteSettings.setVolumeSamplesPerRay( volumeParameters.getSamplesPerRay( ));

    switch( renderingParameters.getRenderer( ))
    {
    case RendererType::proximity:
        _remoteSettings.setShader( ::brayns::v1::Shader::proximity ); break;
    case RendererType::particle:
        _remoteSettings.setShader( ::brayns::v1::Shader::particle ); break;
    case RendererType::simulation:
        _remoteSettings.setShader( ::brayns::v1::Shader::simulation ); break;
    default:
        _remoteSettings.setShader( ::brayns::v1::Shader::basic ); break;
    }

    switch( renderingParameters.getShading( ) )
    {
        case ShadingType::diffuse:
            _remoteSettings.setShading( ::brayns::v1::Shading::diffuse ); break;
        case ShadingType::electron:
            _remoteSettings.setShading( ::brayns::v1::Shading::electron ); break;
        default:
            _remoteSettings.setShading( ::brayns::v1::Shading::none ); break;
    }
    _remoteSettings.setSamplesPerPixel( renderingParameters.getSamplesPerPixel( ));
    _remoteSettings.setAmbientOcclusion( renderingParameters.getAmbientOcclusionStrength( ));
    _remoteSettings.setShadows( renderingParameters.getShadows( ));
    _remoteSettings.setSoftShadows( renderingParameters.getSoftShadows( ));
    _remoteSettings.setRadiance( renderingParameters.getLightEmittingMaterials( ));
    Vector3f value = renderingParameters.getBackgroundColor();
    _remoteSettings.setBackgroundColor( value );
    _remoteSettings.setDetectionDistance( renderingParameters.getDetectionDistance( ));
    value = renderingParameters.getDetectionNearColor();
    _remoteSettings.setDetectionNearColor( value );
    value = renderingParameters.getDetectionFarColor();
    _remoteSettings.setDetectionFarColor( value );
    _remoteSettings.setEpsilon( renderingParameters.getEpsilon( ));
    _remoteSettings.setHeadLight( renderingParameters.getHeadLight( ));
    _remoteSettings.setJpegCompression( applicationParameters.getJpegCompression( ));
    const auto& jpegSize = applicationParameters.getJpegSize();
    _remoteSettings.setJpegSize( { jpegSize[0], jpegSize[1] } );
}

void ZeroEQPlugin::_settingsUpdated()
{
    const auto& renderingParameters = _parametersManager.getRenderingParameters();

    _parametersManager.set(
        "timestamp", std::to_string(_remoteSettings.getTimestamp( )));
    _parametersManager.set(
        "volume-samples-per-ray", std::to_string(_remoteSettings.getVolumeSamplesPerRay( )));
    _parametersManager.set( "renderer", renderingParameters.getRendererAsString(
        static_cast< RendererType >( _remoteSettings.getShader( ))));
    switch( _remoteSettings.getShading( ))
    {
        case ::brayns::v1::Shading::diffuse:
            _parametersManager.set( "shading", "diffuse"); break;
        case ::brayns::v1::Shading::electron:
            _parametersManager.set( "shading", "electron"); break;
        default:
            _parametersManager.set( "shading", "none"); break;
    }
    _parametersManager.set(
        "spp", std::to_string(_remoteSettings.getSamplesPerPixel( )));
    _parametersManager.set(
        "ambient-occlusion", std::to_string(_remoteSettings.getAmbientOcclusion()));
    _parametersManager.set(
        "shadows", (_remoteSettings.getShadows( ) ? "1" : "0"));
    _parametersManager.set(
        "soft-shadows", (_remoteSettings.getSoftShadows( ) ? "1" : "0"));
    _parametersManager.set(
        "radiance", (_remoteSettings.getRadiance( ) ? "1" : "0"));
    _parametersManager.set(
        "background-color",
        std::to_string(_remoteSettings.getBackgroundColor()[0]) + " " +
        std::to_string(_remoteSettings.getBackgroundColor()[1]) + " " +
        std::to_string(_remoteSettings.getBackgroundColor()[2]) );
    _parametersManager.set(
        "detection-distance", std::to_string(_remoteSettings.getDetectionDistance( )));
    _parametersManager.set(
        "detection-on-different-material",
        (_remoteSettings.getDetectionOnDifferentMaterial() ? "1" : "0"));
    _parametersManager.set(
        "detection-near-color",
        std::to_string(_remoteSettings.getDetectionNearColor()[0]) + " " +
        std::to_string(_remoteSettings.getDetectionNearColor()[1]) + " " +
        std::to_string(_remoteSettings.getDetectionNearColor()[2]) );
    _parametersManager.set(
        "detection-far-color",
        std::to_string(_remoteSettings.getDetectionFarColor()[0]) + " " +
        std::to_string(_remoteSettings.getDetectionFarColor()[1]) + " " +
        std::to_string(_remoteSettings.getDetectionFarColor()[2]) );
    _parametersManager.set(
        "epsilon", std::to_string(_remoteSettings.getEpsilon( )));
    _parametersManager.set(
        "head-light", (_remoteSettings.getHeadLight( ) ? "1" : "0"));

    auto& app = _parametersManager.getApplicationParameters();
    app.setJpegSize( Vector2ui{ _remoteSettings.getJpegSize()  } );
    app.setJpegCompression( std::min( _remoteSettings.getJpegCompression(), 100u ));

    _engine.getRenderer().commit();
    _engine.getFrameBuffer().clear();
}

bool ZeroEQPlugin::_requestFrame()
{
    auto simHandler = _engine.getScene().getSimulationHandler();
    auto volHandler = _engine.getScene().getVolumeHandler();
    const uint64_t nbFrames = simHandler ? simHandler->getNbFrames() :
                                 ( volHandler ? volHandler->getNbFrames() : 0 );

    const auto& sceneParams = _parametersManager.getSceneParameters();
    const auto ts = uint64_t(sceneParams.getTimestamp());
    const auto current = nbFrames == 0 ? 0 : (ts % nbFrames);
    const auto animationDelta = sceneParams.getAnimationDelta();

    if( current == _remoteFrame.getCurrent() &&
        nbFrames == _remoteFrame.getEnd() &&
        animationDelta == _remoteFrame.getDelta( ))
    {
        return false;
    }

    _remoteFrame.setCurrent( current );
    _remoteFrame.setDelta( animationDelta );
    _remoteFrame.setEnd( nbFrames );
    _remoteFrame.setStart( 0 );
    return true;
}

void ZeroEQPlugin::_frameUpdated()
{
    auto& sceneParams = _parametersManager.getSceneParameters();
    sceneParams.setTimestamp( _remoteFrame.getCurrent( ));
    sceneParams.setAnimationDelta( _remoteFrame.getDelta( ));
    _engine.commit();
}

bool ZeroEQPlugin::_requestViewport()
{
    const auto& windowSize =
        _parametersManager.getApplicationParameters().getWindowSize();
    _remoteViewport.setSize( { windowSize[0], windowSize[1] } );
    return true;
}

void ZeroEQPlugin::_viewportUpdated()
{
    _parametersManager.getApplicationParameters().setWindowSize(
        Vector2ui{ _remoteViewport.getSize() } );
    _engine.commit();
}

bool ZeroEQPlugin::_requestSimulationHistogram()
{
    auto simulationHandler = _engine.getScene().getSimulationHandler();
    if( simulationHandler )
    {
        const auto& histogram = simulationHandler->getHistogram();
        _remoteSimulationHistogram.setMin( histogram.range.x( ));
        _remoteSimulationHistogram.setMax( histogram.range.y( ));
        _remoteSimulationHistogram.setBins( histogram.values );
    }
    return true;
}

bool ZeroEQPlugin::_requestVolumeHistogram()
{
    auto volumeHandler = _engine.getScene().getVolumeHandler();
    if( volumeHandler )
    {
        const auto& histogram = volumeHandler->getHistogram();
        _remoteVolumeHistogram.setMin( histogram.range.x( ));
        _remoteVolumeHistogram.setMax( histogram.range.y( ));
        _remoteVolumeHistogram.setBins( histogram.values );
    }
    return true;
}

uint8_t* ZeroEQPlugin::_encodeJpeg(const uint32_t width,
                     const uint32_t height,
                     const uint8_t* rawData,
                     const int32_t pixelFormat,
                     unsigned long& dataSize)
{
    uint8_t* tjSrcBuffer = const_cast< uint8_t* >( rawData );
    const int32_t color_components = 4; // Color Depth
    const int32_t tjPitch = width * color_components;
    const int32_t tjPixelFormat = pixelFormat;

    uint8_t* tjJpegBuf = 0;
    const int32_t tjJpegSubsamp = TJSAMP_444;
    const int32_t tjFlags = TJXOP_ROT180;

    const int32_t success = tjCompress2(
        _compressor, tjSrcBuffer, width, tjPitch, height,
        tjPixelFormat, &tjJpegBuf, &dataSize, tjJpegSubsamp,
        _parametersManager.getApplicationParameters().getJpegCompression(),
        tjFlags);

    if(success != 0)
    {
        BRAYNS_ERROR << "libjpeg-turbo image conversion failure" << std::endl;
        return 0;
    }
    return static_cast<uint8_t *>(tjJpegBuf);
}

}

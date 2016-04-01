/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include "ZeroBufPlugin.h"

#include <brayns/common/camera/Camera.h>
#include <brayns/common/renderer/FrameBuffer.h>
#include <zerobuf/render/camera.h>
#include <zerobuf/render/frameBuffers.h>
#include <zeq/hbp/vocabulary.h>

namespace brayns
{

ZeroBufPlugin::ZeroBufPlugin(
    ApplicationParameters& applicationParameters,
    ExtensionParameters& extensionParameters )
    : ExtensionPlugin( applicationParameters, extensionParameters )
    , _compressor( tjInitCompress( ))
    , _jpegCompression( applicationParameters.getJpegCompression( ))
{
    _setupRequests( );
    _setupHTTPServer( );
}

ZeroBufPlugin::~ZeroBufPlugin( )
{
    if( _compressor )
        tjDestroy( _compressor );
    _subscriber.deregisterHandler( ::zeq::vocabulary::EVENT_EXIT );

    if( _httpServer )
    {
        _httpServer->remove( *_extensionParameters.camera->getSerializable( ));
        _httpServer->remove( _remoteImageJPEG );
    }
}

void ZeroBufPlugin::run()
{
    _publisher.publish( zeq::Event( zeq::vocabulary::EVENT_HEARTBEAT ));

    while( _subscriber.receive( 0 )) {}
}

void ZeroBufPlugin::_setupHTTPServer()
{
    const strings& arguments = _applicationParameters.arguments();
    char** argv = new char*[arguments.size()];
    for( size_t i = 0; i < arguments.size( ); ++i )
        argv[ i ] = const_cast< char* >( arguments[ i ].c_str( ));

    _httpServer = ::zeq::http::Server::parse(
        arguments.size(), const_cast< const char** >( argv ), _subscriber );
    delete [] argv;

    if( !_httpServer )
    {
        BRAYNS_ERROR << "HTTP could not be initialized" << std::endl;
        return;
    }

    BRAYNS_INFO << "Registering handlers on " <<
        _httpServer->getURI() << std::endl;

    servus::Serializable& cam = *_extensionParameters.camera->getSerializable( );
    _httpServer->add( cam );
    cam.setUpdatedFunction( std::bind( &ZeroBufPlugin::_cameraUpdated, this ));

    _httpServer->add( _remoteImageJPEG );
    _remoteImageJPEG.setRequestedFunction(
        std::bind( &ZeroBufPlugin::_requestImageJPEG, this ));

    _httpServer->add( _remoteFrameBuffers );
    _remoteFrameBuffers.setRequestedFunction(
        std::bind( &ZeroBufPlugin::_requestFrameBuffers, this ));
}

void ZeroBufPlugin::_setupRequests()
{
    ::zerobuf::render::Camera camera;
    _requests[ camera.getTypeIdentifier() ] = [&]
        { return _publisher.publish(
            *_extensionParameters.camera->getSerializable( ));
        };

    ::zerobuf::render::ImageJPEG imageJPEG;
    _requests[ imageJPEG.getTypeIdentifier() ] =
        std::bind( &ZeroBufPlugin::_requestImageJPEG, this );

    ::zerobuf::render::FrameBuffers frameBuffers;
    _requests[ frameBuffers.getTypeIdentifier() ] =
        std::bind( &ZeroBufPlugin::_requestFrameBuffers, this );
}

void ZeroBufPlugin::_cameraUpdated()
{
    _extensionParameters.frameBuffer->clear();
    _extensionParameters.camera->commit();
}

bool ZeroBufPlugin::_requestImageJPEG()
{
    const Vector2i frameSize = _extensionParameters.frameBuffer->getSize( );
    unsigned int* colorBuffer =
        (unsigned int*)_extensionParameters.frameBuffer->getColorBuffer( );
    if( colorBuffer )
    {
        unsigned long jpegSize =
            frameSize.x( ) * frameSize.y( ) * sizeof( unsigned long );
        uint8_t* jpegData = _encodeJpeg(
                ( uint32_t )frameSize.x( ),
                ( uint32_t )frameSize.y( ),
                ( uint8_t* )colorBuffer, jpegSize );

        _remoteImageJPEG.setData( jpegData, jpegSize  );
        tjFree(jpegData);
    }
    return true;
}

bool ZeroBufPlugin::_requestFrameBuffers()
{
    const Vector2i frameSize = _extensionParameters.frameBuffer->getSize( );
    const float* depthBuffer =
        _extensionParameters.frameBuffer->getDepthBuffer( );

    _remoteFrameBuffers.setWidth( frameSize.x( ));
    _remoteFrameBuffers.setHeight( frameSize.y( ));
    if( depthBuffer )
    {
        typedef std::vector< uint16_t > uint16_ts;
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
        _extensionParameters.frameBuffer->getColorBuffer( );
    if( colorBuffer )
        _remoteFrameBuffers.setDiffuse(
            colorBuffer, frameSize.x( ) * frameSize.y( ) *
              _extensionParameters.frameBuffer->getColorDepth( ));
    else
        _remoteFrameBuffers.setDiffuse( 0, 0 );

    return true;
}

bool ZeroBufPlugin::_onRequest( const ::zeq::Event& event )
{
    const auto& eventType = ::zeq::vocabulary::deserializeRequest( event );
    const auto& i = _requests.find( eventType );
    if( i == _requests.end( ))
        return false;
    return i->second();
}

uint8_t* ZeroBufPlugin::_encodeJpeg(const uint32_t width,
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

    const int32_t success =
            tjCompress2( _compressor, tjSrcBuffer, width, tjPitch, height,
                        tjPixelFormat, &tjJpegBuf, &dataSize, tjJpegSubsamp,
                        _jpegCompression, tjFlags);

    if(success != 0)
    {
        BRAYNS_ERROR << "libjpeg-turbo image conversion failure" << std::endl;
        return 0;
    }
    return static_cast<uint8_t *>(tjJpegBuf);
}

}

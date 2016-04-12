/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#ifndef ZeroBufPlugin_H
#define ZeroBufPlugin_H

#include "ExtensionPlugin.h"

#include <brayns/api.h>
#include <zeq/zeq.h>
#include <turbojpeg.h>
#include <zerobuf/render/imageJPEG.h>
#include <zerobuf/render/frameBuffers.h>
#include <zerobuf/render/attribute.h>

namespace brayns
{

/**
   The ZeroBufPlugin is in charge of exposing a both a http/REST and a Zeq
   interface to the outside world. The http server is configured according
   to the --http-server parameter provided by ApplicationParameters.
 */
class ZeroBufPlugin : public ExtensionPlugin
{
public:
    ZeroBufPlugin( ApplicationParameters&, ExtensionParameters& );
    ~ZeroBufPlugin( );

    /** @copydoc ExtensionPlugin::execute */
    BRAYNS_API void run( ) final;

private:

    void _setupHTTPServer( );

    void _setupRequests( );

    void _cameraUpdated( );

    void _attributeUpdated( );

    bool _requestImageJPEG( );

    bool _requestFrameBuffers( );

    bool _onRequest( const ::zeq::Event& event );

    uint8_t* _encodeJpeg(const uint32_t width,
                         const uint32_t height,
                         const uint8_t* rawData,
                         unsigned long& dataSize);

    tjhandle _compressor;
    ::zeq::Subscriber _subscriber;
    ::zeq::Publisher _publisher;
    std::unique_ptr< ::zeq::http::Server > _httpServer;
    typedef std::function< bool() > RequestFunc;
    typedef std::map< ::zeq::uint128_t, RequestFunc > RequestFuncs;
    RequestFuncs _requests;
    size_t _jpegCompression;

    ::zerobuf::render::ImageJPEG _remoteImageJPEG;
    ::zerobuf::render::FrameBuffers _remoteFrameBuffers;
    ::zerobuf::render::Attribute _remoteAttribute;
};

}
#endif // ZeroBufPlugin_H

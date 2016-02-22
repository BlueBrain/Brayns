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

#ifndef ZeroBufPlugin_H
#define ZeroBufPlugin_H

#include "ExtensionPlugin.h"

#include <brayns/api.h>
#include <zeq/zeq.h>
#include <turbojpeg.h>
#include <zerobuf/render/imageJPEG.h>

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

    bool _requestImageJPEG( );

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

    ::zerobuf::render::ImageJPEG _remoteImageJPEG;

};

}
#endif // ZeroBufPlugin_H

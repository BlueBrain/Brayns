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

#ifndef ZeroBufPlugin_H
#define ZeroBufPlugin_H

#include "ExtensionPlugin.h"

#include <brayns/api.h>
#include <zeroeq/zeroeq.h>
#include <turbojpeg.h>
#include <lexis/render/imageJPEG.h>
#include <zerobuf/render/frameBuffers.h>
#include <zerobuf/render/attribute.h>
#include <zerobuf/render/reset.h>
#include <zerobuf/render/material.h>

namespace brayns
{

/**
   The ZeroBufPlugin is in charge of exposing a both a http/REST and a ZeroEQ
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

    void _resetUpdated( );

    void _materialUpdated( );

    bool _requestImageJPEG( );

    bool _requestFrameBuffers( );

    void _resizeImage(
        unsigned int* srcData,
        const Vector2i& srcSize,
        const Vector2i& dstSize,
        uints& dstData);

    uint8_t* _encodeJpeg(const uint32_t width,
                         const uint32_t height,
                         const uint8_t* rawData,
                         unsigned long& dataSize);

    tjhandle _compressor;
    ::zeroeq::Subscriber _subscriber;
    ::zeroeq::Publisher _publisher;
    std::unique_ptr< ::zeroeq::http::Server > _httpServer;
    typedef std::function< bool() > RequestFunc;
    typedef std::map< ::zeroeq::uint128_t, RequestFunc > RequestFuncs;
    RequestFuncs _requests;
    size_t _jpegCompression;
    bool _processingImageJpeg;

    ::lexis::render::ImageJPEG _remoteImageJPEG;
    ::zerobuf::render::FrameBuffers _remoteFrameBuffers;
    ::zerobuf::render::Attribute _remoteAttribute;
    ::zerobuf::render::Reset _remoteReset;
    ::zerobuf::render::Material _remoteMaterial;

};

}
#endif // ZeroBufPlugin_H

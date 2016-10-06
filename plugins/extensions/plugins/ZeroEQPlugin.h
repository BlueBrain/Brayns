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

#ifndef ZEROEQPLUGIN_H
#define ZEROEQPLUGIN_H

#include "ExtensionPlugin.h"

#include <brayns/api.h>
#include <zeroeq/zeroeq.h>
#include <turbojpeg.h>
#include <lexis/render/imageJPEG.h>
#include <zerobuf/render/frameBuffers.h>
#include <zerobuf/render/attribute.h>
#include <zerobuf/render/reset.h>
#include <zerobuf/render/material.h>
#include <zerobuf/render/transferFunction1D.h>
#include <zerobuf/render/spikes.h>

namespace brayns
{

/**
   The ZeroEQPlugin is in charge of exposing a both a http/REST and a ZeroEQ
   interface to the outside world. The http server is configured according
   to the --http-server parameter provided by ApplicationParameters.
 */
class ZeroEQPlugin : public ExtensionPlugin
{
public:
    ZeroEQPlugin( ApplicationParameters&, ExtensionParameters& );
    ~ZeroEQPlugin( );

    /** @copydoc ExtensionPlugin::execute */
    BRAYNS_API void run( ) final;

private:

    /**
     * @brief Initializes HTTP server
     */
    void _setupHTTPServer();

    /**
     * @brief Sets up ZeroEQ requests
     */
    void _setupRequests();

    /**
     * @brief This method is called when the camera is updated by a ZeroEQ event
     */
    void _cameraUpdated();

    /**
     * @brief This method is called when an application attribute is updated by a ZeroEQ event
     */
    void _attributeUpdated();

    /**
     * @brief This method is called when camera reset is invoked by a ZeroEQ event
     */
    void _resetUpdated();

    /**
     * @brief This method is called when a material is updated by a ZeroEQ event
     */
    void _materialUpdated();

    /**
     * @brief This method is called when spikes are updated by a ZeroEQ event
     */
    void _spikesUpdated();

    /**
     * @brief This method is called when the transfer function is requested by a ZeroEQ event
     * @return True if the method was successfull, false otherwise
     * @todo Specify the attribute that should be returned when the feature is available in ZeroEQ
     */
    bool _requestTransferFunction1D();

    /**
     * @brief This method is called when the transfer function is updated by a ZeroEQ event
     */
    void _transferFunction1DUpdated();

    /**
     * @brief This method is called when an Image JPEG is requested by a ZeroEQ event
     * @return True if the method was successfull, false otherwise
     */
    bool _requestImageJPEG();

    /**
     * @brief This method is called when frame buffers is requested by a ZeroEQ event
     * @return True if the method was successfull, false otherwise
     */
    bool _requestFrameBuffers();

    /**
     * @brief This method is called when spikes are requested by a ZeroEQ event
     * @return True if the method was successfull, false otherwise
     */
    bool _requestSpikes();

    /**
     * @brief Resizes an given image according to the new size
     * @param srcData Source buffer
     * @param srcSize Source size
     * @param dstSize Returned destination size
     * @param dstData Returned destination buffer
     */
    void _resizeImage(
        unsigned int* srcData,
        const Vector2i& srcSize,
        const Vector2i& dstSize,
        uints& dstData);

    /**
     * @brief Encodes an RAW image buffer into JPEG
     * @param width Image width
     * @param height Image height
     * @param rawData Source buffer
     * @param dataSize Returned buffer size
     * @return Destination buffer
     */
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
    ::zerobuf::render::TransferFunction1D _remoteTransferFunction1D;
    ::zerobuf::data::Spikes _remoteSpikes;

};

}
#endif // ZEROEQPLUGIN_H

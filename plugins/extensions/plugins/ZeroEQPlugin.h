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
#include <zeroeq/http/server.h>
#include <turbojpeg.h>

#include <lexis/render/frame.h>
#include <lexis/render/imageJPEG.h>
#include <lexis/render/lookupTable1D.h>
#include <lexis/render/viewport.h>
#include <zerobuf/render/attribute.h>
#include <zerobuf/render/colormap.h>
#include <zerobuf/render/frameBuffers.h>
#include <zerobuf/render/parameters.h>
#include <zerobuf/render/reset.h>
#include <zerobuf/render/scene.h>
#include <zerobuf/render/spikes.h>
#include <zerobuf/render/transferFunction1D.h>

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
    ZeroEQPlugin( EnginePtr engine );
    ~ZeroEQPlugin();

    /** @copydoc ExtensionPlugin::execute */
    BRAYNS_API void run( ) final;

    BRAYNS_API bool operator ! () const;
    BRAYNS_API ::zeroeq::http::Server* operator->();

private:

    /**
     * @brief Initializes HTTP server
     */
    void _setupHTTPServer();

    /**
     * @brief Sets up ZeroEQ requests
     */
    void _setupRequests();

    void _setupSubscriber();


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
    void _resetCameraUpdated();

    /**
     * @brief This method is called when scene reset is invoked by a ZeroEQ event
     */
    void _resetSceneUpdated();

    /**
     * @brief This method is called when the scene is requested by a ZeroEQ event
     * @return True if the method was successful, false otherwise
     */
    bool _requestScene();

    /**
     * @brief This method is called when the scene is updated by a ZeroEQ event
     */
    void _sceneUpdated();

    /**
     * @brief This method is called when spikes are updated by a ZeroEQ event
     */
    void _spikesUpdated();

    /**
     * @brief This method is called when the transfer function is requested by a ZeroEQ event
     * @return True if the method was successful, false otherwise
     * @todo Specify the attribute that should be returned when the feature is available in ZeroEQ
     */
    bool _requestTransferFunction1D();

    /**
     * @brief This method is called when the transfer function is updated by a ZeroEQ event
     */
    void _transferFunction1DUpdated();

    /**
     * @brief This method is called when the lookup table is updated by a ZeroEQ event
     */
    void _LookupTable1DUpdated();

    /**
     * @brief This method is called when a lookup table 1D is requested by a ZeroEQ event
     * @return True if the method was successful, false otherwise
     */
    bool _requestLookupTable1D();

    /**
     * @brief This method is called when the colomap is updated by a ZeroEQ event
     */
    void _colormapUpdated();

    /**
     * @brief This method is called when the colormap is requested by a ZeroEQ event
     * @return True if the method was successful, false otherwise
     */
    bool _requestColormap();

    /**
     * @brief This method is called when an Image JPEG is requested by a ZeroEQ event
     * @return True if the method was successful, false otherwise
     */
    bool _requestImageJPEG();

    /**
     * @brief This method is called when frame buffers is requested by a ZeroEQ event
     * @return True if the method was successful, false otherwise
     */
    bool _requestFrameBuffers();

    /**
     * @brief This method is called when spikes are requested by a ZeroEQ event
     * @return True if the method was successful, false otherwise
     */
    bool _requestSpikes();


    /**
     * @brief This method initializes data sources according to default application parameters
     */
    void _initializeDataSource();

    /**
     * @brief This method is called when data sources are updated by a ZeroEQ event
     */
    void _dataSourceUpdated();

    /**
     * @brief This method initializes data sources according to default application parameters
     */
    void _initializeSettings();

    /**
     * @brief This method is called when settings are updated by a ZeroEQ event
     */
    void _settingsUpdated();

    /**
     * @brief This method is called when simulation frame (timestamp) are requested by a ZeroEQ event
     * @return True if the method was successful, false otherwise
     */
    bool _requestFrame();

    /**
     * @brief This method is called when simulation frame (timestamp) are updated by a ZeroEQ event
     */
    void _frameUpdated();

    /**
     * @brief This method is called when viewport requested by a ZeroEQ event
     * @return True if the method was successful, false otherwise
     */
    bool _requestViewport();

    /**
     * @brief This method is called when viewport is updated by a ZeroEQ event
     */
    void _viewportUpdated();

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
    bool _processingImageJpeg;

    ::brayns::DataSource _remoteDataSource;
    ::brayns::Settings _remoteSettings;
    ::lexis::render::Frame _remoteFrame;
    ::lexis::render::ImageJPEG _remoteImageJPEG;
    ::lexis::render::LookupTable1D _remoteLookupTable1D;
    ::lexis::render::Viewport _remoteViewport;
    ::zerobuf::data::Spikes _remoteSpikes;
    ::zerobuf::render::Attribute _remoteAttribute;
    ::zerobuf::render::Colormap _remoteColormap;
    ::zerobuf::render::FrameBuffers _remoteFrameBuffers;
    ::zerobuf::render::Material _remoteMaterial;
    ::zerobuf::render::ResetCamera _remoteResetCamera;
    ::zerobuf::render::ResetScene _remoteResetScene;
    ::zerobuf::render::Scene _remoteScene;
    ::zerobuf::render::TransferFunction1D _remoteTransferFunction1D;
};

}
#endif // ZEROEQPLUGIN_H

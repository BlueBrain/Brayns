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

#ifndef ZEROEQPLUGIN_H
#define ZEROEQPLUGIN_H

#include "ExtensionPlugin.h"

#include <brayns/api.h>
#include <turbojpeg.h>
#include <zeroeq/http/server.h>
#include <zeroeq/zeroeq.h>

#include <lexis/data/Progress.h>
#include <lexis/render/ClipPlanes.h>
#include <lexis/render/Histogram.h>
#include <lexis/render/frame.h>
#include <lexis/render/imageJPEG.h>
#include <lexis/render/materialLUT.h>
#include <lexis/render/viewport.h>

#include <zerobuf/render/frameBuffers.h>
#include <zerobuf/render/parameters.h>
#include <zerobuf/render/reset.h>
#include <zerobuf/render/scene.h>
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
    ZeroEQPlugin(ParametersManager& parametersManager);
    ~ZeroEQPlugin();

    /** @copydoc ExtensionPlugin::run */
    BRAYNS_API bool run(EnginePtr engine, KeyboardHandler& keyboardHandler,
                        AbstractManipulator& cameraManipulator) final;

    BRAYNS_API bool operator!() const;
    BRAYNS_API::zeroeq::http::Server* operator->();

    /** Handle the given objects in subscriber, publisher (via request) and HTTP
     * server. */
    BRAYNS_API void handle(servus::Serializable& object);

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
     * @brief This method is called when camera reset is invoked by a ZeroEQ
     * event
     */
    void _resetCameraUpdated();

    /**
     * @brief This method is called when the scene is requested by a ZeroEQ
     * event
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
     * @brief This method is called when the material lookup table is updated by
     *        a ZeroEQ event
     */
    void _materialLUTUpdated();

    /**
     * @brief This method is called when a material lookup table is requested by
     * a ZeroEQ event
     * @return True if the method was successful, false otherwise
     */
    void _requestMaterialLUT();

    /**
     * @brief This method is called when an Image JPEG is requested by a ZeroEQ
     * event
     * @return True if the method was successful, false otherwise
     */
    bool _requestImageJPEG();

    /**
     * @brief This method is called when frame buffers is requested by a ZeroEQ
     * event
     * @return True if the method was successful, false otherwise
     */
    bool _requestFrameBuffers();

    /**
     * @brief This method is called when spikes are requested by a ZeroEQ event
     * @return True if the method was successful, false otherwise
     */
    bool _requestSpikes();

    /**
     * @brief This method initializes data sources according to default
     * application parameters
     */
    void _initializeDataSource();

    /**
     * @brief This method is called when data sources are updated by a ZeroEQ
     * event
     */
    void _dataSourceUpdated();

    /**
     * @brief This method initializes data sources according to default
     * application parameters
     */
    void _initializeSettings();

    /**
     * @brief This method is called when settings are updated by a ZeroEQ event
     */
    void _settingsUpdated();

    /**
     * @brief This method is called when simulation frame (timestamp) are
     * requested by a ZeroEQ event
     * @return True if the method was successful, false otherwise
     */
    bool _requestFrame();

    /**
     * @brief This method is called when simulation frame (timestamp) are
     * updated by a ZeroEQ event
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
     * @brief This method is called when the histogram for the current
     * simulation frame is requested
     *        by a ZeroEQ event
     * @return True if the method was successful, false otherwise
     */
    bool _requestSimulationHistogram();

    /**
     * @brief This method is called when the histogram for the current volume is
     * requested
     *        by a ZeroEQ event
     * @return True if the method was successful, false otherwise
     */
    bool _requestVolumeHistogram();

    /**
     * @brief This method is called when the clip planes are updated by a ZeroEQ
     * event
     */
    void _clipPlanesUpdated();

    /**
     * @brief This method is called when the clip planes are requested by a
     * ZeroEQ event
     * @return True if the method was successful, false otherwise
     */
    bool _requestClipPlanes();

    /**
     * @brief This method is called when rendering is forced by a ZeroEQ event
     */
    void _forceRenderingUpdated();

    /**
     * @brief This method is called when the progress of the application is
     * requested by a ZeroEQ event
     * @return True if the method was successful, false otherwise
     */
    bool _requestProgress();

    std::future<::zeroeq::http::Response> _handleCircuitConfigBuilder(
        const ::zeroeq::http::Request&);

    /**
     * @brief Resizes an given image according to the new size
     * @param srcData Source buffer
     * @param srcSize Source size
     * @param dstSize Returned destination size
     * @param dstData Returned destination buffer
     */
    void _resizeImage(unsigned int* srcData, const Vector2i& srcSize,
                      const Vector2i& dstSize, uints& dstData);

    /**
     * @brief Encodes an RAW image buffer into JPEG
     * @param width Image width
     * @param height Image height
     * @param rawData Source buffer
     * @param pixelFormat pixel format of rawData
     * @param dataSize Returned buffer size
     * @return Destination buffer
     */
    uint8_t* _encodeJpeg(const uint32_t width, const uint32_t height,
                         const uint8_t* rawData, const int32_t pixelFormat,
                         unsigned long& dataSize);

    void _onNewEngine();
    void _onChangeEngine();

    bool _writeBlueConfigFile(const std::string& blueConfigFilename,
                              const strings& params);

    Engine* _engine = nullptr;
    ParametersManager& _parametersManager;
    tjhandle _compressor;
    ::zeroeq::Subscriber _subscriber;
    ::zeroeq::Publisher _publisher;
    std::unique_ptr<::zeroeq::http::Server> _httpServer;
    typedef std::function<bool()> RequestFunc;
    typedef std::map<::zeroeq::uint128_t, RequestFunc> RequestFuncs;
    RequestFuncs _requests;
    bool _processingImageJpeg;

    ::lexis::render::Frame _remoteFrame;
    ::lexis::render::ImageJPEG _remoteImageJPEG;
    ::lexis::render::Viewport _remoteViewport;
    ::lexis::render::ClipPlanes _clipPlanes;
    ::lexis::render::Histogram _remoteSimulationHistogram;
    ::lexis::render::Histogram _remoteVolumeHistogram;
    ::lexis::render::MaterialLUT _remoteMaterialLUT;
    ::lexis::data::Progress _remoteProgress;

    ::brayns::v1::DataSource _remoteDataSource;
    ::brayns::v1::Settings _remoteSettings;
    ::brayns::v1::Spikes _remoteSpikes;
    ::brayns::v1::FrameBuffers _remoteFrameBuffers;
    ::brayns::v1::Material _remoteMaterial;
    ::brayns::v1::ResetCamera _remoteResetCamera;
    ::brayns::v1::Scene _remoteScene;
    ::brayns::v1::ForceRendering _remoteForceRendering;
    ::brayns::v1::CircuitConfigurationBuilder _remoteCircuitConfigBuilder;

    bool _forceRendering = false;
    bool _dirtyEngine;
};
}
#endif // ZEROEQPLUGIN_H

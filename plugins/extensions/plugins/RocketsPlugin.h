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

#ifndef ROCKETSPLUGIN_H
#define ROCKETSPLUGIN_H

#include "ExtensionPlugin.h"

#include <brayns/api.h>
#include <rockets/server.h>
#include <turbojpeg.h>

#include <lexis/data/Progress.h>
#include <lexis/render/ClipPlanes.h>
#include <lexis/render/Histogram.h>
#include <lexis/render/frame.h>
#include <lexis/render/imageJPEG.h>
#include <lexis/render/materialLUT.h>
#include <lexis/render/stream.h>
#include <lexis/render/viewport.h>

#include <zerobuf/render/frameBuffers.h>
#include <zerobuf/render/parameters.h>
#include <zerobuf/render/reset.h>
#include <zerobuf/render/scene.h>
#include <zerobuf/render/spikes.h>

namespace brayns
{
/**
   The RocketsPlugin is in charge of exposing a both an http/REST interface to
   the outside world. The http server is configured according
   to the --http-server parameter provided by ApplicationParameters.
 */
class RocketsPlugin : public ExtensionPlugin
{
public:
    RocketsPlugin(ParametersManager& parametersManager);
    ~RocketsPlugin();

    /** @copydoc ExtensionPlugin::run */
    BRAYNS_API bool run(EngineWeakPtr engine, KeyboardHandler& keyboardHandler,
                        AbstractManipulator& cameraManipulator) final;

private:
    void _onNewEngine();
    void _onChangeEngine();

    void _setupHTTPServer();
    std::string _getHttpInterface() const;
    void _handle(const std::string& endpoint, servus::Serializable& obj);
    void _handleGET(const std::string& endpoint,
                    const servus::Serializable& obj);
    void _handlePUT(const std::string& endpoint, servus::Serializable& obj);
    void _handleSchema(const std::string& endpoint,
                       const servus::Serializable& obj);
    void _remove(const std::string& endpoint);

    void _broadcastWebsocketMessages();
    std::string _processWebsocketMessage(const std::string& message);
    void _handleWebsocketEvent(const std::string& endpoint,
                               servus::Serializable& obj);

    void _handleVersion();
    void _handleStreaming();

    void _resetCameraUpdated();
    bool _requestScene();
    void _sceneUpdated();
    void _spikesUpdated();
    void _materialLUTUpdated();
    void _requestMaterialLUT();
    bool _requestImageJPEG();
    bool _requestFrameBuffers();
    bool _requestSpikes();

    void _initializeDataSource();
    void _dataSourceUpdated();

    void _initializeSettings();
    void _settingsUpdated();

    bool _requestFrame();
    void _frameUpdated();

    bool _requestViewport();
    void _viewportUpdated();

    bool _requestSimulationHistogram();

    bool _requestVolumeHistogram();

    void _clipPlanesUpdated();
    bool _requestClipPlanes();

    void _streamParamsUpdated();
    bool _requestStreamParams();

    void _forceRenderingUpdated();

    bool _requestProgress();

    std::future<rockets::http::Response> _handleCircuitConfigBuilder(
        const rockets::http::Request&);

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

    bool _writeBlueConfigFile(const std::string& filename,
                              const std::map<std::string, std::string>& params);

    using WebsocketEventMap =
        std::map<std::string, std::function<bool(const std::string&)>>;
    WebsocketEventMap _websocketEvents;

    Engine* _engine = nullptr;
    ParametersManager& _parametersManager;

    std::unique_ptr<rockets::Server> _httpServer;
    tjhandle _compressor;

    bool _processingImageJpeg = false;
    bool _forceRendering = false;
    bool _dirtyEngine = false;

    ::lexis::render::Frame _remoteFrame;
    ::lexis::render::ImageJPEG _remoteImageJPEG;
    ::lexis::render::Viewport _remoteViewport;
    ::lexis::render::ClipPlanes _clipPlanes;
    ::lexis::render::Histogram _remoteSimulationHistogram;
    ::lexis::render::Histogram _remoteVolumeHistogram;
    ::lexis::render::MaterialLUT _remoteMaterialLUT;
    ::lexis::render::Stream _streamParams;
    ::lexis::data::Progress _remoteProgress{0};

    ::brayns::v1::DataSource _remoteDataSource;
    ::brayns::v1::Settings _remoteSettings;
    ::brayns::v1::Spikes _remoteSpikes;
    ::brayns::v1::FrameBuffers _remoteFrameBuffers;
    ::brayns::v1::Material _remoteMaterial;
    ::brayns::v1::ResetCamera _remoteResetCamera;
    ::brayns::v1::Scene _remoteScene;
    ::brayns::v1::ForceRendering _remoteForceRendering;
    ::brayns::v1::CircuitConfigurationBuilder _remoteCircuitConfigBuilder;
};
}

#endif

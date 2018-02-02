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
#include "ImageGenerator.h"

#include <brayns/api.h>
#include <brayns/common/Timer.h>
#include <rockets/jsonrpc/server.h>
#include <rockets/server.h>

namespace brayns
{
struct RpcDocumentation;

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
    BRAYNS_API bool run(EnginePtr engine, KeyboardHandler& keyboardHandler,
                        AbstractManipulator& cameraManipulator) final;

private:
    std::string _getHttpInterface() const;
    void _setupRocketsServer();
    void _setupWebsocket();
    void _broadcastWebsocketMessages();

    template <class T>
    void _handle(const std::string& endpoint, T& obj);

    template <class T, class F = std::function<bool(const T&)>>
    void _handleGET(const std::string& endpoint, T& obj,
                    F modifiedFunc = [](const T& obj) {
                        return obj.getModified();
                    });

    template <class T>
    void _handlePUT(const std::string& endpoint, T& obj);

    template <class T, class F>
    void _handlePUT(const std::string& endpoint, T& obj, F postUpdateFunc);

    template <class P, class R>
    void _handleRPC(const std::string& method, const RpcDocumentation& doc,
                    std::function<R(P)> action);

    void _handleRPC(const std::string& method, const std::string& description,
                    std::function<void()> action);

    template <class T>
    void _handleObjectSchema(const std::string& endpoint, T& obj);

    void _handleSchema(const std::string& endpoint, const std::string& schema);

    void _registerEndpoints();

    void _handleApplicationParams();
    void _handleFrameBuffer();
    void _handleGeometryParams();
    void _handleImageJPEG();
    void _handleSimulationHistogram();
    void _handleStreaming();
    void _handleVersion();
    void _handleVolumeHistogram();
    void _handleVolumeParams();

    void _handleInspect();
    void _handleQuit();
    void _handleResetCamera();
    void _handleSnapshot();

    std::future<rockets::http::Response> _handleCircuitConfigBuilder(
        const rockets::http::Request&);

    bool _writeBlueConfigFile(const std::string& filename,
                              const std::map<std::string, std::string>& params);

    using WsClientConnectNotifications =
        std::map<std::string, std::function<std::string()>>;
    WsClientConnectNotifications _wsClientConnectNotifications;

    using WsBroadcastOperations = std::map<std::string, std::function<void()>>;
    WsBroadcastOperations _wsBroadcastOperations;

    EnginePtr _engine;
    ParametersManager& _parametersManager;

    std::unique_ptr<rockets::Server> _rocketsServer;
    using JsonRpcServer = rockets::jsonrpc::Server<rockets::Server>;
    std::unique_ptr<JsonRpcServer> _jsonrpcServer;

    ImageGenerator _imageGenerator;

    Timer _timer;
};
}

#endif

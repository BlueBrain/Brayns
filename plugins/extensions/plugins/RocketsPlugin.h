/* Copyright (c) 2015-2018 EPFL/Blue Brain Project
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
#include <brayns/common/ActionInterface.h>
#include <brayns/common/Timer.h>
#include <rockets/jsonrpc/asyncReceiver.h>
#include <rockets/jsonrpc/server.h>
#include <rockets/server.h>

namespace brayns
{
struct RpcDocumentation;
class SocketListener;

/**
   The RocketsPlugin is in charge of exposing a both an http/REST interface to
   the outside world. The http server is configured according
   to the --http-server parameter provided by ApplicationParameters.
 */
class RocketsPlugin : public ExtensionPlugin, public ActionInterface
{
public:
    RocketsPlugin(EnginePtr engine, PluginAPI* api);
    ~RocketsPlugin();

    /**
     * In case no event loop is available, this processes in- and outgoing HTTP
     * and websocket messages.
     *
     * Otherwise, this is a NOP as the incoming message processing is done by
     * the SocketListener.
     */
    BRAYNS_API void preRender() final;

    /**
     * Enqueue modified and registered objects for broadcast that have changed
     * after the rendering is finished (framebuffer, animation params, progress,
     * statistics).
     */
    BRAYNS_API void postRender() final;

    /**
     * Enqueue modified and registered objects for broadcast that have changed
     * after scene loading is finished (camera, progress, statistics).
     */
    BRAYNS_API void postSceneLoading() final;

private:
    std::string _getHttpInterface() const;
    void _setupRocketsServer();
    void _setupWebsocket();
    void _broadcastWebsocketMessages();

    template <class T>
    void _handle(const std::string& endpoint, T& obj);

    template <class T, class F = std::function<bool(const T&)>>
    void _handleGET(const std::string& endpoint, T&,
                    F modifiedFunc = [](const T& obj) {
                        return obj.isModified();
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

    template <class P, class R>
    void _handleAsyncRPC(
        const std::string& method, const RpcDocumentation& doc,
        std::function<void(P, rockets::jsonrpc::AsyncResponse)> action,
        rockets::jsonrpc::AsyncReceiver::CancelRequestCallback cancel);

    template <class T>
    void _handleObjectSchema(const std::string& endpoint, T& obj);

    void _handleSchema(const std::string& endpoint, const std::string& schema);

    void _registerEndpoints();

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

    EnginePtr _engine;

    using WsClientConnectNotifications =
        std::map<std::string, std::function<std::string()>>;
    WsClientConnectNotifications _wsClientConnectNotifications;

    using WsBroadcastOperations = std::map<std::string, std::function<void()>>;
    WsBroadcastOperations _wsBroadcastOperations;

    ParametersManager& _parametersManager;

    std::unique_ptr<rockets::Server> _rocketsServer;
    using JsonRpcServer =
        rockets::jsonrpc::Server<rockets::Server,
                                 rockets::jsonrpc::AsyncReceiver>;
    std::unique_ptr<JsonRpcServer> _jsonrpcServer;

#ifdef BRAYNS_USE_LIBUV
    std::unique_ptr<SocketListener> _socketListener;
#endif

    ImageGenerator _imageGenerator;

    Timer _timer;

    void _registerRequest(const std::string& name,
                          const RetParamFunc& action) final;
    void _registerRequest(const std::string& name, const RetFunc& action) final;
    void _registerNotification(const std::string& name,
                               const ParamFunc& action) final;
    void _registerNotification(const std::string& name,
                               const VoidFunc& action) final;
};
}

#endif

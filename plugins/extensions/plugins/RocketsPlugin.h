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
#include <rockets/server.h>

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
    BRAYNS_API bool run(EnginePtr engine, KeyboardHandler& keyboardHandler,
                        AbstractManipulator& cameraManipulator) final;

private:
    void _onNewEngine();

    void _setupHTTPServer();
    void _setupWebsocket();
    std::string _getHttpInterface() const;

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

    template <class T>
    void _handleSchema(const std::string& endpoint, T& obj);

    void _remove(const std::string& endpoint);

    void _broadcastWebsocketMessages();
    rockets::ws::Response _processWebsocketMessage(const std::string& message);

    void _handleVersion();
    void _handleStreaming();
    void _handleImageJPEG();
    void _handleApplicationParams();
    void _handleGeometryParams();
    void _handleVolumeParams();
    void _handleSimulationHistogram();
    void _handleVolumeHistogram();

    std::future<rockets::http::Response> _handleCircuitConfigBuilder(
        const rockets::http::Request&);

    bool _writeBlueConfigFile(const std::string& filename,
                              const std::map<std::string, std::string>& params);

    using WsIncomingMap =
        std::map<std::string,
                 std::function<bool(const std::string&, std::string& newData)>>;
    WsIncomingMap _wsIncoming;

    using WsOutgoingMap = std::map<std::string, std::function<std::string()>>;
    WsOutgoingMap _wsOutgoing;

    using WsBroadcastMap = std::map<std::string, std::function<void()>>;
    WsBroadcastMap _wsBroadcasts;

    EnginePtr _engine;
    ParametersManager& _parametersManager;

    std::unique_ptr<rockets::Server> _httpServer;

    ImageGenerator _imageGenerator;

    class Timer
    {
    public:
        using clock = std::chrono::high_resolution_clock;

        void start() { _startTime = clock::now(); }
        void restart() { start(); }
        float elapsed()
        {
            return std::chrono::duration<float>{clock::now() - _startTime}
                .count();
        }

        Timer() { start(); }
    private:
        clock::time_point _startTime;
    } _timer;
};
}

#endif

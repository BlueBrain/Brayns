/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include "NetworkManager.h"

#include <optional>

#include <Poco/JSON/JSONException.h>

#include <brayns/common/Log.h>

#include <brayns/json/JsonSchemaValidator.h>

#include <brayns/network/entrypoint/EntrypointException.h>
#include <brayns/network/interface/NetworkInterface.h>
#include <brayns/network/jsonrpc/JsonRpcFactory.h>
#include <brayns/network/jsonrpc/JsonRpcSender.h>
#include <brayns/network/socket/ClientSocket.h>
#include <brayns/network/socket/ServerSocket.h>

#include <brayns/network/entrypoints/AddClipPlaneEntrypoint.h>
#include <brayns/network/entrypoints/AddLightEntrypoint.h>
#include <brayns/network/entrypoints/AddModelEntrypoint.h>
#include <brayns/network/entrypoints/AnimationParametersEntrypoint.h>
#include <brayns/network/entrypoints/ApplicationParametersEntrypoint.h>
#include <brayns/network/entrypoints/CameraEntrypoint.h>
#include <brayns/network/entrypoints/CameraParamsEntrypoint.h>
#include <brayns/network/entrypoints/CancelEntrypoint.h>
#include <brayns/network/entrypoints/ChunkEntrypoint.h>
#include <brayns/network/entrypoints/ClearLightsEntrypoint.h>
#include <brayns/network/entrypoints/ExitLaterEntrypoint.h>
#include <brayns/network/entrypoints/ExportFramesEntrypoint.h>
#include <brayns/network/entrypoints/ExportFramesToDiskEntryPoint.h>
#include <brayns/network/entrypoints/GetClipPlanesEntrypoint.h>
#include <brayns/network/entrypoints/GetExportFramesProgressEntrypoint.h>
#include <brayns/network/entrypoints/GetInstancesEntrypoint.h>
#include <brayns/network/entrypoints/GetLightsEntrypoint.h>
#include <brayns/network/entrypoints/GetLoadersEntrypoint.h>
#include <brayns/network/entrypoints/GetModelEntrypoint.h>
#include <brayns/network/entrypoints/ImageJpegEntrypoint.h>
#include <brayns/network/entrypoints/ImageStreamingModeEntrypoint.h>
#include <brayns/network/entrypoints/InspectEntrypoint.h>
#include <brayns/network/entrypoints/LoadersSchemaEntrypoint.h>
#include <brayns/network/entrypoints/ModelPropertiesEntrypoint.h>
#include <brayns/network/entrypoints/ModelTransferFunctionEntrypoint.h>
#include <brayns/network/entrypoints/QuitEntrypoint.h>
#include <brayns/network/entrypoints/RegistryEntrypoint.h>
#include <brayns/network/entrypoints/RemoveClipPlanesEntrypoint.h>
#include <brayns/network/entrypoints/RemoveLightsEntrypoint.h>
#include <brayns/network/entrypoints/RemoveModelEntrypoint.h>
#include <brayns/network/entrypoints/RendererEntrypoint.h>
#include <brayns/network/entrypoints/RendererParamsEntrypoint.h>
#include <brayns/network/entrypoints/RequestModelUploadEntrypoint.h>
#include <brayns/network/entrypoints/ResetCameraEntrypoint.h>
#include <brayns/network/entrypoints/SceneEntrypoint.h>
#include <brayns/network/entrypoints/SchemaEntrypoint.h>
#include <brayns/network/entrypoints/SnapshotEntrypoint.h>
#include <brayns/network/entrypoints/StatisticsEntrypoint.h>
#include <brayns/network/entrypoints/TriggerJpegStreamEntrypoint.h>
#include <brayns/network/entrypoints/UpdateClipPlaneEntrypoint.h>
#include <brayns/network/entrypoints/UpdateInstanceEntrypoint.h>
#include <brayns/network/entrypoints/UpdateModelEntrypoint.h>
#include <brayns/network/entrypoints/VersionEntrypoint.h>
#include <brayns/network/entrypoints/VolumeParametersEntrypoint.h>

namespace
{
class RequestParser
{
public:
    static brayns::RequestMessage parse(const std::string &data)
    {
        auto json = _parse(data);
        _validateSchema(json);
        auto message = brayns::Json::deserialize<brayns::RequestMessage>(json);
        _validateHeader(message);
        return message;
    }

private:
    static inline const auto _schema = brayns::Json::getSchema<brayns::RequestMessage>();

    static brayns::JsonValue _parse(const std::string &data)
    {
        try
        {
            return brayns::Json::parse(data);
        }
        catch (const Poco::JSON::JSONException &e)
        {
            throw brayns::ParsingErrorException(e.displayText());
        }
        catch (const std::exception &e)
        {
            throw brayns::InternalErrorException("Unexpected error during request parsing: " + std::string(e.what()));
        }
        catch (...)
        {
            throw brayns::InternalErrorException("Unknown error during request parsing");
        }
    }

    static void _validateSchema(const brayns::JsonValue &json)
    {
        auto errors = brayns::JsonSchemaValidator::validate(json, _schema);
        if (!errors.empty())
        {
            throw brayns::InvalidRequestException("JSON schema errors", errors);
        }
    }

    static void _validateHeader(const brayns::RequestMessage &message)
    {
        if (message.jsonrpc != "2.0")
        {
            throw brayns::InvalidRequestException("Unsupported JSON-RPC version: '" + message.jsonrpc + "'");
        }
        auto &method = message.method;
        if (method.empty())
        {
            throw brayns::InvalidRequestException("No method provided in request");
        }
    }
};

class JsonRpcDispatcher
{
public:
    static void dispatch(const brayns::JsonRpcRequest &request, const brayns::EntrypointManager &entrypoints)
    {
        try
        {
            _dispatch(request, entrypoints);
        }
        catch (const brayns::EntrypointException &e)
        {
            auto &message = request.getMessage();
            auto &client = request.getClient();
            auto error = brayns::JsonRpcFactory::error(message, e);
            brayns::JsonRpcSender::error(error, client);
        }
    }

private:
    static void _dispatch(const brayns::JsonRpcRequest &request, const brayns::EntrypointManager &entrypoints)
    {
        try
        {
            entrypoints.onRequest(request);
        }
        catch (const std::exception &e)
        {
            throw brayns::InternalErrorException("Unexpected error while processing request: " + std::string(e.what()));
        }
        catch (...)
        {
            throw brayns::InternalErrorException("Unknown error while processing request");
        }
    }
};

class JsonRpcHandler
{
public:
    static void processRequest(
        const brayns::ClientRef &client,
        const std::string &data,
        const brayns::EntrypointManager &entrypoints)
    {
        brayns::RequestMessage message;
        try
        {
            message = RequestParser::parse(data);
        }
        catch (const brayns::EntrypointException &e)
        {
            auto error = brayns::JsonRpcFactory::error(e);
            brayns::JsonRpcSender::error(error, client);
            return;
        }
        auto request = brayns::JsonRpcRequest(client, std::move(message));
        JsonRpcDispatcher::dispatch(request, entrypoints);
    }
};

class ModelUploadHandler
{
public:
    static void processBinaryRequest(
        const brayns::ClientRef &client,
        std::string_view data,
        brayns::ModelUploadManager &modelUploads)
    {
        try
        {
            _processBinaryRequest(client, data, modelUploads);
        }
        catch (const brayns::EntrypointException &e)
        {
            auto error = brayns::JsonRpcFactory::error(e);
            brayns::JsonRpcSender::error(error, client);
        }
    }

private:
    static void _processBinaryRequest(
        const brayns::ClientRef &client,
        std::string_view data,
        brayns::ModelUploadManager &modelUploads)
    {
        try
        {
            modelUploads.addChunk(client, data);
        }
        catch (const std::exception &e)
        {
            throw brayns::InternalErrorException(
                "Unexpected error while processing binary request: " + std::string(e.what()));
        }
        catch (...)
        {
            throw brayns::InternalErrorException("Unknown error while processing binary request");
        }
    }
};

class RequestDispatcher
{
public:
    RequestDispatcher(const brayns::EntrypointManager &entrypoints, brayns::ModelUploadManager &modelUploads)
        : _handler(entrypoints)
        , _modelUploads(modelUploads)
    {
    }

    void dispatch(
        brayns::RequestBuffer &buffer,
        const brayns::EntrypointManager &entrypoints,
        brayns::ModelUploadManager &modelUploads)
    {
        auto requests = buffer.extractAll();
        return dispatch(requests);
    }

    void dispatch(
        const brayns::RequestBuffer::Map &requests,
        const brayns::EntrypointManager &entrypoints,
        brayns::ModelUploadManager &modelUploads)
    {
        for (const auto &[client, packets] : requests)
        {
            for (const auto &packet : packets)
            {
                _dispatch(client, packet);
            }
        }
    }

private:
    JsonRpcHandler _handler;
    brayns::ModelUploadManager &_modelUploads;

    void _dispatch(const brayns::ClientRef > &client, const brayns::InputPacket &packet)
    {
        if (packet.isBinary())
        {
            _dispatchBinary(socket, packet);
            return;
        }
        if (packet.isText())
        {
            _dispatchText(socket, packet);
            return;
        }
        brayns::Log::error("Invalid packet received.");
    }

    void _dispatchBinary(const std::shared_ptr<brayns::WebSocket> &socket, const brayns::InputPacket &packet)
    {
        auto data = packet.getData();
        _modelUploads.processBinaryRequest(socket, data);
    }

    void _dispatchText(const std::shared_ptr<brayns::WebSocket> &socket, const brayns::InputPacket &packet)
    {
        auto data = std::string(packet.getData());
        _handler.processRequest(socket, data);
    }
};

class SocketListener : public brayns::ISocketListener
{
public:
    SocketListener(brayns::ClientManager &clients, brayns::RequestBuffer &requests)
        : _clients(clients)
        , _requests(requests)
    {
    }

    virtual void onRequest(const std::shared_ptr<brayns::WebSocket> &socket, brayns::InputPacket request) override
    {
        auto data = request.isBinary() ? "<Binary data>" : request.getData();
        brayns::Log::info("New request from {}: '{}'.", socket, data);
        _requests.add(socket, std::move(request));
    }

    virtual void onConnect(const std::shared_ptr<brayns::WebSocket> &socket) override
    {
        brayns::Log::info("Client connection : {}.", socket);
        _clients.add(socket);
    }

    virtual void onDisconnect(const std::shared_ptr<brayns::WebSocket> &socket) override
    {
        brayns::Log::info("Client disconnection : {}.", socket);
        _clients.remove(socket);
    }

private:
    brayns::ClientManager &_clients;
    brayns::RequestBuffer &_requests;
};

class SocketFactory
{
public:
    static std::unique_ptr<brayns::ISocket> create(
        const brayns::NetworkParameters &parameters,
        std::unique_ptr<SocketListener> listener)
    {
        if (parameters.isClient())
        {
            return std::make_unique<brayns::ClientSocket>(parameters, std::move(listener));
        }
        return std::make_unique<brayns::ServerSocket>(parameters, std::move(listener));
    }
};

class CoreEntrypointRegistry
{
public:
    CoreEntrypointRegistry(brayns::NetworkContext &context)
        : _entrypoints(context.entrypoints)
        , _clients(context.clients)
        , _streamer(context.streamer)
        , _tasks(context.tasks)
        , _modelUploads(context.modelUploads)
        , _frameExporter(context.frameExporter)
    {
    }

    void registerEntrypoints(brayns::ExtensionPlugin &plugin)
    {
        plugin.add<brayns::GetAnimationParametersEntrypoint>();
        plugin.add<brayns::SetAnimationParametersEntrypoint>();
        plugin.add<brayns::GetCameraEntrypoint>();
        plugin.add<brayns::SetCameraEntrypoint>();
        plugin.add<brayns::ImageJpegEntrypoint>();
        plugin.add<brayns::TriggerJpegStreamEntrypoint>();
        plugin.add<brayns::ImageStreamingModeEntrypoint>();
        plugin.add<brayns::GetRendererEntrypoint>();
        plugin.add<brayns::SetRendererEntrypoint>();
        plugin.add<brayns::VersionEntrypoint>();
        plugin.add<brayns::GetApplicationParametersEntrypoint>();
        plugin.add<brayns::SetApplicationParametersEntrypoint>();
        plugin.add<brayns::GetVolumeParametersEntrypoint>();
        plugin.add<brayns::SetVolumeParametersEntrypoint>();
        plugin.add<brayns::GetSceneEntrypoint>();
        plugin.add<brayns::SetSceneEntrypoint>();
        plugin.add<brayns::GetStatisticsEntrypoint>();
        plugin.add<brayns::SchemaEntrypoint>();
        plugin.add<brayns::InspectEntrypoint>();
        plugin.add<brayns::QuitEntrypoint>();
        plugin.add<brayns::ExitLaterEntrypoint>();
        plugin.add<brayns::ResetCameraEntrypoint>();
        plugin.add<brayns::SnapshotEntrypoint>();
        plugin.add<brayns::RequestModelUploadEntrypoint>();
        plugin.add<brayns::ChunkEntrypoint>();
        plugin.add<brayns::AddModelEntrypoint>();
        plugin.add<brayns::RemoveModelEntrypoint>();
        plugin.add<brayns::UpdateModelEntrypoint>();
        plugin.add<brayns::GetModelEntrypoint>();
        plugin.add<brayns::GetModelPropertiesEntrypoint>();
        plugin.add<brayns::SetModelPropertiesEntrypoint>();
        plugin.add<brayns::ModelPropertiesSchemaEntrypoint>();
        plugin.add<brayns::GetModelTransferFunctionEntrypoint>();
        plugin.add<brayns::SetModelTransferFunctionEntrypoint>();
        plugin.add<brayns::AddClipPlaneEntrypoint>();
        plugin.add<brayns::GetClipPlanesEntrypoint>();
        plugin.add<brayns::UpdateClipPlaneEntrypoint>();
        plugin.add<brayns::RemoveClipPlanesEntrypoint>();
        plugin.add<brayns::GetInstancesEntrypoint>();
        plugin.add<brayns::UpdateInstanceEntrypoint>();
        plugin.add<brayns::GetLoadersEntrypoint>();
        plugin.add<brayns::LoadersSchemaEntrypoint>();
        plugin.add<brayns::GetCameraParamsEntrypoint>();
        plugin.add<brayns::SetCameraParamsEntrypoint>();
        plugin.add<brayns::GetRendererParamsEntrypoint>();
        plugin.add<brayns::SetRendererParamsEntrypoint>();
        plugin.add<brayns::GetLightsEntrypoint>();
        plugin.add<brayns::AddLightDirectionalEntrypoint>();
        plugin.add<brayns::AddLightSphereEntrypoint>();
        plugin.add<brayns::AddLightQuadEntrypoint>();
        plugin.add<brayns::AddLightSpotEntrypoint>();
        plugin.add<brayns::AddLightAmbientEntrypoint>();
        plugin.add<brayns::RemoveLightsEntrypoint>();
        plugin.add<brayns::ClearLightsEntrypoint>();
        plugin.add<brayns::CancelEntrypoint>();
        plugin.add<brayns::RegistryEntrypoint>();
        plugin.add<brayns::ExportFramesEntrypoint>();

        plugin.add<brayns::ExportFramesToDiskEntrypoint>(_frameExporter);
        plugin.add<brayns::GetExportFramesProgressEntrypoint>(_frameExporter);
    }

private:
    const brayns::EntrypointManager &_entrypoints;
    brayns::ClientManager &_clients;
    brayns::StreamManager &_streamer;
    brayns::NetworkTaskManager &_tasks;
    brayns::ModelUploadManager &_modelUploads;
    brayns::FrameExporter &_frameExporter;
};

class NetworkContextBuilder
{
public:
    static void build(brayns::NetworkContext &context, brayns::ExtensionPlugin &plugin)
    {
        _registerInterface(context);
        _setupStream(context);
        _createSocket(context);
        _registerEntrypoints(context, plugin);
    }

private:
    static void _registerInterface(brayns::NetworkContext &context)
    {
        auto &api = *context.api;
        auto &entrypoints = context.entrypoints;
        auto &tasks = context.tasks;
        auto &clients = context.clients;
        auto interface = std::make_unique<brayns::NetworkInterface>(entrypoints, tasks, clients);
        api.setNetworkInterface(std::move(interface));
    }

    static void _registerEntrypoints(brayns::NetworkContext &context, brayns::ExtensionPlugin &plugin)
    {
        CoreEntrypointRegistry registry(context);
        registry.registerEntrypoints(plugin);
    }

    static void _setupStream(brayns::NetworkContext &context)
    {
        auto &api = *context.api;
        auto &streamer = context.streamer;
        auto &manager = api.getParametersManager();
        auto &parameters = manager.getApplicationParameters();
        streamer.setParameters(parameters);
    }

    static void _createSocket(brayns::NetworkContext &context)
    {
        auto &api = *context.api;
        auto &manager = api.getParametersManager();
        auto &parameters = manager.getNetworkParameters();
        auto &clients = context.clients;
        auto &requests = context.requests;
        auto &socket = context.socket;
        auto listener = std::make_unique<SocketListener>(clients, requests);
        socket = SocketFactory::create(parameters, std::move(listener));
    }
};

class NetworkStartup
{
public:
    static void run(brayns::NetworkContext &context)
    {
        auto &entrypoints = context.entrypoints;
        auto &socket = *context.socket;
        entrypoints.onCreate();
        socket.start();
    }
};

class NetworkRequestProcessing
{
public:
    static void run(brayns::NetworkContext &context)
    {
        auto &entrypoints = context.entrypoints;
        auto &modelUploads = context.modelUploads;
        auto &requests = context.requests;
        RequestDispatcher dispatcher(entrypoints, modelUploads);
        dispatcher.dispatch(requests);
    }
};

class NetworkUpdate
{
public:
    static void run(brayns::NetworkContext &context)
    {
        _updateEntrypoints(context);
        _pollTasks(context);
        _pollModelUploads(context);
    }

private:
    static void _updateEntrypoints(brayns::NetworkContext &context)
    {
        auto &entrypoints = context.entrypoints;
        entrypoints.onUpdate();
    }

    static void _pollTasks(brayns::NetworkContext &context)
    {
        auto &tasks = context.tasks;
        tasks.poll();
    }

    static void _pollModelUploads(brayns::NetworkContext &context)
    {
        auto &modelUploads = context.modelUploads;
        modelUploads.poll();
    }
};

class NetworkPreRender
{
public:
    static void run(brayns::NetworkContext &context)
    {
        auto &entrypoints = context.entrypoints;
        entrypoints.onPreRender();
    }
};

class NetworkPostRender
{
public:
    static void run(brayns::NetworkContext &context)
    {
        _notifyEntrypoints(context);
        _broadcastImage(context);
    }

private:
    static void _notifyEntrypoints(brayns::NetworkContext &context)
    {
        auto &entrypoints = context.entrypoints;
        entrypoints.onPostRender();
    }

    static void _broadcastImage(brayns::NetworkContext &context)
    {
        auto &api = *context.api;
        auto &engine = api.getEngine();
        auto &streamer = context.streamer;
        auto &framebuffer = engine.getFrameBuffer();
        auto &clients = context.clients;
        streamer.broadcast(framebuffer, clients);
    }
};
} // namespace

namespace brayns
{
NetworkManager::NetworkManager()
    : ExtensionPlugin("Core")
{
}

void NetworkManager::start()
{
    NetworkStartup::run(_context);
}

void NetworkManager::processRequests()
{
    NetworkRequestProcessing::run(_context);
}

void NetworkManager::update()
{
    NetworkUpdate::run(_context);
}

void NetworkManager::init()
{
    _context.api = _api;
    NetworkContextBuilder::build(_context, *this);
}

void NetworkManager::preRender()
{
    NetworkPreRender::run(_context);
}

void NetworkManager::postRender()
{
    NetworkPostRender::run(_context);
}
} // namespace brayns

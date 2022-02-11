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

#include <brayns/network/interface/NetworkInterface.h>
#include <brayns/network/jsonrpc/JsonRpcException.h>
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
        catch (const brayns::JsonRpcException &e)
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
        catch (const brayns::JsonRpcException &e)
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
        catch (const brayns::JsonRpcException &e)
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

class RequestHandler
{
public:
    static void processRequests(brayns::NetworkContext &context)
    {
        auto &buffer = context.requests;
        auto requests = buffer.extractAll();
        return _dispatch(requests, context);
    }

private:
    static void _dispatch(const brayns::RequestBuffer::Map &requests, brayns::NetworkContext &context)
    {
        for (const auto &[client, packets] : requests)
        {
            for (const auto &packet : packets)
            {
                _dispatchTextOrBinary(client, packet, context);
            }
        }
    }

    static void _dispatchTextOrBinary(
        const brayns::ClientRef &client,
        const brayns::InputPacket &packet,
        brayns::NetworkContext &context)
    {
        if (packet.isBinary())
        {
            _dispatchBinary(client, packet, context);
            return;
        }
        if (packet.isText())
        {
            _dispatchText(client, packet, context);
            return;
        }
        brayns::Log::error("Invalid packet received.");
    }

    static void _dispatchBinary(
        const brayns::ClientRef &client,
        const brayns::InputPacket &packet,
        brayns::NetworkContext &context)
    {
        auto data = packet.getData();
        auto &modelUploads = context.modelUploads;
        ModelUploadHandler::processBinaryRequest(client, data, modelUploads);
    }

    static void _dispatchText(
        const brayns::ClientRef &client,
        const brayns::InputPacket &packet,
        brayns::NetworkContext &context)
    {
        auto data = std::string(packet.getData());
        auto &entrypoints = context.entrypoints;
        JsonRpcHandler::processRequest(client, data, entrypoints);
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

    virtual void onRequest(const brayns::ClientRef &client, brayns::InputPacket request) override
    {
        auto data = request.isBinary() ? "<Binary data>" : request.getData();
        brayns::Log::trace("New request from client {}: '{}'.", client, data);
        _requests.add(client, std::move(request));
    }

    virtual void onConnect(const brayns::ClientRef &client) override
    {
        brayns::Log::info("Connection of client {}.", client);
        _clients.add(client);
    }

    virtual void onDisconnect(const brayns::ClientRef &client) override
    {
        brayns::Log::info("Disconnection of client {}.", client);
        _clients.remove(client);
    }

private:
    brayns::ClientManager &_clients;
    brayns::RequestBuffer &_requests;
};

class SocketFactory
{
public:
    static std::unique_ptr<brayns::ISocket> create(brayns::NetworkContext &context)
    {
        auto &parameters = _getNetworkParameters(context);
        auto listener = _createListener(context);
        if (parameters.isClient())
        {
            return std::make_unique<brayns::ClientSocket>(parameters, std::move(listener));
        }
        return std::make_unique<brayns::ServerSocket>(parameters, std::move(listener));
    }

private:
    static const brayns::NetworkParameters &_getNetworkParameters(brayns::NetworkContext &context)
    {
        auto &api = *context.api;
        auto &manager = api.getParametersManager();
        return manager.getNetworkParameters();
    }

    static std::unique_ptr<brayns::ISocketListener> _createListener(brayns::NetworkContext &context)
    {
        auto &clients = context.clients;
        auto &requests = context.requests;
        return std::make_unique<SocketListener>(clients, requests);
    }
};

class CoreEntrypointRegistry
{
public:
    static void registerEntrypoints(brayns::NetworkContext &context, brayns::ExtensionPlugin &plugin)
    {
        auto &api = *context.api;
        auto &parameters = api.getParametersManager();
        auto &application = parameters.getApplicationParameters();
        auto &animation = parameters.getAnimationParameters();
        auto &volume = parameters.getVolumeParameters();
        auto &rendering = parameters.getRenderingParameters();
        auto &engine = api.getEngine();
        auto &renderer = engine.getRenderer();
        auto &statistics = engine.getStatistics();
        auto &scene = engine.getScene();
        auto &lights = scene.getLightManager();
        auto &camera = api.getCamera();
        auto &loaders = api.getLoaderRegistry();
        auto &interface = *api.getNetworkInterface();

        auto &exporter = context.frameExporter;
        auto &entrypoints = context.entrypoints;
        auto &tasks = context.tasks;
        auto &modelUploads = context.modelUploads;
        auto &stream = context.stream;
        auto &monitor = stream.getMonitor();

        plugin.add<brayns::AddClipPlaneEntrypoint>(engine, interface);
        plugin.add<brayns::AddLightAmbientEntrypoint>(engine);
        plugin.add<brayns::AddLightDirectionalEntrypoint>(engine);
        plugin.add<brayns::AddLightQuadEntrypoint>(engine);
        plugin.add<brayns::AddLightSphereEntrypoint>(engine);
        plugin.add<brayns::AddLightSpotEntrypoint>(engine);
        plugin.add<brayns::AddModelEntrypoint>(engine, loaders, interface);
        plugin.add<brayns::CancelEntrypoint>(tasks);
        plugin.add<brayns::ChunkEntrypoint>(modelUploads);
        plugin.add<brayns::ClearLightsEntrypoint>(engine);
        plugin.add<brayns::ExitLaterEntrypoint>(engine);
        plugin.add<brayns::ExportFramesEntrypoint>(engine, interface);
        plugin.add<brayns::ExportFramesToDiskEntrypoint>(parameters, engine, exporter);
        plugin.add<brayns::GetAnimationParametersEntrypoint>(animation, interface);
        plugin.add<brayns::GetApplicationParametersEntrypoint>(application, interface);
        plugin.add<brayns::GetCameraEntrypoint>(camera, interface);
        plugin.add<brayns::GetCameraParamsEntrypoint>(camera);
        plugin.add<brayns::GetClipPlanesEntrypoint>(scene);
        plugin.add<brayns::GetExportFramesProgressEntrypoint>(exporter);
        plugin.add<brayns::GetInstancesEntrypoint>(scene);
        plugin.add<brayns::GetLightsEntrypoint>(lights);
        plugin.add<brayns::GetLoadersEntrypoint>(loaders);
        plugin.add<brayns::GetModelEntrypoint>(scene);
        plugin.add<brayns::GetModelPropertiesEntrypoint>(scene);
        plugin.add<brayns::GetModelTransferFunctionEntrypoint>(scene);
        plugin.add<brayns::GetRendererEntrypoint>(rendering, interface);
        plugin.add<brayns::GetRendererParamsEntrypoint>(renderer);
        plugin.add<brayns::GetSceneEntrypoint>(scene, interface);
        plugin.add<brayns::GetStatisticsEntrypoint>(statistics, interface);
        plugin.add<brayns::GetVolumeParametersEntrypoint>(volume, interface);
        plugin.add<brayns::ImageJpegEntrypoint>(application, engine);
        plugin.add<brayns::ImageStreamingModeEntrypoint>(application, monitor);
        plugin.add<brayns::InspectEntrypoint>(renderer);
        plugin.add<brayns::LoadersSchemaEntrypoint>(loaders);
        plugin.add<brayns::ModelPropertiesSchemaEntrypoint>(scene);
        plugin.add<brayns::QuitEntrypoint>(engine);
        plugin.add<brayns::RegistryEntrypoint>(entrypoints);
        plugin.add<brayns::RemoveClipPlanesEntrypoint>(engine, interface);
        plugin.add<brayns::RemoveLightsEntrypoint>(engine);
        plugin.add<brayns::RemoveModelEntrypoint>(engine);
        plugin.add<brayns::RequestModelUploadEntrypoint>(engine, loaders, modelUploads);
        plugin.add<brayns::ResetCameraEntrypoint>(engine);
        plugin.add<brayns::SchemaEntrypoint>(entrypoints);
        plugin.add<brayns::SetAnimationParametersEntrypoint>(animation, engine);
        plugin.add<brayns::SetApplicationParametersEntrypoint>(application, engine);
        plugin.add<brayns::SetCameraEntrypoint>(camera, engine);
        plugin.add<brayns::SetCameraParamsEntrypoint>(camera, engine, interface);
        plugin.add<brayns::SetModelPropertiesEntrypoint>(scene);
        plugin.add<brayns::SetModelTransferFunctionEntrypoint>(scene);
        plugin.add<brayns::SetRendererEntrypoint>(rendering, engine);
        plugin.add<brayns::SetRendererParamsEntrypoint>(renderer, engine, interface);
        plugin.add<brayns::SetSceneEntrypoint>(scene, engine);
        plugin.add<brayns::SetVolumeParametersEntrypoint>(volume, engine);
        plugin.add<brayns::SnapshotEntrypoint>(engine, interface);
        plugin.add<brayns::TriggerJpegStreamEntrypoint>(engine, monitor);
        plugin.add<brayns::UpdateClipPlaneEntrypoint>(engine, interface);
        plugin.add<brayns::UpdateInstanceEntrypoint>(engine, interface);
        plugin.add<brayns::UpdateModelEntrypoint>(engine);
        plugin.add<brayns::VersionEntrypoint>();
    }
};

class NetworkContextBuilder
{
public:
    static void build(brayns::NetworkContext &context, brayns::ExtensionPlugin &plugin)
    {
        _registerInterface(context);
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
        CoreEntrypointRegistry::registerEntrypoints(context, plugin);
    }

    static void _createSocket(brayns::NetworkContext &context)
    {
        context.socket = SocketFactory::create(context);
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
        auto &manager = api.getParametersManager();
        auto &parameters = manager.getApplicationParameters();
        auto &engine = api.getEngine();
        auto &stream = context.stream;
        auto &framebuffer = engine.getFrameBuffer();
        auto &clients = context.clients;
        stream.broadcast(framebuffer, clients, parameters);
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
    RequestHandler::processRequests(_context);
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

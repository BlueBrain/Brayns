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

#include <brayns/common/Log.h>

#include <brayns/network/dispatch/RequestDispatcher.h>
#include <brayns/network/entrypoint/EntrypointBuilder.h>
#include <brayns/network/interface/NetworkInterface.h>
#include <brayns/network/jsonrpc/JsonRpcException.h>
#include <brayns/network/jsonrpc/JsonRpcFactory.h>
#include <brayns/network/jsonrpc/JsonRpcParser.h>
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
class SocketListener : public brayns::ISocketListener
{
public:
    SocketListener(brayns::NetworkContext &context)
        : _context(context)
    {
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

    virtual void onRequest(brayns::ClientRequest request) override
    {
        auto &client = request.getClient();
        auto data = request.isBinary() ? "<Binary data>" : request.getData();
        brayns::Log::debug("Received request from client {}: '{}'.", client, data);
        _requests.add(std::move(request));
    }

private:
    brayns::NetworkContext &_context;
};

class SocketFactory
{
public:
    static void create(brayns::NetworkContext &context)
    {
        auto &parameters = _getNetworkParameters(context);
        auto listener = _createListener(context);
        context.socket = _createSocket(parameters, std::move(listener));
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

    static std::unique_ptr<brayns::ISocket> _createSocket(
        const brayns::NetworkParameters &parameters,
        std::unique_ptr<brayns::ISocketListener> listener)
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
    static void registerEntrypoints(brayns::NetworkContext &context, brayns::INetworkInterface &interface)
    {
        EntrypointBuilder builder("Core", interface);

        auto &api = *context.api;

        auto &parameters = api.getParametersManager();
        auto &application = parameters.getApplicationParameters();
        auto &animation = parameters.getAnimationParameters();
        auto &volume = parameters.getVolumeParameters();
        auto &rendering = parameters.getRenderingParameters();

        auto &engine = api.getEngine();
        auto &camera = engine.getCamera();
        auto &renderer = engine.getRenderer();
        auto &scene = engine.getScene();
        auto &lights = scene.getLightManager();
        auto &statistics = engine.getStatistics();

        auto &loaders = api.getLoaderRegistry();

        auto &exporter = context.frameExporter;
        auto &entrypoints = context.entrypoints;
        auto &tasks = context.tasks;
        auto &modelUploads = context.modelUploads;
        auto &stream = context.stream;
        auto &monitor = stream.getMonitor();

        builder.add<brayns::AddClipPlaneEntrypoint>(scene);
        builder.add<brayns::AddLightAmbientEntrypoint>(lights);
        builder.add<brayns::AddLightDirectionalEntrypoint>(lights);
        builder.add<brayns::AddLightQuadEntrypoint>(lights);
        builder.add<brayns::AddLightSphereEntrypoint>(lights);
        builder.add<brayns::AddLightSpotEntrypoint>(lights);
        builder.add<brayns::AddModelEntrypoint>(scene, loaders, interface);
        builder.add<brayns::CancelEntrypoint>(tasks);
        builder.add<brayns::ChunkEntrypoint>(modelUploads);
        builder.add<brayns::ClearLightsEntrypoint>(lights);
        builder.add<brayns::ExitLaterEntrypoint>(engine);
        builder.add<brayns::ExportFramesEntrypoint>(engine, interface);
        builder.add<brayns::ExportFramesToDiskEntrypoint>(parameters, engine, exporter);
        builder.add<brayns::GetAnimationParametersEntrypoint>(animation);
        builder.add<brayns::GetApplicationParametersEntrypoint>(application);
        builder.add<brayns::GetCameraEntrypoint>(camera);
        builder.add<brayns::GetCameraParamsEntrypoint>(camera);
        builder.add<brayns::GetClipPlanesEntrypoint>(scene);
        builder.add<brayns::GetExportFramesProgressEntrypoint>(exporter);
        builder.add<brayns::GetInstancesEntrypoint>(scene);
        builder.add<brayns::GetLightsEntrypoint>(lights);
        builder.add<brayns::GetLoadersEntrypoint>(loaders);
        builder.add<brayns::GetModelEntrypoint>(scene);
        builder.add<brayns::GetModelPropertiesEntrypoint>(scene);
        builder.add<brayns::GetModelTransferFunctionEntrypoint>(scene);
        builder.add<brayns::GetRendererEntrypoint>(rendering);
        builder.add<brayns::GetRendererParamsEntrypoint>(renderer);
        builder.add<brayns::GetSceneEntrypoint>(scene);
        builder.add<brayns::GetStatisticsEntrypoint>(statistics);
        builder.add<brayns::GetVolumeParametersEntrypoint>(volume);
        builder.add<brayns::ImageJpegEntrypoint>(application, engine);
        builder.add<brayns::ImageStreamingModeEntrypoint>(application, monitor);
        builder.add<brayns::InspectEntrypoint>(renderer);
        builder.add<brayns::LoadersSchemaEntrypoint>(loaders);
        builder.add<brayns::ModelPropertiesSchemaEntrypoint>(scene);
        builder.add<brayns::QuitEntrypoint>(engine);
        builder.add<brayns::RegistryEntrypoint>(entrypoints);
        builder.add<brayns::RemoveClipPlanesEntrypoint>(scene);
        builder.add<brayns::RemoveLightsEntrypoint>(lights);
        builder.add<brayns::RemoveModelEntrypoint>(scene);
        builder.add<brayns::RequestModelUploadEntrypoint>(scene, loaders, modelUploads);
        builder.add<brayns::ResetCameraEntrypoint>(camera);
        builder.add<brayns::SchemaEntrypoint>(entrypoints);
        builder.add<brayns::SetAnimationParametersEntrypoint>(animation);
        builder.add<brayns::SetApplicationParametersEntrypoint>(application);
        builder.add<brayns::SetCameraEntrypoint>(camera);
        builder.add<brayns::SetCameraParamsEntrypoint>(camera);
        builder.add<brayns::SetModelPropertiesEntrypoint>(scene);
        builder.add<brayns::SetModelTransferFunctionEntrypoint>(scene);
        builder.add<brayns::SetRendererEntrypoint>(rendering);
        builder.add<brayns::SetRendererParamsEntrypoint>(renderer);
        builder.add<brayns::SetSceneEntrypoint>(scene);
        builder.add<brayns::SetVolumeParametersEntrypoint>(volume);
        builder.add<brayns::SnapshotEntrypoint>(engine, interface);
        builder.add<brayns::TriggerJpegStreamEntrypoint>(monitor);
        builder.add<brayns::UpdateClipPlaneEntrypoint>(scene);
        builder.add<brayns::UpdateInstanceEntrypoint>(scene);
        builder.add<brayns::UpdateModelEntrypoint>(scene);
        builder.add<brayns::VersionEntrypoint>();
    }
};

class NetworkInitialization
{
public:
    static void run(brayns::NetworkContext &context, brayns::ExtensionPlugin &plugin)
    {
        SocketFactory::create(context);
        CoreEntrypointRegistry::registerEntrypoints(context, plugin);
    }
};

class NetworkStartup
{
public:
    static void run(brayns::NetworkContext &context)
    {
        _notifyEntrypoints(context);
        _startSocket(context);
    }

private:
    static void _notifyEntrypoints(brayns::NetworkContext &context)
    {
        auto &entrypoints = context.entrypoints;
        entrypoints.forEach([](auto &entrypoint) { entrypoint.onCreate(); });
    }

    static void _startSocket(brayns::NetworkContext &context)
    {
        auto &socket = *context.socket;
        socket.start();
    }
};

class NetworkPreRender
{
public:
    static void run(brayns::NetworkContext &context)
    {
        _processRequests(context);
        _notifyEntrypoints(context);
    }

private:
    static void _processRequests(brayns::NetworkContext &context)
    {
        auto &socket = *context.socket;
        socket.poll();
    }

    static void _notifyEntrypoints(brayns::NetworkContext &context)
    {
        auto &entrypoints = context.entrypoints;
        entrypoints.forEach([](auto &entrypoint) { entrypoint.onPreRender(); });
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
        entrypoints.forEach([](auto &entrypoint) { entrypoint.onPostRender(); });
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
    , _interface(_context.entrypoints, _context.tasks)
{
    Log::info("Network plugin is enabled.");
}

INetworkInterface &NetworkManager::getInterface()
{
    return _interface;
}

void NetworkManager::start()
{
    Log::info("Network plugin started.");
    NetworkStartup::run(_context);
}

void NetworkManager::init()
{
    Log::info("Initializing network plugin.");
    _context.api = _api;
    NetworkInitialization::run(_context, *this);
}

void NetworkManager::registerEntrypoints(INetworkInterface &interface)
{
    CoreEntrypointRegistry::registerEntrypoints(*_context, interface);
}

void NetworkManager::preRender()
{
    Log::trace("Network pre render.");
    NetworkPreRender::run(_context);
}

void NetworkManager::postRender()
{
    Log::trace("Network post render.");
    NetworkPostRender::run(_context);
}
} // namespace brayns

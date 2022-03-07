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

#include <brayns/network/common/EngineObjectRegisterer.h>
#include <brayns/network/dispatch/RequestDispatcher.h>
#include <brayns/network/entrypoint/EntrypointBuilder.h>
#include <brayns/network/interface/NetworkInterface.h>
#include <brayns/network/jsonrpc/JsonRpcException.h>
#include <brayns/network/jsonrpc/JsonRpcFactory.h>
#include <brayns/network/jsonrpc/JsonRpcParser.h>
#include <brayns/network/jsonrpc/JsonRpcSender.h>
#include <brayns/network/socket/ClientSocket.h>
#include <brayns/network/socket/ServerSocket.h>
#include <brayns/network/socket/SocketListener.h>

#include <brayns/network/entrypoints/AddClipPlaneEntrypoint.h>
#include <brayns/network/entrypoints/AddLightEntrypoint.h>
#include <brayns/network/entrypoints/AddModelEntrypoint.h>
#include <brayns/network/entrypoints/AnimationParametersEntrypoint.h>
#include <brayns/network/entrypoints/ApplicationParametersEntrypoint.h>
#include <brayns/network/entrypoints/CameraEntrypoint.h>
#include <brayns/network/entrypoints/CancelEntrypoint.h>
#include <brayns/network/entrypoints/ClearLightsEntrypoint.h>
#include <brayns/network/entrypoints/ExitLaterEntrypoint.h>
#include <brayns/network/entrypoints/ExportFramesEntrypoint.h>
#include <brayns/network/entrypoints/GetClipPlanesEntrypoint.h>
#include <brayns/network/entrypoints/GetLightsEntrypoint.h>
#include <brayns/network/entrypoints/GetLoadersEntrypoint.h>
#include <brayns/network/entrypoints/GetModelEntrypoint.h>
#include <brayns/network/entrypoints/ImageJpegEntrypoint.h>
#include <brayns/network/entrypoints/ImageStreamingModeEntrypoint.h>
#include <brayns/network/entrypoints/InspectEntrypoint.h>
#include <brayns/network/entrypoints/ModelTransferFunctionEntrypoint.h>
#include <brayns/network/entrypoints/QuitEntrypoint.h>
#include <brayns/network/entrypoints/RegistryEntrypoint.h>
#include <brayns/network/entrypoints/RemoveClipPlanesEntrypoint.h>
#include <brayns/network/entrypoints/RemoveLightsEntrypoint.h>
#include <brayns/network/entrypoints/RemoveModelEntrypoint.h>
#include <brayns/network/entrypoints/RendererEntrypoint.h>
#include <brayns/network/entrypoints/RequestModelUploadEntrypoint.h>
#include <brayns/network/entrypoints/SceneEntrypoint.h>
#include <brayns/network/entrypoints/SchemaEntrypoint.h>
#include <brayns/network/entrypoints/SnapshotEntrypoint.h>
#include <brayns/network/entrypoints/StatisticsEntrypoint.h>
#include <brayns/network/entrypoints/TriggerJpegStreamEntrypoint.h>
#include <brayns/network/entrypoints/UpdateClipPlaneEntrypoint.h>
#include <brayns/network/entrypoints/UpdateModelEntrypoint.h>
#include <brayns/network/entrypoints/VersionEntrypoint.h>

namespace
{
class CoreEntrypointRegistry
{
public:
    static void registerEntrypoints(brayns::NetworkContext &context)
    {
        auto &api = *context.api;
        auto &interface = *context.interface;

        auto &parameters = api.getParametersManager();
        auto &application = parameters.getApplicationParameters();
        auto &animation = parameters.getAnimationParameters();

        auto &engine = api.getEngine();
        auto &camera = engine.getCamera();
        auto &renderer = engine.getRenderer();
        auto &scene = engine.getScene();
        auto &statistics = engine.getStatistics();

        auto &loaders = api.getLoaderRegistry();

        auto &binary = context.binary;
        auto &entrypoints = context.entrypoints;
        auto &tasks = context.tasks;
        auto &stream = context.stream;
        auto &monitor = stream.getMonitor();

        std::shared_ptr<brayns::CameraFactory> cameraFactory = std::make_shared<brayns::CameraFactory>();
        std::shared_ptr<brayns::LightFactory> lightFactory = std::make_shared<brayns::LightFactory>();
        std::shared_ptr<brayns::MaterialFactory> materialFactory = std::make_shared<brayns::MaterialFactory>();
        std::shared_ptr<brayns::RendererFactory> rendererFactory = std::make_shared<brayns::RendererFactory>();

        brayns::CancellationToken token(interface);
        brayns::EntrypointBuilder builder("Core", interface);

        builder.add<brayns::AddClipPlaneEntrypoint>(scene);
        builder.add<brayns::AddLightAmbientEntrypoint>(scene);
        builder.add<brayns::AddLightDirectionalEntrypoint>(scene);
        builder.add<brayns::AddLightQuadEntrypoint>(scene);
        builder.add<brayns::AddModelEntrypoint>(scene, loaders, token);
        builder.add<brayns::CancelEntrypoint>(tasks);
        builder.add<brayns::ClearLightsEntrypoint>(scene);
        builder.add<brayns::ExitLaterEntrypoint>(engine);
        builder.add<brayns::ExportFramesEntrypoint>(engine, token);
        builder.add<brayns::GetAnimationParametersEntrypoint>(animation);
        builder.add<brayns::GetApplicationParametersEntrypoint>(application);
        builder.add<brayns::GetCameraEntrypoint>(engine, cameraFactory);
        builder.add<brayns::GetClipPlanesEntrypoint>(scene);
        builder.add<brayns::GetLightsEntrypoint>(scene);
        builder.add<brayns::GetLoadersEntrypoint>(loaders);
        builder.add<brayns::GetModelTransferFunctionEntrypoint>(scene);
        builder.add<brayns::GetRendererEntrypoint>(engine, rendererFactory);
        builder.add<brayns::GetSceneEntrypoint>(scene);
        builder.add<brayns::GetStatisticsEntrypoint>(statistics);
        builder.add<brayns::ImageJpegEntrypoint>(application, engine);
        builder.add<brayns::ImageStreamingModeEntrypoint>(application, monitor);
        builder.add<brayns::InspectEntrypoint>(renderer);
        builder.add<brayns::QuitEntrypoint>(engine);
        builder.add<brayns::RegistryEntrypoint>(entrypoints);
        builder.add<brayns::RemoveClipPlanesEntrypoint>(scene);
        builder.add<brayns::RemoveLightsEntrypoint>(scene);
        builder.add<brayns::RemoveModelEntrypoint>(scene);
        builder.add<brayns::RequestModelUploadEntrypoint>(scene, loaders, binary, token);
        builder.add<brayns::SchemaEntrypoint>(entrypoints);
        builder.add<brayns::SetAnimationParametersEntrypoint>(animation);
        builder.add<brayns::SetApplicationParametersEntrypoint>(application);
        builder.add<brayns::SetCameraOrthographicEntrypoint>(engine);
        builder.add<brayns::SetCameraPerspectiveEntrypoint>(engine);
        builder.add<brayns::SetModelTransferFunctionEntrypoint>(scene);
        builder.add<brayns::SetRendererInteractiveEntrypoint>(engine);
        builder.add<brayns::SetRendererProductionEntrypoint>(engine);
        builder.add<brayns::SnapshotEntrypoint>(engine, interface);
        builder.add<brayns::TriggerJpegStreamEntrypoint>(monitor);
        builder.add<brayns::UpdateClipPlaneEntrypoint>(scene);
        builder.add<brayns::UpdateModelEntrypoint>(scene);
        builder.add<brayns::VersionEntrypoint>();
    }
};

class SocketFactory
{
public:
    static std::unique_ptr<brayns::ISocket> create(brayns::NetworkContext &context)
    {
        auto &parameters = _getParameters(context);
        auto listener = _createListener(context);
        if (parameters.isClient())
        {
            return std::make_unique<brayns::ClientSocket>(parameters, std::move(listener));
        }
        return std::make_unique<brayns::ServerSocket>(parameters, std::move(listener));
    }

private:
    static const brayns::NetworkParameters &_getParameters(brayns::NetworkContext &context)
    {
        auto &api = *context.api;
        auto &manager = api.getParametersManager();
        return manager.getNetworkParameters();
    }

    static std::unique_ptr<brayns::ISocketListener> _createListener(brayns::NetworkContext &context)
    {
        auto &binary = context.binary;
        auto &clients = context.clients;
        auto &entrypoints = context.entrypoints;
        auto &tasks = context.tasks;
        return std::make_unique<brayns::SocketListener>(binary, clients, entrypoints, tasks);
    }
};

class NetworkInitialization
{
public:
    static void run(brayns::NetworkContext &context)
    {
        _createSocket(context);
        _createInterface(context);
    }

private:
    static void _createSocket(brayns::NetworkContext &context)
    {
        context.socket = SocketFactory::create(context);
    }

    static void _createInterface(brayns::NetworkContext &context)
    {
        auto &entrypoints = context.entrypoints;
        auto &socket = *context.socket;
        context.interface = std::make_unique<brayns::NetworkInterface>(entrypoints, socket);
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
        entrypoints.onCreate();
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
        _pollSocket(context);
        _runTasks(context);
        _notifyEntrypoints(context);
    }

private:
    static void _pollSocket(brayns::NetworkContext &context)
    {
        auto &socket = *context.socket;
        socket.poll();
    }

    static void _runTasks(brayns::NetworkContext &context)
    {
        auto &tasks = context.tasks;
        tasks.runAllTasks();
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
{
    Log::info("Network plugin is enabled.");
}

INetworkInterface &NetworkManager::getInterface()
{
    return *_context.interface;
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
    NetworkInitialization::run(_context);
}

void NetworkManager::registerEntrypoints(INetworkInterface &interface)
{
    (void)interface;
    CoreEntrypointRegistry::registerEntrypoints(_context);
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

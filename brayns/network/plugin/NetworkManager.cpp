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
#include <brayns/network/socket/SocketListener.h>

#include <brayns/network/entrypoints/AddClipPlaneEntrypoint.h>
#include <brayns/network/entrypoints/AddGeometryEntrypoint.h>
#include <brayns/network/entrypoints/AddLightEntrypoint.h>
#include <brayns/network/entrypoints/AddModelEntrypoint.h>
#include <brayns/network/entrypoints/ApplicationParametersEntrypoint.h>
#include <brayns/network/entrypoints/CameraEntrypoint.h>
#include <brayns/network/entrypoints/CameraLookAtEntrypoint.h>
#include <brayns/network/entrypoints/CancelEntrypoint.h>
#include <brayns/network/entrypoints/ClearClipPlanesEntrypoint.h>
#include <brayns/network/entrypoints/ClearLightsEntrypoint.h>
#include <brayns/network/entrypoints/ClearModelsEntrypoint.h>
#include <brayns/network/entrypoints/EnableSimulationEntrypoint.h>
#include <brayns/network/entrypoints/ExitLaterEntrypoint.h>
#include <brayns/network/entrypoints/ExportFramesEntrypoint.h>
#include <brayns/network/entrypoints/GetLoadersEntrypoint.h>
#include <brayns/network/entrypoints/GetModelEntrypoint.h>
#include <brayns/network/entrypoints/ImageJpegEntrypoint.h>
#include <brayns/network/entrypoints/InspectEntrypoint.h>
#include <brayns/network/entrypoints/ModelColorRampEntrypoint.h>
#include <brayns/network/entrypoints/ModelMaterialEntrypoint.h>
#include <brayns/network/entrypoints/QuitEntrypoint.h>
#include <brayns/network/entrypoints/RegistryEntrypoint.h>
#include <brayns/network/entrypoints/RemoveModelEntrypoint.h>
#include <brayns/network/entrypoints/RendererEntrypoint.h>
#include <brayns/network/entrypoints/SceneEntrypoint.h>
#include <brayns/network/entrypoints/SchemaEntrypoint.h>
#include <brayns/network/entrypoints/SimulationParametersEntrypoint.h>
#include <brayns/network/entrypoints/SnapshotEntrypoint.h>
#include <brayns/network/entrypoints/TriggerJpegStreamEntrypoint.h>
#include <brayns/network/entrypoints/UpdateModelEntrypoint.h>
#include <brayns/network/entrypoints/UploadModelEntrypoint.h>
#include <brayns/network/entrypoints/VersionEntrypoint.h>

namespace
{
class CoreEntrypointRegistry
{
public:
    static void registerEntrypoints(brayns::NetworkContext &context)
    {
        auto &api = context.api;
        auto &interface = *context.interface;

        auto &parameters = api.getParametersManager();
        auto &application = parameters.getApplicationParameters();
        auto &simulation = parameters.getSimulationParameters();

        auto &engine = api.getEngine();
        auto &scene = engine.getScene();
        auto &models = scene.getModels();

        auto &loaders = api.getLoaderRegistry();

        auto &entrypoints = context.entrypoints;
        auto &tasks = context.tasks;
        auto &stream = context.stream;

        brayns::CancellationToken token(interface);
        brayns::EntrypointBuilder builder("Core", interface);

        builder.add<brayns::AddBoxesEntrypoint>(models);
        builder.add<brayns::AddCapsulesEntrypoint>(models);
        builder.add<brayns::AddClipPlaneEntrypoint>(models);
        builder.add<brayns::AddLightAmbientEntrypoint>(models);
        builder.add<brayns::AddLightDirectionalEntrypoint>(models);
        builder.add<brayns::AddLightQuadEntrypoint>(models);
        builder.add<brayns::AddModelEntrypoint>(models, loaders, simulation, token);
        builder.add<brayns::AddPlanesEntrypoint>(models);
        builder.add<brayns::AddSpheresEntrypoint>(models);
        builder.add<brayns::CancelEntrypoint>(tasks);
        builder.add<brayns::ClearClipPlanesEntrypoint>(models);
        builder.add<brayns::ClearLightsEntrypoint>(models);
        builder.add<brayns::ClearModelsEntrypoint>(models, simulation);
        builder.add<brayns::EnableSimulationEntrypoint>(models);
        builder.add<brayns::ExitLaterEntrypoint>(engine);
        builder.add<brayns::ExportFramesEntrypoint>(engine, parameters, token);
        builder.add<brayns::GetApplicationParametersEntrypoint>(application);
        builder.add<brayns::GetCameraLookAtEntrypoint>(engine);
        builder.add<brayns::GetCameraOrthographicEntrypoint>(engine);
        builder.add<brayns::GetCameraPerspectiveEntrypoint>(engine);
        builder.add<brayns::GetCameraTypeEntrypoint>(engine);
        builder.add<brayns::GetLoadersEntrypoint>(loaders);
        builder.add<brayns::GetMaterialCarPaint>(models);
        builder.add<brayns::GetMaterialPhong>(models);
        builder.add<brayns::GetMaterialEmissive>(models);
        builder.add<brayns::GetMaterialGlass>(models);
        builder.add<brayns::GetMaterialMatte>(models);
        builder.add<brayns::GetMaterialMetal>(models);
        builder.add<brayns::GetMaterialPlastic>(models);
        builder.add<brayns::GetMaterialType>(models);
        builder.add<brayns::GetModelEntrypoint>(models);
        builder.add<brayns::GetModelTransferFunctionEntrypoint>(models);
        builder.add<brayns::GetRendererInteractiveEntrypoint>(engine);
        builder.add<brayns::GetRendererProductionEntrypoint>(engine);
        builder.add<brayns::GetRendererTypeEntrypoint>(engine);
        builder.add<brayns::GetSceneEntrypoint>(scene);
        builder.add<brayns::GetSimulationParametersEntrypoint>(simulation);
        builder.add<brayns::ImageJpegEntrypoint>(application, engine);
        builder.add<brayns::InspectEntrypoint>(engine);
        builder.add<brayns::QuitEntrypoint>(engine);
        builder.add<brayns::RegistryEntrypoint>(entrypoints);
        builder.add<brayns::RemoveModelEntrypoint>(models, simulation);
        builder.add<brayns::UploadModelEntrypoint>(models, loaders, simulation, token);
        builder.add<brayns::SchemaEntrypoint>(entrypoints);
        builder.add<brayns::SetApplicationParametersEntrypoint>(application);
        builder.add<brayns::SetCameraLookAtEntrypoint>(engine);
        builder.add<brayns::SetCameraOrthographicEntrypoint>(engine);
        builder.add<brayns::SetCameraPerspectiveEntrypoint>(engine);
        builder.add<brayns::SetMaterialCarPaint>(models);
        builder.add<brayns::SetMaterialPhong>(models);
        builder.add<brayns::SetMaterialEmissive>(models);
        builder.add<brayns::SetMaterialGlass>(models);
        builder.add<brayns::SetMaterialMatte>(models);
        builder.add<brayns::SetMaterialMetal>(models);
        builder.add<brayns::SetMaterialPlastic>(models);
        builder.add<brayns::SetModelTransferFunctionEntrypoint>(models);
        builder.add<brayns::SetRendererInteractiveEntrypoint>(engine);
        builder.add<brayns::SetRendererProductionEntrypoint>(engine);
        builder.add<brayns::SetSimulationParametersEntrypoint>(simulation);
        builder.add<brayns::SnapshotEntrypoint>(engine, interface);
        builder.add<brayns::TriggerJpegStreamEntrypoint>(stream);
        builder.add<brayns::UpdateModelEntrypoint>(models);
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
        auto &api = context.api;
        auto &manager = api.getParametersManager();
        return manager.getNetworkParameters();
    }

    static std::unique_ptr<brayns::ISocketListener> _createListener(brayns::NetworkContext &context)
    {
        auto &clients = context.clients;
        auto &entrypoints = context.entrypoints;
        auto &tasks = context.tasks;
        return std::make_unique<brayns::SocketListener>(clients, entrypoints, tasks);
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
        auto &api = context.api;
        auto &manager = api.getParametersManager();
        auto &parameters = manager.getApplicationParameters();
        auto &engine = api.getEngine();
        auto &stream = context.stream;
        auto &framebuffer = engine.getFramebuffer();
        auto &clients = context.clients;
        stream.broadcast(framebuffer, clients, parameters);
    }
};
} // namespace

namespace brayns
{
NetworkContext::NetworkContext(PluginAPI &pluginAPI)
    : api(pluginAPI)
{
}

NetworkManager::NetworkManager(PluginAPI &api)
    : _context(api)
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

void NetworkManager::onCreate()
{
    Log::info("Initializing network plugin.");
    NetworkInitialization::run(_context);
}

void NetworkManager::registerEntrypoints(INetworkInterface &interface)
{
    (void)interface;
    CoreEntrypointRegistry::registerEntrypoints(_context);
}

void NetworkManager::onPreRender()
{
    Log::trace("Network pre render.");
    NetworkPreRender::run(_context);
}

void NetworkManager::onPostRender()
{
    Log::trace("Network post render.");
    NetworkPostRender::run(_context);
}
} // namespace brayns

/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include <brayns/utils/Log.h>

#include <brayns/network/entrypoint/EntrypointBuilder.h>
#include <brayns/network/socket/ClientSocket.h>
#include <brayns/network/socket/ServerSocket.h>
#include <brayns/network/socket/SocketListener.h>

#include <brayns/network/entrypoints/AddClipPlaneEntrypoint.h>
#include <brayns/network/entrypoints/AddClippingGeometryEntrypoint.h>
#include <brayns/network/entrypoints/AddGeometryEntrypoint.h>
#include <brayns/network/entrypoints/AddLightEntrypoint.h>
#include <brayns/network/entrypoints/AddModelEntrypoint.h>
#include <brayns/network/entrypoints/ApplicationParametersEntrypoint.h>
#include <brayns/network/entrypoints/CameraEntrypoint.h>
#include <brayns/network/entrypoints/CameraViewEntrypoint.h>
#include <brayns/network/entrypoints/CancelEntrypoint.h>
#include <brayns/network/entrypoints/ClearClippingGeometriesEntrypoint.h>
#include <brayns/network/entrypoints/ClearLightsEntrypoint.h>
#include <brayns/network/entrypoints/ClearModelsEntrypoint.h>
#include <brayns/network/entrypoints/ClearRenderablesEntrypoint.h>
#include <brayns/network/entrypoints/ColorRampEntrypoint.h>
#include <brayns/network/entrypoints/EnableSimulationEntrypoint.h>
#include <brayns/network/entrypoints/ExitLaterEntrypoint.h>
#include <brayns/network/entrypoints/FramebufferEntrypoint.h>
#include <brayns/network/entrypoints/GetLoadersEntrypoint.h>
#include <brayns/network/entrypoints/GetModelEntrypoint.h>
#include <brayns/network/entrypoints/InspectEntrypoint.h>
#include <brayns/network/entrypoints/InstantiateModelEntrypoint.h>
#include <brayns/network/entrypoints/MaterialEntrypoint.h>
#include <brayns/network/entrypoints/ModelColoringEntrypoint.h>
#include <brayns/network/entrypoints/QuitEntrypoint.h>
#include <brayns/network/entrypoints/RegistryEntrypoint.h>
#include <brayns/network/entrypoints/RemoveModelEntrypoint.h>
#include <brayns/network/entrypoints/RenderImageEntrypoint.h>
#include <brayns/network/entrypoints/RendererEntrypoint.h>
#include <brayns/network/entrypoints/SceneEntrypoint.h>
#include <brayns/network/entrypoints/SchemaEntrypoint.h>
#include <brayns/network/entrypoints/SimulationParametersEntrypoint.h>
#include <brayns/network/entrypoints/SnapshotEntrypoint.h>
#include <brayns/network/entrypoints/UpdateModelEntrypoint.h>
#include <brayns/network/entrypoints/UploadModelEntrypoint.h>
#include <brayns/network/entrypoints/VersionEntrypoint.h>

namespace
{
class CoreEntrypointRegistry
{
public:
    static void registerEntrypoints(
        brayns::INetworkInterface &interface,
        brayns::PluginAPI &api,
        const brayns::EntrypointRegistry &entrypoints,
        brayns::TaskManager &tasks)
    {
        auto &parameters = api.getParametersManager();
        auto &application = parameters.getApplicationParameters();
        auto &simulation = parameters.getSimulationParameters();

        auto &engine = api.getEngine();
        auto &scene = engine.getScene();
        auto &models = scene.getModels();

        auto &loaders = api.getLoaderRegistry();

        brayns::CancellationToken token(interface);
        brayns::EntrypointBuilder builder("Core", interface);

        builder.add<brayns::AddBoundedPlanesEntrypoint>(models);
        builder.add<brayns::AddBoxesEntrypoint>(models);
        builder.add<brayns::AddCapsulesEntrypoint>(models);
        builder.add<brayns::AddClippingBoundedPlanesEntrypoint>(models);
        builder.add<brayns::AddClippingBoxesEntrypoint>(models);
        builder.add<brayns::AddClippingCapsulesEntrypoint>(models);
        builder.add<brayns::AddClippingPlanesEntrypoint>(models);
        builder.add<brayns::AddClippingSpheresEntrypoint>(models);
        builder.add<brayns::AddClipPlaneEntrypoint>(models);
        builder.add<brayns::AddLightAmbientEntrypoint>(models);
        builder.add<brayns::AddLightDirectionalEntrypoint>(models);
        builder.add<brayns::AddLightQuadEntrypoint>(models);
        builder.add<brayns::AddModelEntrypoint>(models, loaders, simulation, token);
        builder.add<brayns::AddPlanesEntrypoint>(models);
        builder.add<brayns::AddSpheresEntrypoint>(models);
        builder.add<brayns::CancelEntrypoint>(tasks);
        builder.add<brayns::ClearClipPlanesEntrypoint>(models);
        builder.add<brayns::ClearClippingGeometriesEntrypoint>(models);
        builder.add<brayns::ClearLightsEntrypoint>(models);
        builder.add<brayns::ClearModelsEntrypoint>(models, simulation);
        builder.add<brayns::ClearRenderablesEntrypoint>(models, simulation);
        builder.add<brayns::ColorModelEntrypoint>(models);
        builder.add<brayns::EnableSimulationEntrypoint>(models);
        builder.add<brayns::ExitLaterEntrypoint>(engine);
        builder.add<brayns::GetApplicationParametersEntrypoint>(application);
        builder.add<brayns::GetCameraViewEntrypoint>(engine);
        builder.add<brayns::GetCameraOrthographicEntrypoint>(engine);
        builder.add<brayns::GetCameraPerspectiveEntrypoint>(engine);
        builder.add<brayns::GetCameraTypeEntrypoint>(engine);
        builder.add<brayns::GetColorMethodsEntrypoint>(models);
        builder.add<brayns::GetColorRampEntrypoint>(models);
        builder.add<brayns::GetColorValuesEntrypoint>(models);
        builder.add<brayns::GetLoadersEntrypoint>(loaders);
        builder.add<brayns::GetMaterialCarPaint>(models);
        builder.add<brayns::GetMaterialEmissive>(models);
        builder.add<brayns::GetMaterialGlass>(models);
        builder.add<brayns::GetMaterialMatte>(models);
        builder.add<brayns::GetMaterialMetal>(models);
        builder.add<brayns::GetMaterialPhong>(models);
        builder.add<brayns::GetMaterialPlastic>(models);
        builder.add<brayns::GetMaterialType>(models);
        builder.add<brayns::GetModelEntrypoint>(models);
        builder.add<brayns::GetRendererInteractiveEntrypoint>(engine);
        builder.add<brayns::GetRendererProductionEntrypoint>(engine);
        builder.add<brayns::GetRendererTypeEntrypoint>(engine);
        builder.add<brayns::GetSceneEntrypoint>(scene);
        builder.add<brayns::GetSimulationParametersEntrypoint>(simulation);
        builder.add<brayns::InspectEntrypoint>(engine);
        builder.add<brayns::InstantiateModelEntrypoint>(models);
        builder.add<brayns::QuitEntrypoint>(engine);
        builder.add<brayns::RegistryEntrypoint>(entrypoints);
        builder.add<brayns::RemoveModelEntrypoint>(models, simulation);
        builder.add<brayns::RenderImageEntrypoint>(engine);
        builder.add<brayns::SchemaEntrypoint>(entrypoints);
        builder.add<brayns::SetApplicationParametersEntrypoint>(application);
        builder.add<brayns::SetCameraViewEntrypoint>(engine);
        builder.add<brayns::SetCameraOrthographicEntrypoint>(engine);
        builder.add<brayns::SetCameraPerspectiveEntrypoint>(engine);
        builder.add<brayns::SetColorRampEntrypoint>(models);
        builder.add<brayns::SetProgressiveFramebufferEntrypoint>(engine);
        builder.add<brayns::SetStaticFramebufferEntrypoint>(engine);
        builder.add<brayns::SetMaterialCarPaint>(models);
        builder.add<brayns::SetMaterialEmissive>(models);
        builder.add<brayns::SetMaterialGlass>(models);
        builder.add<brayns::SetMaterialMatte>(models);
        builder.add<brayns::SetMaterialMetal>(models);
        builder.add<brayns::SetMaterialPhong>(models);
        builder.add<brayns::SetMaterialPlastic>(models);
        builder.add<brayns::SetRendererInteractiveEntrypoint>(engine);
        builder.add<brayns::SetRendererProductionEntrypoint>(engine);
        builder.add<brayns::SetSimulationParametersEntrypoint>(simulation);
        builder.add<brayns::SnapshotEntrypoint>(engine, token);
        builder.add<brayns::UpdateModelEntrypoint>(models);
        builder.add<brayns::UploadModelEntrypoint>(models, loaders, simulation, token);
        builder.add<brayns::VersionEntrypoint>();
    }
};

class SocketFactory
{
public:
    static std::unique_ptr<brayns::ISocket> createSocket(
        brayns::PluginAPI &api,
        std::unique_ptr<brayns::ISocketListener> listener)
    {
        auto &manager = api.getParametersManager();
        auto &parameters = manager.getNetworkParameters();
        return createSocket(parameters, std::move(listener));
    }

    static std::unique_ptr<brayns::ISocket> createSocket(
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
} // namespace

namespace brayns
{
NetworkManager::NetworkManager(PluginAPI &api)
    : _api(api)
{
    auto listener = std::make_unique<brayns::SocketListener>(_clients, _entrypoints, _tasks);
    _socket = SocketFactory::createSocket(_api, std::move(listener));
    CoreEntrypointRegistry::registerEntrypoints(*this, _api, _entrypoints, _tasks);
}

void NetworkManager::start()
{
    _socket->start();
}

void NetworkManager::stop()
{
    _socket->stop();
}

void NetworkManager::update()
{
    Log::trace("Network update");
    _socket->poll();
    _tasks.runAllTasks();
    _entrypoints.forEach([](auto &entrypoint) { entrypoint.onUpdate(); });
}

void NetworkManager::registerEntrypoint(EntrypointRef entrypoint)
{
    auto &plugin = entrypoint.getPlugin();
    auto &method = entrypoint.getMethod();
    Log::info("Plugin '{}' registered entrypoint '{}'.", plugin, method);
    _entrypoints.add(std::move(entrypoint));
}

void NetworkManager::poll()
{
    Log::trace("Poll network requests from plugin or entrypoint");
    _socket->poll();
}
} // namespace brayns

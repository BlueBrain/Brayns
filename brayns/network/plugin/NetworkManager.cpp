/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include <brayns/network/entrypoints/AnimationParametersEntrypoint.h>
#include <brayns/network/entrypoints/CameraEntrypoint.h>
#include <brayns/network/entrypoints/SchemaEntrypoint.h>
#include <brayns/network/entrypoints/TestEntrypoint.h>
#include <brayns/network/interface/ServerInterface.h>
#include <brayns/pluginapi/PluginAPI.h>

using namespace brayns;

namespace
{
using NetworkInterfacePtr = std::shared_ptr<NetworkInterface>;

class NetworkInterfaceFactory
{
public:
    static NetworkInterfacePtr createNetworkInterface(PluginAPI& api)
    {
        return std::make_shared<ServerInterface>(api);
    }
};

class EntrypointManager
{
public:
    static void registerEntrypoints(NetworkInterface& interface)
    {
        interface.add<GetAnimationParametersEntrypoint>();
        interface.add<SetAnimationParametersEntrypoint>();
        interface.add<GetCameraEntrypoint>();
        interface.add<SetCameraEntrypoint>();
        interface.add<SchemaEntrypoint>();
        interface.add<TestEntrypoint>();
    }
};
} // namespace

namespace brayns
{
NetworkManager::~NetworkManager()
{
    if (_api->getActionInterface() == _actionInterface.get())
    {
        _api->setActionInterface(nullptr);
    }
}

void NetworkManager::init()
{
    auto interface = NetworkInterfaceFactory::createNetworkInterface(*_api);
    _actionInterface = interface;
    _api->setActionInterface(_actionInterface);
    EntrypointManager::registerEntrypoints(*interface);
}
} // namespace brayns

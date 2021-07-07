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

#include <brayns/network/context/NetworkContext.h>
#include <brayns/network/interface/ServerInterface.h>
#include <brayns/network/stream/StreamManager.h>

#include <brayns/network/entrypoints/AnimationParametersEntrypoint.h>
#include <brayns/network/entrypoints/CameraEntrypoint.h>
#include <brayns/network/entrypoints/ImageJpegEntrypoint.h>
#include <brayns/network/entrypoints/ImageStreamingModeEntrypoint.h>
#include <brayns/network/entrypoints/SchemaEntrypoint.h>
#include <brayns/network/entrypoints/TestEntrypoint.h>
#include <brayns/network/entrypoints/TriggerJpegStreamEntrypoint.h>

namespace
{
using namespace brayns;

class EntrypointManager
{
public:
    static void registerEntrypoints(ActionInterface& interface)
    {
        interface.add<GetAnimationParametersEntrypoint>();
        interface.add<SetAnimationParametersEntrypoint>();
        interface.add<GetCameraEntrypoint>();
        interface.add<SetCameraEntrypoint>();
        interface.add<ImageJpegEntrypoint>();
        interface.add<TriggerJpegStreamEntrypoint>();
        interface.add<ImageStreamingModeEntrypoint>();
        interface.add<SchemaEntrypoint>();
        interface.add<TestEntrypoint>();
    }
};
} // namespace

namespace brayns
{
NetworkManager::NetworkManager() {}

NetworkManager::~NetworkManager()
{
    if (_api->getActionInterface() != _interface.get())
    {
        return;
    }
    _api->setActionInterface(nullptr);
}

void NetworkManager::init()
{
    _context = std::make_unique<NetworkContext>(*_api);
    _interface = std::make_shared<ServerInterface>(*_context);
    _api->setActionInterface(_interface);
    EntrypointManager::registerEntrypoints(*_interface);
}

void NetworkManager::postRender()
{
    auto lock = _context->lock();
    auto& entrypoints = _context->getEntrypoints();
    entrypoints.update();
    StreamManager::broadcast(*_context);
}
} // namespace brayns

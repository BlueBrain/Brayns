/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include "BaseEntrypoint.h"

#include <brayns/common/Log.h>
#include <brayns/network/json/MessageFactory.h>

#include "EntrypointException.h"

namespace brayns
{
NetworkContext &BaseEntrypoint::getContext() const
{
    return *_context;
}

PluginAPI &BaseEntrypoint::getApi() const
{
    return _context->getApi();
}

EntrypointManager &BaseEntrypoint::getEntrypoints() const
{
    return _context->getEntrypoints();
}

ConnectionManager &BaseEntrypoint::getConnections() const
{
    return _context->getConnections();
}

StreamManager &BaseEntrypoint::getStream() const
{
    return _context->getStream();
}

NetworkTaskManager &BaseEntrypoint::getTasks() const
{
    return _context->getTasks();
}

BinaryRequestManager &BaseEntrypoint::getBinary() const
{
    return _context->getBinary();
}

const std::string &BaseEntrypoint::getPlugin() const
{
    return _plugin;
}

void BaseEntrypoint::setPlugin(const std::string &plugin)
{
    _plugin = plugin;
}

void BaseEntrypoint::setContext(NetworkContext &context)
{
    _context = &context;
}

void BaseEntrypoint::triggerRender() const
{
    auto &engine = getApi().getEngine();
    engine.triggerRender();
}

void BaseEntrypoint::notify(const JsonValue &params) const
{
    try
    {
        NotificationMessage notification;
        notification.jsonrpc = "2.0";
        notification.method = getName();
        notification.params = params;
        auto json = Json::stringify(notification);
        auto &connections = _context->getConnections();
        connections.broadcast(json);
    }
    catch (...)
    {
        Log::error("Error during notification.");
    }
}
} // namespace brayns

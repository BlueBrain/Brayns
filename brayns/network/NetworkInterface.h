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

#pragma once

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include <brayns/common/ActionInterface.h>
#include <brayns/parameters/ParametersManager.h>
#include <brayns/pluginapi/PluginAPI.h>

#include "NetworkRequest.h"

namespace brayns
{
class EntryPoints
{
public:
    EntryPoints(PluginAPI& api)
        : _api(&api)
    {
    }

    const EntryPoint* find(const std::string& name) const
    {
        auto i = _entryPoints.find(name);
        return i == _entryPoints.end() ? nullptr : i->second.get();
    }

    void add(EntryPointPtr entryPoint)
    {
        assert(entryPoint);
        entryPoint->setApi(*_api);
        entryPoint->onCreate();
        auto& name = entryPoint->getName();
        assert(!name.empty());
        _entryPoints[name] = std::move(entryPoint);
    }

private:
    PluginAPI* _api;
    std::unordered_map<std::string, EntryPointPtr> _entryPoints;
};

class EntryPointException : public std::runtime_error
{
public:
    EntryPointException(const std::string& message)
        : std::runtime_error(message)
    {
    }

    EntryPointException(int code, const std::string& message)
        : std::runtime_error(message)
        , _code(code)
    {
    }

    int getCode() const { return _code; }

private:
    int _code = 0;
};

class MessageReceiver
{
public:
    static RequestMessage receive(NetworkSocket& socket)
    {
        auto packet = _receivePacket(socket);
        return _parseMessage(packet);
    }

private:
    static InputPacket _receivePacket(NetworkSocket& socket)
    {
        auto packet = socket.receive();
        if (packet.isClose())
        {
            throw NetworkException("Connection closed");
        }
        if (!packet.isText())
        {
            throw EntryPointException("Text frame expected");
        }
        return packet;
    }

    static RequestMessage _parseMessage(const InputPacket& packet)
    {
        RequestMessage message;
        try
        {
            message = Json::parse<RequestMessage>(packet.getData());
        }
        catch (Poco::JSON::JSONException& e)
        {
            throw EntryPointException("Failed to parse JSON request: " +
                                      e.displayText());
        }
        if (message.jsonrpc != "2.0")
        {
            throw EntryPointException("Unsupported JSON-RPC version: " +
                                      message.jsonrpc);
        }
        if (message.id.empty())
        {
            throw EntryPointException("Missing message ID");
        }
        return message;
    }
};

class NetworkTransaction
{
public:
    static bool run(const EntryPoints& entryPoints, NetworkSocket& socket)
    {
        NetworkRequest request(socket);
        try
        {
            auto message = MessageReceiver::receive(socket);
            request.setMessage(std::move(message));
            _run(entryPoints, request);
        }
        catch (EntryPointException& e)
        {
            request.sendError(e.getCode(), e.what());
        }
        catch (NetworkException& e)
        {
            std::cout << e.what() << '\n';
            return false;
        }
        catch (std::exception& e)
        {
            request.sendError(0, e.what());
        }
        return true;
    }

private:
    static void _run(const EntryPoints& entryPoints, NetworkRequest& request)
    {
        auto& method = request.getMethod();
        auto entryPoint = entryPoints.find(method);
        if (!entryPoint)
        {
            throw EntryPointException("Invalid entrypoint: " + method);
        }
        entryPoint->processRequest(request);
    }
};

class NetworkInterface : public ActionInterface
{
public:
    NetworkInterface(PluginAPI& api)
        : _entryPoints(api)
    {
    }

    void run(NetworkSocket& socket)
    {
        while (NetworkTransaction::run(_entryPoints, socket))
        {
        }
    }

    virtual void addEntryPoint(EntryPointPtr entryPoint) override
    {
        _entryPoints.add(std::move(entryPoint));
    }

private:
    EntryPoints _entryPoints;
};
} // namespace brayns
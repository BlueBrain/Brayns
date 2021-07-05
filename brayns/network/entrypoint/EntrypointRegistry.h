/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#pragma once

#include <cassert>
#include <string>
#include <unordered_map>

#include <brayns/network/entrypoint/EntrypointRef.h>
#include <brayns/network/socket/ClientRegistry.h>

#include <brayns/pluginapi/PluginAPI.h>

namespace brayns
{
class EntrypointRegistry
{
public:
    EntrypointRegistry(PluginAPI& api, ClientRegistry& clients)
        : _api(&api)
        , _clients(&clients)
    {
    }

    const EntrypointRef* find(const std::string& name) const
    {
        auto i = _entrypoints.find(name);
        return i == _entrypoints.end() ? nullptr : &i->second;
    }

    void add(EntrypointRef entrypoint)
    {
        entrypoint.setup(*_api, *_clients);
        auto& name = entrypoint.getName();
        assert(!name.empty());
        assert(_entrypoints.find(name) == _entrypoints.end());
        _entrypoints.emplace(name, std::move(entrypoint));
    }

private:
    PluginAPI* _api;
    ClientRegistry* _clients;
    std::unordered_map<std::string, EntrypointRef> _entrypoints;
};
} // namespace brayns
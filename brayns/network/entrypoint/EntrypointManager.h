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

#include <string>
#include <unordered_map>

#include <brayns/network/entrypoint/EntrypointRef.h>

namespace brayns
{
class NetworkContext;

using EntrypointMap = std::unordered_map<std::string, EntrypointRef>;

class EntrypointManager
{
public:
    EntrypointManager(NetworkContext& context);

    const EntrypointRef* find(const std::string& name) const;
    void add(EntrypointRef entrypoint);
    void setup();
    void update() const;
    void processRequest(const NetworkRequest& request) const;

private:
    NetworkContext* _context;
    EntrypointMap _entrypoints;
};
} // namespace brayns
/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include "Endpoint.h"

namespace brayns
{
EndpointRegistry::EndpointRegistry(std::map<std::string, Endpoint> endpoints):
    _endpoints(std::move(endpoints))
{
}

std::vector<std::string> EndpointRegistry::getMethods() const
{
    auto result = std::vector<std::string>();
    result.reserve(_endpoints.size());

    for (const auto &[method, endpoint] : _endpoints)
    {
        result.push_back(method);
    }

    return result;
}

const Endpoint *EndpointRegistry::find(const std::string &method) const
{
    auto i = _endpoints.find(method);
    return i == _endpoints.end() ? nullptr : &i->second;
}
}

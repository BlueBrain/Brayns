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

#include "BinaryManager.h"

#include <cassert>

#include <brayns/common/Log.h>

namespace brayns
{
void BinaryManager::add(ClientRequest request)
{
    assert(request.isBinary());
    Log::debug("Binary request buffered {}.", request);
    _requests.push_back(std::move(request));
}

std::optional<ClientRequest> BinaryManager::poll()
{
    if (_requests.empty())
    {
        return std::nullopt;
    }
    auto request = std::move(_requests.front());
    _requests.pop_front();
    return request;
}

void BinaryManager::flush()
{
    for (const auto &request : _requests)
    {
        Log::info("Discard unused binary request {}.", request);
    }
    _requests.clear();
}
} // namespace brayns

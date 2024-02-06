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

#pragma once

#include <brayns/network/jsonrpc/JsonRpcRequest.h>

#include "EntrypointRegistry.h"

namespace brayns
{
/**
 * @brief Helper class to match a request with its entrypoint and validate it.
 *
 */
class EntrypointFinder
{
public:
    /**
     * @brief Find entrypoint corresponding to request.
     *
     * Check also that the request is valid based on the entrypoint schema.
     *
     * @param request JSON RPC client request.
     * @param entrypoints Entrypoint registry.
     * @return const EntrypointRef & Entrypoint to process request.
     * @throw JsonRpcException No entrypoint found or invalid params.
     */
    static EntrypointRef &find(const JsonRpcRequest &request, EntrypointRegistry &entrypoints);
};
} // namespace brayns

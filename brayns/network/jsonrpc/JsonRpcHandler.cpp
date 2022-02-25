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

#include "JsonRpcHandler.h"

#include <brayns/common/Log.h>

#include <brayns/network/jsonrpc/JsonRpcException.h>

namespace brayns
{
void JsonRpcHandler::handle(const JsonRpcRequest &request, const EntrypointRef &entrypoint)
{
    Log::info("Execution of request {} from entrypoint.", request);
    try
    {
        entrypoint.onRequest(request);
        Log::info("Request {} executed with success.", request);
    }
    catch (const JsonRpcException &e)
    {
        Log::info("Request {} execution error: {}.", request, e);
        request.error(e);
    }
    catch (const std::exception &e)
    {
        Log::error("Unexpected entrypoint error during execution of request {}: '{}'.", request, e.what());
        request.error(InternalErrorException(e.what()));
    }
    catch (...)
    {
        Log::error("Unknown entrypoint error during execution of request {}.", request);
        request.error(InternalErrorException("Unknown error"));
    }
}
} // namespace brayns

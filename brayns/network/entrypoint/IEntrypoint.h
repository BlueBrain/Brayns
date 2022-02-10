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

#pragma once

#include <string>

#include <brayns/json/JsonSchema.h>

#include <brayns/network/jsonrpc/JsonRpcRequest.h>

namespace brayns
{
/**
 * @brief IEntrypoint interface.
 *
 */
class IEntrypoint
{
public:
    virtual ~IEntrypoint() = default;

    /**
     * @brief Return the name of the entrypoint (unique ID).
     *
     * @return std::string Name of the entrypoint.
     */
    virtual std::string getName() const = 0;

    /**
     * @brief Return a description of the entrypoint.
     *
     * @return std::string Description of the entrypoint.
     */
    virtual std::string getDescription() const = 0;

    /**
     * @brief Return the schema of the entrypoint request (params field).
     *
     * @return JsonSchema Schema of the entrypoint request.
     */
    virtual JsonSchema getParamsSchema() const = 0;

    /**
     * @brief Return the schema of the entrypoint reply (result field).
     *
     * @return JsonSchema Schema of the entrypoint reply.
     */
    virtual JsonSchema getResultSchema() const = 0;

    /**
     * @brief Called each time the client sends a request to the entrypoint.
     *
     * @param request Client request.
     */
    virtual void onRequest(const JsonRpcRequest &request) = 0;

    /**
     * @brief Return true if the entrypoint takes a long time and must be
     * executed in a separated thread.
     *
     * @return true The entrypoint will start a task in a separated thread.
     * @return false The entrypoint will be executed in the main loop.
     */
    virtual bool isAsync() const
    {
        return false;
    }

    /**
     * @brief Called once the entrypoint is ready to be used.
     *
     */
    virtual void onCreate()
    {
    }

    /**
     * @brief Called at each update of Brayns.
     *
     */
    virtual void onUpdate()
    {
    }

    /**
     * @brief Called before each render.
     *
     */
    virtual void onPreRender()
    {
    }

    /**
     * @brief Called after each render.
     *
     */
    virtual void onPostRender()
    {
    }
};
} // namespace brayns

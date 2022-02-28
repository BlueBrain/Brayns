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

#include <memory>
#include <string>

#include <brayns/network/messages/SchemaMessage.h>

#include "IEntrypoint.h"

namespace brayns
{
/**
 * @brief Entrypoint wrapper.
 *
 */
class EntrypointRef
{
public:
    /**
     * @brief Construct a ref on an entrypoint implementing IEntrypoint.
     *
     * @param entrypoint Entrypoint implementation.
     */
    EntrypointRef(std::string plugin, std::unique_ptr<IEntrypoint> entrypoint);

    /**
     * @brief Call implementation onCreate() and build JSON schema.
     *
     */
    void onCreate();

    /**
     * @brief Call implementation onRequest() with given request.
     *
     * @param request Client request to the underlying entrypoint.
     * @throw JsonRpcException Errors that must be replied.
     */
    void onRequest(const JsonRpcRequest &request) const;

    /**
     * @brief Call implementation onPreRender().
     *
     */
    void onPreRender() const;

    /**
     * @brief Call implementation onPostRender().
     *
     */
    void onPostRender() const;

    /**
     * @brief Call implementation onCancel().
     *
     */
    void onCancel() const;

    /**
     * @brief Call implementation onConnect(client).
     *
     * @param client Client ref.
     */
    void onConnect(const ClientRef &client) const;

    /**
     * @brief Call implementation onDisconnect(client).
     *
     * @param client Client ref.
     */
    void onDisconnect(const ClientRef &client) const;

    /**
     * @brief Get the name of the plugin that registered the entrypoint.
     *
     * @return const std::string& Parent plugin name.
     */
    const std::string &getPlugin() const;

    /**
     * @brief Get the entrypoint schema.
     *
     * Must be called after onCreate().
     *
     * @return const SchemaResult& Entrypoint schema.
     */
    const SchemaResult &getSchema() const;

    /**
     * @brief Get the method the entrypoint performs.
     *
     * Must be called after onCreate().
     *
     * @return const std::string& Entrypoint name.
     */
    const std::string &getMethod() const;

    /**
     * @brief Get the description of the entrypoint.
     *
     * Must be called after onCreate().
     *
     * @return const std::string& Entrypoint description.
     */
    const std::string &getDescription() const;

    /**
     * @brief Get the JSON schema of the params of the entrypoint.
     *
     * Must be called after onCreate().
     *
     * @return const std::vector<JsonSchema>& Vector with JSON schema as first
     * component or empty if no schema.
     */
    const std::vector<JsonSchema> &getParamsSchema() const;

    /**
     * @brief Get the JSON schema of the entrypoint result.
     *
     * Must be called after onCreate().
     *
     * @return const JsonSchema& JSON schema.
     */
    const JsonSchema &getResultSchema() const;

    /**
     * @brief Check if the request can be cancelled.
     *
     * @return true Can be cancelled.
     * @return false Cannot be cancelled.
     */
    bool isAsync() const;

    /**
     * @brief Check if the entrypoint has priority over the others in queue.
     *
     * @return true Higher priority.
     * @return false Normal priority.
     */
    bool hasPriority() const;

private:
    std::unique_ptr<IEntrypoint> _entrypoint;
    SchemaResult _schema;
};
} // namespace brayns

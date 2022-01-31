/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
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
#include <type_traits>

#include <brayns/network/messages/SchemaMessage.h>

#include "IEntrypoint.h"

namespace brayns
{
/**
 * @brief Entrypoint holder of an implementation of IEntrypoint.
 *
 */
class EntrypointRef
{
public:
    /**
     * @brief Shortcut to instanciate a ref on an entrypoint of given type.
     *
     * @tparam T Entrypoint type.
     * @tparam Args Entrypoint constructor arg types.
     * @param args Entrypoint constructor args.
     * @return EntrypointRef Entrypoint.
     */
    template<typename T, typename... Args>
    static EntrypointRef create(Args &&...args)
    {
        static_assert(std::is_base_of<IEntrypoint, T>());
        return EntrypointRef(std::make_unique<T>(std::forward<Args>(args)...));
    }

    /**
     * @brief Construct a ref on an entrypoint implementing IEntrypoint.
     *
     * @param entrypoint Entrypoint implementation.
     */
    EntrypointRef(std::unique_ptr<IEntrypoint> entrypoint);

    /**
     * @brief Setup entrypoint with given context.
     *
     * Give underlying entrypoint context access, call onCreate() and build JSON
     * schema using implementation.
     *
     * @param context Common data to all entrypoints (API, connections).
     */
    void setup(NetworkContext &context);

    /**
     * @brief Call implementation onUpdate().
     *
     */
    void update() const;

    /**
     * @brief Call implementation onRequest with given request.
     *
     * @param request Client request to the underlying entrypoint.
     */
    void processRequest(const NetworkRequest &request) const;

    /**
     * @brief Call implementation onPreRender.
     *
     */
    void preRender() const;

    /**
     * @brief Call implementation onPostRender.
     *
     */
    void postRender() const;

    /**
     * @brief Retrieve entrypoint name from implementation.
     *
     * @return std::string Entrypoint name (ex: "get-camera").
     */
    std::string loadName() const;

    /**
     * @brief Get the name of the plugin the entrypoint belongs to.
     *
     * @return const std::string& Parent plugin name.
     */
    const std::string &getPlugin() const;

    /**
     * @brief Set the name of the plugin the entrypoint belongs to.
     *
     * @param plugin Parent plugin name.
     */
    void setPlugin(const std::string &plugin);

    /**
     * @brief Get the entrypoint schema.
     *
     * Must be called after setup.
     *
     * @return const SchemaResult& Entrypoint schema.
     */
    const SchemaResult &getSchema() const;

    /**
     * @brief Get the name of the entrypoint cached at setup.
     *
     * Must be called after setup.
     *
     * @return const std::string& Entrypoint name.
     */
    const std::string &getName() const;

    /**
     * @brief Get the description of the entrypoint cached at setup.
     *
     * Must be called after setup.
     *
     * @return const std::string& Entrypoint description.
     */
    const std::string &getDescription() const;

    /**
     * @brief Get the JSON schema of the params of the entrypoint.
     *
     * Must be called after setup.
     *
     * @return const std::vector<JsonSchema>& Vector with JSON schema as first
     * component or empty if no schema.
     */
    const std::vector<JsonSchema> &getParamsSchema() const;

    /**
     * @brief Get the JSON schema of the entrypoint result.
     *
     * Must be called after setup.
     *
     * @return const JsonSchema& JSON schema.
     */
    const JsonSchema &getResultSchema() const;

    /**
     * @brief Check if the entrypoint is asynchronous.
     *
     * @return true Async.
     * @return false Sync.
     */
    bool isAsync() const;

private:
    std::unique_ptr<IEntrypoint> _entrypoint;
    SchemaResult _schema;
};
} // namespace brayns

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

#include <brayns/json/JsonType.h>

#include <brayns/network/context/NetworkContext.h>

#include "IEntrypoint.h"

namespace brayns
{
/**
 * @brief Base entrypoint providing basic functionalities.
 *
 */
class BaseEntrypoint : public IEntrypoint
{
public:
    /**
     * @brief Get the Network context assigned to the entrypoint.
     *
     * @return NetworkContext& Network context.
     */
    NetworkContext &getContext() const;

    /**
     * @brief Shortcut to get a reference to Brayns API.
     *
     * @return PluginAPI& Brayns API access.
     */
    PluginAPI &getApi() const;

    /**
     * @brief Shortcut to get the Entrypoint list.
     *
     * @return EntrypointManager& Entrypoint manager.
     */
    EntrypointManager &getEntrypoints() const;

    /**
     * @brief Shortcut to get the connection list.
     *
     * @return ConnectionManager& Client connection manager.
     */
    ConnectionManager &getConnections() const;

    /**
     * @brief Shortcut to get the stream manager.
     *
     * @return StreamManager& Stream manager.
     */
    StreamManager &getStream() const;

    /**
     * @brief Shortcut to get the task manager holding all asynchronous tasks
     * currently running.
     *
     * @return NetworkTaskManager& Task manager.
     */
    NetworkTaskManager &getTasks() const;

    /**
     * @brief Shortcut to get the binary request manager (raw model uploader).
     *
     * @return BinaryRequestManager& Binary request manager.
     */
    BinaryRequestManager &getBinary() const;

    /**
     * @brief Get the stored plugin name.
     *
     * @return const std::string& Parent plugin name.
     */
    virtual const std::string &getPlugin() const override;

    /**
     * @brief Store the plugin name.
     *
     * @param plugin Parent plugin name.
     */
    virtual void setPlugin(const std::string &plugin) override;

    /**
     * @brief Store the network context reference inside instance.
     *
     * @param context A reference to the network context.
     */
    virtual void setContext(NetworkContext &context) override;

    /**
     * @brief Shortcut to trigger the rendering of a new frame.
     *
     */
    void triggerRender() const;

    /**
     * @brief Broadcast a notification message to all connected clients.
     *
     * @param params Message to send ("params" field).
     */
    void notify(const JsonValue &params) const;

private:
    std::string _plugin;
    NetworkContext *_context = nullptr;
};
} // namespace brayns

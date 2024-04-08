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

#include <brayns/core/network/client/ClientRef.h>
#include <brayns/core/network/jsonrpc/RequestId.h>

namespace brayns
{
/**
 * @brief Interface for a JSON-RPC or binary request processing.
 *
 */
class ITask
{
public:
    virtual ~ITask() = default;

    /**
     * @brief Get the client who sent the request.
     *
     * @return const ClientRef& Client ref.
     */
    virtual const ClientRef &getClient() const = 0;

    /**
     * @brief Get the request ID of the task.
     *
     * @return const RequestId& JSON-RPC request ID (empty if binary).
     */
    virtual const RequestId &getId() const = 0;

    /**
     * @brief Get the method of the task.
     *
     * @return const std::string& JSON-RPC method.
     */
    virtual const std::string &getMethod() const = 0;

    /**
     * @brief Check if the task has priority.
     *
     * Tasks with priority are executed directly while others are queued.
     *
     * @return true Higher priority.
     * @return false Normal priority.
     */
    virtual bool hasPriority() const = 0;

    /**
     * @brief Execute the task.
     *
     */
    virtual void run() = 0;

    /**
     * @brief Cancel the task or throw if not cancellable.
     *
     * @throw TaskNotCancellableException Task is not cancellable.
     */
    virtual void cancel() = 0;

    /**
     * @brief Notify the task if its client disconnects.
     *
     */
    virtual void disconnect() = 0;
};
} // namespace brayns

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

#include <brayns/network/binary/BinaryManager.h>
#include <brayns/network/client/ClientRequest.h>
#include <brayns/network/entrypoint/EntrypointRegistry.h>
#include <brayns/network/task/TaskManager.h>

namespace brayns
{
/**
 * @brief Helper class to process a request.
 *
 */
class RequestDispatcher
{
public:
    /**
     * @brief Construct a dispatcher with dependencies.
     *
     * @param binary Binary manager to buffer binary requests.
     * @param entrypoints Available entrypoints to process request.
     * @param tasks Task queue if the request processing must be delayed.
     */
    RequestDispatcher(BinaryManager &binary, const EntrypointRegistry &entrypoints, TaskManager &tasks);

    /**
     * @brief Dispatch client request to entrypoints or create a task.
     *
     * @param request Raw client request.
     */
    void dispatch(ClientRequest request);

private:
    BinaryManager &_binary;
    const EntrypointRegistry &_entrypoints;
    TaskManager &_tasks;
};
} // namespace brayns

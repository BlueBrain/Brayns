/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include "BaseEntrypoint.h"
#include "EntrypointRequest.h"
#include "EntrypointTask.h"

namespace brayns
{
/**
 * @brief Helper class to handle requests params and result with template.
 *
 * @tparam ParamsType
 * @tparam ResultType
 *
 * Example:
 * @code {.cpp}
 * // MyEntrypoint.h
 * class MyEntrypoint : public Entrypoint<MyParams, MyResult>
 * {
 * public:
 *      virtual void onRequest(const Request& request) override
 *      {
 *          auto params = request.getParams();
 *          // Do stuff
 *          request.reply(MyResult());
 *      }
 * };
 *
 * MyPlugin.cpp
 * auto interface = _api->getActionInterface();
 * if (interface)
 * {
 *      interface->add<MyEntrypoint>();
 * }
 * @endcode
 *
 */
template <typename ParamsType, typename ResultType>
class Entrypoint : public BaseEntrypoint
{
public:
    using Params = ParamsType;
    using Result = ResultType;
    using Request = EntrypointRequest<ParamsType, ResultType>;
    using Task = EntrypointTask<ParamsType, ResultType>;
    using TaskPtr = std::shared_ptr<Task>;

    /**
     * @brief Build JsonSchema using ParamsType.
     *
     * @return JsonSchema Schema of the entrypoint request.
     */
    virtual JsonSchema getParamsSchema() const override
    {
        return Json::getSchema<ParamsType>();
    }

    /**
     * @brief Build JsonSchema using ResultType.
     *
     * @return JsonSchema Schema of the entrypoint reply.
     */
    virtual JsonSchema getResultSchema() const override
    {
        return Json::getSchema<ResultType>();
    }

    /**
     * @brief Create a specialized request for Result and Params and forward it
     * to onRequest(const Request&).
     *
     * @param request Generic request.
     */
    virtual void onRequest(const NetworkRequest& request) override
    {
        onRequest(Request(request));
    }

    /**
     * @brief Shortcut to process the request with the message already parsed.
     *
     * @param request Request specialized for Params and Result.
     */
    virtual void onRequest(const Request& request) = 0;

    /**
     * @brief Launch a task to process the given request.
     *
     * This method will throw if a task is already running with the same client
     * and ID.
     *
     * @param task Corresponding EntrypointTask on which execute(request) will
     * be called.
     * @param request Client request to process.
     * @throw EntrypointException A request with the same client and ID is
     * already running.
     */
    void launchTask(const TaskPtr& task, const Request& request) const
    {
        auto& tasks = getTasks();
        auto& handle = request.getConnectionHandle();
        auto& id = request.getId();
        if (!tasks.addIfNotPresent(handle, id, task))
        {
            throw EntrypointException("A task with ID " + id.getDisplayText() +
                                      " is already running for this client");
        }
        task->execute(request);
    }

    /**
     * @brief Launch or restart a task to process the given request.
     *
     * This method will cancel and restart the task if any exists with
     * the same client and ID.
     *
     * @param task Corresponding EntrypointTask on which execute(request) will
     * be called.
     * @param request Client request to process.
     */
    void launchOrRestartTask(const TaskPtr& task, const Request& request) const
    {
        auto& tasks = getTasks();
        auto& handle = request.getConnectionHandle();
        auto& id = request.getId();
        tasks.addIfNotPresent(handle, id, task);
        task->execute(request);
    }
};
} // namespace brayns
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

#include "Task.h"

#include <cassert>

#include <fmt/format.h>
#include "Api.h"

namespace
{
using namespace brayns;

const TaskInterface &getRawTask(const std::map<TaskId, TaskInterface> &tasks, TaskId id)
{
    auto i = tasks.find(id);

    if (i == tasks.end())
    {
        throw InvalidParams(fmt::format("Task ID not found: {}", id));
    }

    return i->second;
}

TaskId addTask(TaskInterface task, std::map<TaskId, TaskInterface> &tasks, IdGenerator<TaskId> &ids)
{
    auto id = ids.next();
    assert(!tasks.contains(id));

    try
    {
        tasks[id] = std::move(task);
    }
    catch (...)
    {
        ids.recycle(id);
        throw;
    }

    return id;
}
}

namespace brayns
{
Api::Api(std::map<std::string, Endpoint> endpoints):
    _endpoints(std::move(endpoints))
{
}

Api::~Api()
{
    for (const auto &[id, task] : _tasks)
    {
        task.cancel();
    }
}

std::vector<std::string> Api::getMethods() const
{
    auto result = std::vector<std::string>();
    result.reserve(_endpoints.size());

    for (const auto &[method, endpoint] : _endpoints)
    {
        result.push_back(method);
    }

    return result;
}

const EndpointSchema &Api::getSchema(const std::string &method) const
{
    auto i = _endpoints.find(method);

    if (i == _endpoints.end())
    {
        throw InvalidParams(fmt::format("Invalid endpoint method: '{}'", method));
    }

    return i->second.schema;
}

Payload Api::execute(const std::string &method, Payload params)
{
    auto i = _endpoints.find(method);

    if (i == _endpoints.end())
    {
        throw MethodNotFound(method);
    }

    const auto &endpoint = i->second;

    auto errors = validate(params.json, endpoint.schema.params);

    if (!errors.empty())
    {
        throw InvalidParams("Invalid params schema", errors);
    }

    if (endpoint.schema.async)
    {
        const auto &handler = std::get<AsyncEndpointHandler>(endpoint.handler);

        auto task = handler(std::move(params));

        auto id = addTask(std::move(task), _tasks, _ids);

        return {serializeToJson(TaskResult{id})};
    }

    const auto &handler = std::get<SyncEndpointHandler>(endpoint.handler);

    return handler(std::move(params));
}

std::vector<TaskInfo> Api::getTasks() const
{
    auto infos = std::vector<TaskInfo>();
    infos.reserve(_tasks.size());

    for (const auto &[id, task] : _tasks)
    {
        auto operationCount = task.operationCount;
        auto currentOperation = task.getCurrentOperation();

        infos.push_back({id, operationCount, std::move(currentOperation)});
    }

    return infos;
}

TaskInfo Api::getTask(TaskId id) const
{
    const auto &task = getRawTask(_tasks, id);

    auto operationCount = task.operationCount;
    auto currentOperation = task.getCurrentOperation();

    return {id, operationCount, std::move(currentOperation)};
}

Payload Api::waitForTaskResult(TaskId id)
{
    const auto &task = getRawTask(_tasks, id);

    auto result = task.wait();

    _tasks.erase(id);
    _ids.recycle(id);

    return result;
}

void Api::cancelTask(TaskId id)
{
    const auto &task = getRawTask(_tasks, id);

    task.cancel();

    _tasks.erase(id);
    _ids.recycle(id);
}
}

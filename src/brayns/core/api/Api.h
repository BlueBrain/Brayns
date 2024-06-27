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

#include <map>

#include <brayns/core/utils/IdGenerator.h>

#include "Endpoint.h"
#include "Task.h"

namespace brayns
{
class Api
{
public:
    explicit Api(std::map<std::string, Endpoint> endpoints);
    ~Api();

    Api(const Api &) = delete;
    Api(Api &&) = default;
    Api &operator=(const Api &) = delete;
    Api &operator=(Api &&) = default;

    std::vector<std::string> getMethods() const;
    const EndpointSchema &getSchema(const std::string &method) const;
    RawResult execute(const std::string &method, RawParams params);
    std::vector<TaskInfo> getTasks() const;
    ProgressInfo getTaskProgress(TaskId id) const;
    RawResult waitForTaskResult(TaskId id);
    void cancelTask(TaskId id);

private:
    std::map<std::string, Endpoint> _endpoints;
    std::map<TaskId, RawTask> _tasks;
    IdGenerator<TaskId> _ids;
};
}

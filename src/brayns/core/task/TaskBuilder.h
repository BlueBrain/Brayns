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

#include <concepts>
#include <functional>
#include <future>
#include <string>
#include <vector>

#include <brayns/core/api/ApiBuilder.h>

#include "Task.h"

namespace brayns::experimental
{
template<typename ResultType>
struct Task
{
    std::function<ProgressInfo()> getProgress;
    std::function<ResultType()> wait;
    std::function<void()> cancel;
};

template<typename ResultType>
Task<ResultType> startTask(std::invocable<Progress> auto task)
{
    auto state = std::make_shared<ProgressState>();
    auto progress = Progress(*state);

    auto future = std::async(std::launch::async, std::move(task), progress);
    auto shared = std::make_shared<decltype(future)>(std::move(future));

    return {
        .getProgress = [=] { return state->get(); },
        .wait = [=] { return shared->get(); },
        .cancel = [=] { state->cancel(); },
    };
}

template<typename T>
struct TaskReflector;

template<typename T>
struct TaskReflector<Task<T>>
{
    using Result = T;
};

template<typename T>
using GetTaskResultType = typename TaskReflector<T>::Result;

template<typename FactoryType>
TaskSchema reflectTaskSchema(std::string name)
{
    using ParamsReflector = ApiReflector<GetParamsType<FactoryType>>;
    using ResultReflector = ApiReflector<GetTaskResultType<GetResultType<FactoryType>>>;

    return {
        .name = std::move(name),
        .params = ParamsReflector::getSchema(),
        .result = ResultReflector::getSchema(),
    };
}

template<typename FactoryType>
auto addTaskFactoryParsing(FactoryType factory)
{
    using ParamsReflector = ApiReflector<GetParamsType<FactoryType>>;
    using ResultReflector = ApiReflector<GetTaskResultType<GetResultType<FactoryType>>>;

    return [factory = std::move(factory)](auto rawParams)
    {
        auto params = ParamsReflector::deserialize(std::move(rawParams));
        auto task = factory(std::move(params));
        auto waitAndSerialize = [wait = std::move(task.wait)] { return ResultReflector::serialize(wait()); };

        return RawTask{
            .getProgress = std::move(task.getProgress),
            .wait = std::move(waitAndSerialize),
            .cancel = std::move(task.cancel),
        };
    };
}

class TaskDefinitionBuilder
{
public:
    explicit TaskDefinitionBuilder(TaskDefinition &definition):
        _definition(&definition)
    {
    }

    TaskDefinitionBuilder description(std::string value)
    {
        _definition->schema.description = std::move(value);
        return *this;
    }

private:
    TaskDefinition *_definition;
};

class TaskApiBuilder
{
public:
    TaskManager build();

    TaskDefinitionBuilder task(std::string name, auto factory)
    {
        auto wrapper = ensureHasParams(std::move(factory));
        auto schema = reflectTaskSchema<decltype(wrapper)>(std::move(name));
        auto start = addTaskFactoryParsing(std::move(wrapper));
        _definitions.push_back({std::move(schema), std::move(start)});
        return TaskDefinitionBuilder(_definitions.back());
    }

private:
    std::vector<TaskDefinition> _definitions;
};
}

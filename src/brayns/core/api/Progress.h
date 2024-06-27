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

#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>

#include <brayns/core/json/Json.h>
#include <brayns/core/jsonrpc/Errors.h>

namespace brayns
{
struct ProgressInfo
{
    std::string currentOperation = "Task startup";
    float currentOperationProgress = 0.0F;
};

template<>
struct JsonReflector<ProgressInfo>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<ProgressInfo>();
        builder.field("current_operation", [](auto &object) { return &object.currentOperation; })
            .description("Description of the current operation");
        builder.field("current_operation_progress", [](auto &object) { return &object.currentOperationProgress; })
            .description("Progress of the current operation between 0 and 1");
        return builder.build();
    }
};

class TaskCancelledException : public JsonRpcException
{
public:
    explicit TaskCancelledException();
};

class ProgressState
{
public:
    ProgressInfo get();
    void update(float currentOperationProgress);
    void nextOperation(std::string value);
    void cancel();

private:
    std::mutex _mutex;
    ProgressInfo _info;
    bool _cancelled = false;
};

class Progress
{
public:
    explicit Progress(std::shared_ptr<ProgressState> state);

    void update(float currentOperationProgress);
    void nextOperation(std::string value);

private:
    std::shared_ptr<ProgressState> _state;
};
}

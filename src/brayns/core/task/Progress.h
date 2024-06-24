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

#include <brayns/core/jsonv2/Json.h>

namespace brayns::experimental
{
struct ProgressInfo
{
    std::string operationName;
    float currentOperationProgress = 0.0F;
};

template<>
struct JsonReflector<ProgressInfo>
{
    static auto reflect()
    {
        auto builder = JsonObjectInfoBuilder<ProgressInfo>();
        builder.field("operation_name", [](auto &object) { return &object.operationName; })
            .description("Description of the current operation");
        builder.field("current_operation_progress", [](auto &object) { return &object.currentOperationProgress; })
            .description("Progress of the current operation between 0 and 1");
        return builder.build();
    }
};

class TaskCancelledException : public std::runtime_error
{
public:
    using runtime_error::runtime_error;
};

class ProgressState
{
public:
    ProgressInfo get();
    void update(float currentOperationProgress);
    void nextOperation(std::string name);
    void cancel();

private:
    std::mutex _mutex;
    ProgressInfo _info;
    bool _cancelled = false;
};

class Progress
{
public:
    explicit Progress(ProgressState &state);

    void update(float currentOperationProgress);
    void nextOperation(std::string name);

private:
    ProgressState *_state;
};
}

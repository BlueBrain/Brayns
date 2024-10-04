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

#include <functional>

#include <brayns/core/json/Json.h>
#include <brayns/core/jsonrpc/Messages.h>

#include "Progress.h"

namespace brayns
{
struct Task
{
    std::size_t operationCount;
    std::function<TaskOperation()> getCurrentOperation;
    std::function<Payload()> wait;
    std::function<void()> cancel;
};

struct TaskInfo
{
    std::size_t operationCount;
    TaskOperation currentOperation;
};

template<>
struct JsonObjectReflector<TaskInfo>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<TaskInfo>();
        builder.field("operationCount", [](auto &object) { return &object.operationCount; }).description("Number of operations the task will perform");
        builder.field("currentOperation", [](auto &object) { return &object.currentOperation; }).description("Current task operation");
        return builder.build();
    }
};
}

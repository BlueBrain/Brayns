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

#include "Progress.h"

#include <cassert>

namespace brayns
{
TaskCancelledException::TaskCancelledException():
    JsonRpcException(0, "Task has been cancelled")
{
}

TaskMonitor::TaskMonitor(std::size_t operationCount):
    _operationCount(operationCount)
{
}

std::size_t TaskMonitor::getOperationCount() const
{
    return _operationCount;
}

TaskOperation TaskMonitor::getCurrentOperation()
{
    auto lock = std::lock_guard(_mutex);

    return _currentOperation;
}

void TaskMonitor::update(float completion)
{
    auto lock = std::lock_guard(_mutex);

    if (_cancelled)
    {
        throw TaskCancelledException();
    }

    assert(completion >= 0.0F && completion <= 1.0F);

    _currentOperation.completion = completion;
}

void TaskMonitor::nextOperation(std::string description)
{
    auto lock = std::lock_guard(_mutex);

    if (_cancelled)
    {
        throw TaskCancelledException();
    }

    _currentOperation.description = std::move(description);
    _currentOperation.completion = 0.0F;
    _currentOperation.index += 1;

    assert(_currentOperation.index < _operationCount);
}

void TaskMonitor::cancel()
{
    auto lock = std::lock_guard(_mutex);

    assert(!_cancelled);

    _cancelled = true;
}

Progress::Progress(std::shared_ptr<TaskMonitor> monitor):
    _monitor(std::move(monitor))
{
}

void Progress::update(float completion)
{
    _monitor->update(completion);
}

void Progress::nextOperation(std::string description)
{
    _monitor->nextOperation(std::move(description));
}
}

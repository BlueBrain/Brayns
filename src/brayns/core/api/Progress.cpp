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

TaskOperation Progress::getCurrentOperation()
{
    auto lock = std::lock_guard(_mutex);

    return _currentOperation;
}

void Progress::update(float completion)
{
    auto lock = std::lock_guard(_mutex);

    checkCancellation();

    assert(completion >= 0.0F && completion <= 1.0F);
    _currentOperation.completion = completion;
}

void Progress::nextOperation(std::string description)
{
    auto lock = std::lock_guard(_mutex);

    checkCancellation();

    _currentOperation.description = std::move(description);
    _currentOperation.completion = 0.0F;
}

void Progress::cancel()
{
    auto lock = std::lock_guard(_mutex);

    assert(!_cancelled);
    _cancelled = true;
}

void Progress::checkCancellation()
{
    if (_cancelled)
    {
        throw TaskCancelledException();
    }
}
}

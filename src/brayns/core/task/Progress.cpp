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

namespace brayns::experimental
{
ProgressInfo ProgressState::get()
{
    auto lock = std::lock_guard(_mutex);

    return _info;
}

void ProgressState::update(float currentOperationProgress)
{
    auto lock = std::lock_guard(_mutex);

    if (_cancelled)
    {
        throw TaskCancelledException("Cancelled in update");
    }

    assert(currentOperationProgress >= 0.0F && currentOperationProgress <= 1.0F);
    _info.currentOperationProgress = currentOperationProgress;
}

void ProgressState::nextOperation(std::string name)
{
    auto lock = std::lock_guard(_mutex);

    if (_cancelled)
    {
        throw TaskCancelledException("Cancelled in next operation");
    }

    _info.operationName = std::move(name);
    _info.currentOperationProgress = 0.0F;
}

void ProgressState::cancel()
{
    auto lock = std::lock_guard(_mutex);

    assert(!_cancelled);
    _cancelled = true;
}

Progress::Progress(ProgressState &state):
    _state(&state)
{
}

void Progress::update(float currentOperationProgress)
{
    _state->update(currentOperationProgress);
}

void Progress::nextOperation(std::string name)
{
    _state->nextOperation(std::move(name));
}
}

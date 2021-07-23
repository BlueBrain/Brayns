/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include <algorithm>
#include <cassert>
#include <memory>
#include <vector>

#include "NetworkTask.h"

namespace brayns
{
class NetworkTaskManager
{
public:
    void add(NetworkTaskPtr task)
    {
        assert(task);
        _tasks.push_back(std::move(task));
    }

    void poll()
    {
        auto first = _tasks.begin();
        auto last = _tasks.end();
        auto predictor = [](auto& task)
        {
            task->poll();
            return !task->isRunning();
        };
        auto from = std::remove_if(first, last, predictor);
        _tasks.erase(from, last);
    }

private:
    std::vector<NetworkTaskPtr> _tasks;
};
} // namespace brayns
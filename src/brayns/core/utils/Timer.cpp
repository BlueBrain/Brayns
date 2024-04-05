/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "Timer.h"

#include <chrono>

namespace
{
template<typename Rep>
int64_t getDuration(const std::chrono::high_resolution_clock::time_point &start)
{
    const auto end = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::duration<int64_t, Rep>>(end - start);
    return duration.count();
}
}

namespace brayns
{
Timer::Timer():
    _startTime(clock::now())
{
}

void Timer::reset() noexcept
{
    _startTime = clock::now();
}

int64_t Timer::seconds() const noexcept
{
    return getDuration<std::ratio<1, 1>>(_startTime);
}
} // namespace brayns

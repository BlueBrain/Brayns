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

#pragma once

#include <chrono>

namespace brayns
{
/**
 * @brief The Timer class is a simpler timer class that uses the standard high resolution clock
 * to measure time between 2 time points.
 *
 * It is initialized when constructed, and resetted by calling the reset() method.
 * After that, every call to micros(), millis() or seconds() will return the time, in such
 * time unit, since the last call to reset() up to that point.
 *
 * THIS CLASS IS NOT THREAD SAFE
 */
class Timer
{
public:
    /**
     * @brief Timer constructs the timer, setting the start time point to the current time
     */
    Timer();

    /**
     * @brief reset resets the start time point to the current time
     */
    void reset() noexcept;

    /**
     * @brief seconds returns the time passed, in seconds, since the start time point and now (rounded down)
     * @return seconds
     */
    int64_t seconds() const noexcept;

private:
    using clock = std::chrono::high_resolution_clock;

    clock::time_point _startTime;
};
} // namespace brayns

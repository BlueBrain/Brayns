/* Copyright 2015-2024 Blue Brain Project/EPFL
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
using clock = std::chrono::high_resolution_clock;

/** Simple timer class to measure time spent in a portion of the code */
class Timer
{
public:
    Timer();

    /** (Re)Start the timer at 'now' */
    void start();

    /** Stops the timer and records the interval + a smoothed value over time*/
    void stop();

    /** @return the elapsed time in seconds since the last start(). */
    double elapsed() const;

    /** @return last interval from start() to stop() in microseconds. */
    int64_t microseconds() const;

    /** @return last interval from start() to stop() in milliseconds. */
    int64_t milliseconds() const;

    /** @return last interval from start() to stop() in seconds. */
    double seconds() const;

    /**
     * @return last interval from start() to stop() in per seconds, e.g. for
     * frame per seconds
     */
    double perSecond() const;

    /**
     * @return the current FPS, updated every 150 ms
     */
    double fps() const;

    /**
     * @return last smoothed interval from start() to stop() in per seconds,
     * e.g. for frame per seconds
     */
    double perSecondSmoothed() const;

private:
    clock::time_point _startTime;
    int64_t _microseconds{0};
    double _smoothNom{0.0};
    double _smoothDen{0.0};
    const double _smoothingFactor{0.9}; // closer to 1 means more smoothing
    clock::time_point _lastFPSTickTime;
    double _fps{0.0};
};
} // namespace brayns

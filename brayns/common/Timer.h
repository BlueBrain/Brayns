/* Copyright (c) 2018, EPFL/Blue Brain Project
 *
 * Responsible Author: Daniel.Nachbaur@epfl.ch
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
/** Simple timer class to measure time spent in a portion of the code */
class Timer
{
public:
    Timer() = default;

    /** (Re)Start the timer at 'now' */
    void start() { _startTime = clock::now(); }
    /** @return the elapsed time in seconds since the last start(). */
    double elapsed() const
    {
        return std::chrono::duration<double>{clock::now() - _startTime}.count();
    }

    /** Stops the timer and records the interval + a smoothed value over time*/
    void stop()
    {
        _milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
                            clock::now() - _startTime)
                            .count();
        _smoothNom = _smoothNom * _smoothingFactor + _milliseconds / 1000.0;
        _smoothDen = _smoothDen * _smoothingFactor + 1.f;
    }

    /** @return last interval from start() to stop() in milliseconds. */
    int64_t milliseconds() const { return _milliseconds; }
    /** @return last interval from start() to stop() in seconds. */
    double seconds() const { return _milliseconds / 1000.0; }
    /**
     * @return last interval from start() to stop() in per seconds, e.g. for
     * frame per seconds
     */
    double perSecond() const { return 1000.0 / _milliseconds; }
    /**
     * @return last smoothed interval from start() to stop() in per seconds,
     * e.g. for frame per seconds
     */
    double perSecondSmoothed() const { return _smoothDen / _smoothNom; }
private:
    using clock = std::chrono::high_resolution_clock;
    clock::time_point _startTime;
    int64_t _milliseconds{0};
    double _smoothNom{0.0};
    double _smoothDen{0.0};
    const double _smoothingFactor{0.9}; // closer to 1 means more smoothing
};
}

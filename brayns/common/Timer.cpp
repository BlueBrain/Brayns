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

#include "Timer.h"

#include <chrono>
constexpr double MICRO_PER_SEC = 1000000.0;
constexpr double FPS_UPDATE_MILLISECS = 150;

namespace brayns
{
Timer::Timer()
{
    _lastFPSTickTime = clock::now();
    _startTime = clock::now();
}

void Timer::start()
{
    _startTime = clock::now();
}
double Timer::elapsed() const
{
    return std::chrono::duration<double>{clock::now() - _startTime}.count();
}

void Timer::stop()
{
    const auto now = clock::now();
    _microseconds =
        std::chrono::duration_cast<std::chrono::microseconds>(now - _startTime)
            .count();
    _smoothNom = _smoothNom * _smoothingFactor + _microseconds / MICRO_PER_SEC;
    _smoothDen = _smoothDen * _smoothingFactor + 1.f;

    const auto secsLastFPSTick =
        std::chrono::duration_cast<std::chrono::milliseconds>(now -
                                                              _lastFPSTickTime)
            .count();

    if (secsLastFPSTick >= FPS_UPDATE_MILLISECS)
    {
        _lastFPSTickTime = now;
        _fps = perSecond();
    }
}

int64_t Timer::microseconds() const
{
    return _microseconds;
}

int64_t Timer::milliseconds() const
{
    return _microseconds / 1000.0;
}

double Timer::seconds() const
{
    return _microseconds / MICRO_PER_SEC;
}

double Timer::perSecond() const
{
    return MICRO_PER_SEC / _microseconds;
}

double Timer::perSecondSmoothed() const
{
    return _smoothDen / _smoothNom;
}

double Timer::fps() const
{
    return _fps;
}
} // namespace brayns

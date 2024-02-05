/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <chrono>

namespace brayns
{
/**
 * @brief Helper class to call a functor with a max rate.
 *
 */
class RateLimiter
{
public:
    /**
     * @brief Clock used to measure the rate.
     *
     */
    using Clock = std::chrono::high_resolution_clock;

    /**
     * @brief Clock timepoint used to measure the rate.
     *
     */
    using TimePoint = Clock::time_point;

    /**
     * @brief Duration unit used to measure the rate.
     *
     */
    using Duration = Clock::duration;

    /**
     * @brief Build a rate limiter using FPS specification.
     *
     * @param fps Frames (calls) per second expected.
     * @return RateLimiter Matching rate limiter.
     */
    static RateLimiter fromFps(size_t fps)
    {
        if (fps == 0)
        {
            return {Duration::max()};
        }
        auto period = std::chrono::duration<double>(1.0 / fps);
        return std::chrono::duration_cast<Duration>(period);
    }

    /**
     * @brief Construct a rate limiter with min period to call functor.
     *
     * @param period Min duration between two functor calls.
     */
    RateLimiter(Duration period = Duration(0))
        : _period(period)
    {
    }

    /**
     * @brief Call the functor if the given period is ellapsed.
     *
     * @tparam FunctorType Functor type.
     * @param functor Functor instance to call.
     * @return true Functor has been called.
     * @return false Too soon to call functor.
     */
    template <typename FunctorType>
    bool call(FunctorType functor)
    {
        auto currentTime = Clock::now();
        auto elapsed = currentTime - _lastCall;
        if (elapsed < _period)
        {
            return false;
        }
        auto delay = (elapsed - _period) % _period;
        _lastCall = currentTime - delay;
        functor();
        return true;
    }

private:
    Duration _period;
    TimePoint _lastCall;
};
} // namespace brayns
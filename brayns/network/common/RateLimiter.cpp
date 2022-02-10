/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include "RateLimiter.h"

namespace brayns
{
RateLimiter RateLimiter::fromRate(size_t rate)
{
    RateLimiter limiter;
    limiter.setRate(rate);
    return limiter;
}

RateLimiter::RateLimiter(Duration period)
    : _period(period)
{
}

void RateLimiter::setPeriod(Duration period)
{
    _period = period;
}

void RateLimiter::setRate(size_t rate)
{
    if (rate == 0)
    {
        _period = Duration::max();
        return;
    }
    auto period = std::chrono::duration<double>(1.0 / rate);
    _period = std::chrono::duration_cast<Duration>(period);
}

bool RateLimiter::_tryUpdateLastCall()
{
    auto currentTime = Clock::now();
    auto elapsed = currentTime - _lastCall;
    if (elapsed < _period)
    {
        return false;
    }
    auto delay = (elapsed - _period) % _period;
    _lastCall = currentTime - delay;
    return true;
}
} // namespace brayns

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

#pragma once

#include "Clock.h"

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
     * @brief Build a rate limiter using a rate specification.
     *
     * @param rate Max calls per second.
     * @return RateLimiter Matching rate limiter.
     */
    static RateLimiter fromRate(size_t rate);

    /**
     * @brief Construct a rate limiter using a period specification.
     *
     * Limits nothing by default if no period is specified.
     *
     * @param period Min duration between two calls.
     */
    explicit RateLimiter(Duration period = Duration(0));

    /**
     * @brief Update period but not last call.
     *
     * @param period Min duration between two calls.
     */
    void setPeriod(Duration period);

    /**
     * @brief Update period from rate.
     *
     * @param rate Max calls per second.
     */
    void setRate(size_t rate);

    /**
     * @brief Call the functor if the given period is ellapsed.
     *
     * @tparam FunctorType Functor type.
     * @param functor Functor instance to call.
     * @return true Functor has been called.
     * @return false Too soon to call functor.
     */
    template<typename FunctorType>
    bool call(FunctorType functor)
    {
        if (!_tryUpdateLastCall())
        {
            return false;
        }
        functor();
        return true;
    }

private:
    bool _tryUpdateLastCall();

    Duration _period;
    TimePoint _lastCall;
};
} // namespace brayns

/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#endif
#include <async++.h>
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#include <string>

namespace brayns
{
/**
 * A base class for functors that can be used for Tasks. It provides support for
 * progress reporting and cancellation during execution.
 */
class TaskFunctor
{
public:
    /** message, increment, amount */
    using ProgressFunc = std::function<void(std::string, float, float)>;

    /** Set the function when progress() is called. */
    void setProgressFunc(const ProgressFunc& progressFunc)
    {
        _progressFunc = progressFunc;
    }

    /**
     * Report progress using the provided callback from setProgressFunc() and
     * also check if the execution has been cancelled.
     *
     * @param message the progress message
     * @param increment the fractional increment of this progress update
     * @param amount the absolute amount of progress at the time of this update
     */
    void progress(const std::string& message, const float increment,
                  const float amount)
    {
        cancelCheck();
        if (_progressFunc)
            _progressFunc(message, increment, amount);
    }

    /** Set the cancel token from e.g. the task that uses this functor. */
    void setCancelToken(async::cancellation_token& cancelToken)
    {
        _cancelToken = &cancelToken;
    }

    /**
     * Checks if the execution has been cancelled. If so, this will throw an
     * exception that is ultimately handled by the task and is stored in the
     * tasks' result.
     */
    void cancelCheck() const
    {
        if (_cancelToken)
            async::interruption_point(*_cancelToken);
    }

private:
    async::cancellation_token* _cancelToken{nullptr};
    ProgressFunc _progressFunc;
};
} // namespace brayns

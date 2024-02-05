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
#include <condition_variable>
#include <mutex>

namespace brayns
{
/**
 * @brief Wrapper around a condition variable to monitor a task.
 *
 */
class NetworkTaskMonitor
{
public:
    /**
     * @brief Blocks until notified.
     *
     */
    void wait()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _monitor.wait(lock);
    }

    /**
     * @brief Notify the monitor and unlock all thread waiting on it.
     *
     */
    void notify() { _monitor.notify_all(); }

    /**
     * @brief Wait for notification or throw if timeout is reached.
     *
     * @tparam R Representation type.
     * @tparam P Period ratio.
     * @param timeout Timeout duration.
     * @throw std::runtime_error Timeout is reached with no notifications.
     */
    template <typename R, typename P>
    void wait(std::chrono::duration<R, P> timeout)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        auto status = _monitor.wait_for(lock, timeout);
        if (status == std::cv_status::timeout)
        {
            throw std::runtime_error("Task monitor timeout");
        }
    }

    /**
     * @brief Wait for notification with time limit.
     *
     * @tparam R Representation type.
     * @tparam P Period type.
     * @param duration Max duration to wait for notifications.
     * @return true No notifications has been received, duration waited.
     * @return false Cancelled by a notification.
     */
    template <typename R, typename P>
    bool waitFor(std::chrono::duration<R, P> duration)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        auto status = _monitor.wait_for(lock, duration);
        return status == std::cv_status::timeout;
    }

private:
    std::mutex _mutex;
    std::condition_variable _monitor;
};
} // namespace brayns
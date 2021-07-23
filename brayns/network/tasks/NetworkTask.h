/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include <atomic>
#include <chrono>
#include <future>
#include <stdexcept>

namespace brayns
{
class NetworkTask;

using NetworkTaskPtr = std::unique_ptr<NetworkTask>;

class NetworkTask
{
public:
    virtual ~NetworkTask()
    {
        cancel();
        wait();
    }

    bool isReady() const { return _hasStatus(std::future_status::ready); }

    bool isRunning() const { return _hasStatus(std::future_status::timeout); }

    bool isCancelled() const { return _cancelled; }

    void start()
    {
        cancel();
        wait();
        onStart();
        _cancelled = false;
        _result = std::async(std::launch::async, [this] { run(); });
    }

    void poll()
    {
        if (!isReady())
        {
            return;
        }
        wait();
    }

    void wait()
    {
        if (!_result.valid())
        {
            return;
        }
        try
        {
            _result.get();
            _throwIfCancelled();
            onComplete();
        }
        catch (...)
        {
            onError(std::current_exception());
        }
    }

    void cancel()
    {
        if (_cancelled || !isRunning())
        {
            return;
        }
        onCancel();
        _cancelled = true;
    }

    void progress(const std::string& operation, double amount)
    {
        _throwIfCancelled();
        onProgress(operation, amount);
    }

    virtual void run() = 0;

    virtual void onStart() {}

    virtual void onComplete() {}

    virtual void onCancel() {}

    virtual void onError(std::exception_ptr) {}

    virtual void onProgress(const std::string&, double) {}

private:
    bool _hasStatus(std::future_status status) const
    {
        if (!_result.valid())
        {
            return false;
        }
        return _result.wait_for(std::chrono::seconds(0)) == status;
    }

    void _throwIfCancelled() const
    {
        if (!_cancelled)
        {
            return;
        }
        throw std::runtime_error("Task cancelled");
    }

    std::future<void> _result;
    std::atomic_bool _cancelled{false};
};
} // namespace brayns
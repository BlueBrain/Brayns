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
#include <memory>
#include <mutex>
#include <stdexcept>

namespace brayns
{
class NetworkTaskException
{
public:
    void rethrow()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (!_e)
        {
            _e = std::make_exception_ptr(std::runtime_error("Task cancelled"));
        }
        std::rethrow_exception(_e);
    }

    NetworkTaskException& operator=(std::exception_ptr e)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _e = std::move(e);
        return *this;
    }

    template <typename T>
    NetworkTaskException& operator=(T e)
    {
        *this = std::make_exception_ptr(std::move(e));
        return *this;
    }

private:
    std::mutex _mutex;
    std::exception_ptr _e;
};

class NetworkTask
{
public:
    virtual ~NetworkTask() = default;

    bool isReady() const { return _hasStatus(std::future_status::ready); }

    bool isRunning() const { return _hasStatus(std::future_status::timeout); }

    bool isCancelled() const { return _cancelled; }

    bool isCancellable() const { return isRunning() && !isCancelled(); }

    void start()
    {
        cancelAndWait();
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
        _fetchResult();
    }

    void wait()
    {
        if (!_result.valid())
        {
            return;
        }
        _fetchResult();
    }

    void cancel()
    {
        if (!isCancellable())
        {
            return;
        }
        onCancel();
        _cancelled = true;
    }

    void setException(std::exception_ptr e) { _e = e; }

    void cancelWith(std::exception_ptr e)
    {
        _e = e;
        cancel();
    }

    void cancelAndWait()
    {
        cancel();
        wait();
    }

    void checkCancelled()
    {
        if (!_cancelled)
        {
            return;
        }
        _e.rethrow();
    }

    void progress(const std::string& operation, double amount)
    {
        checkCancelled();
        onProgress(operation, amount);
    }

    virtual void run() = 0;

    virtual void onStart() {}

    virtual void onComplete() {}

    virtual void onCancel() {}

    virtual void onError(std::exception_ptr) {}

    virtual void onProgress(const std::string&, double) {}

    virtual void onDisconnect() {}

    template <typename T>
    void setException(T e)
    {
        _e = std::move(e);
    }

    template <typename T>
    void cancelWith(T e)
    {
        _e = std::move(e);
        cancel();
    }

private:
    bool _hasStatus(std::future_status status) const
    {
        if (!_result.valid())
        {
            return false;
        }
        return _result.wait_for(std::chrono::seconds(0)) == status;
    }

    void _fetchResult()
    {
        try
        {
            _result.get();
            checkCancelled();
            onComplete();
        }
        catch (...)
        {
            onError(std::current_exception());
        }
    }

    std::future<void> _result;
    std::atomic_bool _cancelled{false};
    NetworkTaskException _e;
};

using NetworkTaskPtr = std::shared_ptr<NetworkTask>;
} // namespace brayns
/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
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

#include "NetworkTask.h"

namespace brayns
{
void NetworkTaskException::rethrow()
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (!_e)
    {
        _e = std::make_exception_ptr(std::runtime_error("Task cancelled"));
    }
    std::rethrow_exception(_e);
}

NetworkTaskException &NetworkTaskException::operator=(std::exception_ptr e)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _e = std::move(e);
    return *this;
}

bool NetworkTask::isReady() const
{
    return _hasStatus(std::future_status::ready);
}

bool NetworkTask::isRunning() const
{
    return _hasStatus(std::future_status::timeout);
}

bool NetworkTask::isCancelled() const
{
    return _cancelled;
}

bool NetworkTask::isCancellable() const
{
    return isRunning() && !isCancelled();
}

void NetworkTask::start()
{
    cancelAndWait();
    onStart();
    _cancelled = false;
    _result = std::async(std::launch::async, [this] { run(); });
}

void NetworkTask::poll()
{
    if (!isReady())
    {
        return;
    }
    _fetchResult();
}

void NetworkTask::wait()
{
    if (!_result.valid())
    {
        return;
    }
    _fetchResult();
}

void NetworkTask::cancel()
{
    if (!isCancellable())
    {
        return;
    }
    onCancel();
    _cancelled = true;
}

void NetworkTask::setException(std::exception_ptr e)
{
    _e = e;
}

void NetworkTask::cancelWith(std::exception_ptr e)
{
    _e = e;
    cancel();
}

void NetworkTask::cancelAndWait()
{
    cancel();
    wait();
}

void NetworkTask::checkCancelled()
{
    if (!_cancelled)
    {
        return;
    }
    _e.rethrow();
}

void NetworkTask::progress(const std::string &operation, double amount)
{
    checkCancelled();
    onProgress(operation, amount);
}

void NetworkTask::onStart()
{
}

void NetworkTask::onComplete()
{
}

void NetworkTask::onCancel()
{
}

void NetworkTask::onError(std::exception_ptr)
{
}

void NetworkTask::onProgress(const std::string &, double)
{
}

void NetworkTask::onDisconnect()
{
}

bool NetworkTask::_hasStatus(std::future_status status) const
{
    if (!_result.valid())
    {
        return false;
    }
    return _result.wait_for(std::chrono::seconds(0)) == status;
}

void NetworkTask::_fetchResult()
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
} // namespace brayns

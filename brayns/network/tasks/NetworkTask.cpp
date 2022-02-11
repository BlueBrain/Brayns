/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

namespace
{
class NetworkTaskStatus
{
public:
    static bool isReady(const std::future<void> &future)
    {
        return hasStatus(future, std::future_status::ready);
    }

    static bool isRunning(const std::future<void> &future)
    {
        return hasStatus(future, std::future_status::timeout);
    }

    static bool hasStatus(const std::future<void> &future, std::future_status status)
    {
        if (!future.valid())
        {
            return false;
        }
        return future.wait_for(std::chrono::seconds(0)) == status;
    }
};
} // namespace

namespace brayns
{
TaskCancelledException::TaskCancelledException()
    : JsonRpcException(-32000, "Task cancelled")
{
}

void NetworkTask::start()
{
    cancelAndWait();
    _cancelled = false;
    onStart();
    _future = std::async(std::launch::async, [this] { run(); });
}

bool NetworkTask::poll()
{
    if (!NetworkTaskStatus::isReady(_future))
    {
        return false;
    }
    _fetchResult();
    return true;
}

void NetworkTask::wait()
{
    if (!_future.valid())
    {
        return;
    }
    _fetchResult();
}

void NetworkTask::cancel()
{
    if (!NetworkTaskStatus::isRunning(_future) || _cancelled)
    {
        return;
    }
    onCancel();
    _cancelled = true;
}

void NetworkTask::cancelAndWait()
{
    cancel();
    wait();
}

void NetworkTask::disconnect()
{
    onDisconnect();
}

void NetworkTask::progress(const std::string &operation, double amount)
{
    _throwIfCancelled();
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

void NetworkTask::onError(const JsonRpcException &e)
{
    (void)e;
}

void NetworkTask::onProgress(const std::string &operation, double amount)
{
    (void)operation;
    (void)amount;
}

void NetworkTask::onDisconnect()
{
}

void NetworkTask::_throwIfCancelled()
{
    if (_cancelled)
    {
        throw TaskCancelledException();
    }
}

void NetworkTask::_fetchResult()
{
    try
    {
        _future.get();
        _throwIfCancelled();
        onComplete();
    }
    catch (const JsonRpcException &e)
    {
        onError(e);
    }
    catch (const std::exception &e)
    {
        onError(InternalErrorException(e.what()));
    }
    catch (...)
    {
        onError(InternalErrorException("Unknown error"));
    }
}
} // namespace brayns

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

#pragma once

#include <atomic>
#include <future>
#include <stdexcept>

#include <brayns/network/entrypoint/EntrypointException.h>

namespace brayns
{
/**
 * @brief Exception thrown when a task is cancelled.
 *
 */
class TaskCancelledException : public EntrypointException
{
public:
    TaskCancelledException();
};

/**
 * @brief Base class to run a task in a parallel thread.
 *
 * Provides cancellation and polling support to monitor the task from the main
 * thread.
 *
 * The task functionality must be implemented by overridding the run() method.
 *
 * Only the content of the run() method will be executed in a separated thread.
 *
 */
class NetworkTask
{
public:
    virtual ~NetworkTask() = default;

    /**
     * @brief Start the task by calling run() in a separated thread.
     *
     */
    void start();

    /**
     * @brief Check if the task is ready and fetch result if yes.
     *
     * @return true Task is complete, the result has been fetched.
     * @return false Task is not ready yet, no result available.
     */
    bool poll();

    /**
     * @brief Retreive task result or wait for it if not ready.
     *
     */
    void wait();

    /**
     * @brief Cancel the task if cancellable.
     *
     */
    void cancel();

    /**
     * @brief Cancel the task and wait for its termination.
     *
     */
    void cancelAndWait();

    /**
     * @brief Called when the client who started the task disconnects.
     *
     */
    void disconnect();

protected:
    /**
     * @brief Check cancellation and call onProgress() implementation.
     *
     * @param operation Current operation description.
     * @param amount Percentage progress 0-1.
     */
    void progress(const std::string &operation, double amount);

    /**
     * @brief Implementation of the task that will run in a separated thread.
     *
     */
    virtual void run() = 0;

    /**
     * @brief Called in start().
     *
     */
    virtual void onStart();

    /**
     * @brief Called in poll() if the task is complete.
     *
     * A task is complete if no exceptions have been thrown in run().
     */
    virtual void onComplete();

    /**
     * @brief Called in cancel().
     *
     */
    virtual void onCancel();

    /**
     * @brief Called in poll() if the task fails.
     *
     * A task fails if an exception has been thrown in run().
     *
     * @param e Opaque exception pointer.
     */
    virtual void onError(std::exception_ptr e);

    /**
     * @brief Called in progress().
     *
     * @param operation Current operation description.
     * @param amount Percentage progress 0-1.
     */
    virtual void onProgress(const std::string &operation, double amount);

    /**
     * @brief Called if the client starting the task is disconnected.
     *
     */
    virtual void onDisconnect();

private:
    void _throwIfCancelled();
    void _fetchResult();

    std::future<void> _future;
    std::atomic_bool _cancelled{false};
};
} // namespace brayns

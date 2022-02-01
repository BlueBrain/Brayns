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
#include <memory>
#include <mutex>
#include <stdexcept>

namespace brayns
{
/**
 * @brief Synchronized wrapper around an arbitrary exception.
 *
 * Can be used to set an exception to a task from the task thread or the main
 * thread.
 *
 */
class NetworkTaskException
{
public:
    /**
     * @brief Rethrow the contained exception.
     *
     * Throw std::runtime_error with generic cancellation message if not set.
     *
     */
    void rethrow();

    /**
     * @brief Assign an arbitrary exception.
     *
     * @param e Opaque exception pointer.
     * @return NetworkTaskException& *this.
     */
    NetworkTaskException &operator=(std::exception_ptr e);

    /**
     * @brief Assign an arbitrary exception.
     *
     * @tparam T Exception type.
     * @param e Exception instance.
     * @return NetworkTaskException& *this
     */
    template<typename T>
    NetworkTaskException &operator=(T e)
    {
        *this = std::make_exception_ptr(std::move(e));
        return *this;
    }

private:
    std::mutex _mutex;
    std::exception_ptr _e;
};

/**
 * @brief Base class to run a network request in a parallel thread.
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
     * @brief Check wether the task result is ready to be retreived.
     *
     * @return true Task is done and a the result has not been retreived yet.
     * @return false Task is running or the result has already been retreived.
     */
    bool isReady() const;

    /**
     * @brief Check if the task is running.
     *
     * @return true Task is running.
     * @return false Task is done or not started.
     */
    bool isRunning() const;

    /**
     * @brief Check if the task has been cancelled.
     *
     * @return true Cancel was called between start() and onComplete().
     * @return false Task was not cancelled or has been restarted.
     */
    bool isCancelled() const;

    /**
     * @brief Check if the task can be cancelled.
     *
     * Cancelling a task is meaningful if the task is running and not already
     * cancelled.
     *
     * @return true Task cancellation is meaningful.
     * @return false Task cannot be cancelled.
     */
    bool isCancellable() const;

    /**
     * @brief Start the task by calling run() in a separated thread.
     *
     */
    void start();

    /**
     * @brief Retreive task result if this one is ready to be fetched.
     *
     */
    void poll();

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
     * @brief Assign an exception to the task (thread safe).
     *
     * The exception will be passed to the onError(exception) method on the next
     * poll() from the main thread.
     *
     * @param e Opaque exception pointer.
     */
    void setException(std::exception_ptr e);

    /**
     * @brief Shortcut to assign an exception with cancellation.
     *
     * @param e  Opaque exception pointer.
     */
    void cancelWith(std::exception_ptr e);

    /**
     * @brief Cancel the task and wait for its termination.
     *
     */
    void cancelAndWait();

    /**
     * @brief Throw the current exception set if the task is cancelled.
     *
     */
    void checkCancelled();

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
     * @brief Custom logic to run in the main thread when a task is started.
     *
     */
    virtual void onStart();

    /**
     * @brief Custom logic to run in the main thread when a task succeed.
     *
     */
    virtual void onComplete();

    /**
     * @brief Custom logic to run in the main thread just before cancellation.
     *
     */
    virtual void onCancel();

    /**
     * @brief Custom logic to run in the main thread when an exception is set.
     *
     */
    virtual void onError(std::exception_ptr);

    /**
     * @brief Custom logic to run when progress() is called.
     *
     * @param operation Current operation description.
     * @param amount Percentage progress 0-1.
     */
    virtual void onProgress(const std::string &, double);

    /**
     * @brief Called when the task client is disconnected.
     *
     */
    virtual void onDisconnect();

    /**
     * @brief Assign an exception to the task.
     *
     * @tparam T Exception type.
     * @param e Exception instance.
     */
    template<typename T>
    void setException(T e)
    {
        _e = std::move(e);
    }

    /**
     * @brief Cancel the task with the given exception.
     *
     * @tparam T Exception type.
     * @param e Exception instance.
     */
    template<typename T>
    void cancelWith(T e)
    {
        _e = std::move(e);
        cancel();
    }

private:
    bool _hasStatus(std::future_status status) const;
    void _fetchResult();

    std::future<void> _result;
    std::atomic_bool _cancelled{false};
    NetworkTaskException _e;
};

using NetworkTaskPtr = std::shared_ptr<NetworkTask>;
} // namespace brayns

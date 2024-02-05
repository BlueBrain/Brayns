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

#include <brayns/common/Progress.h>
#include <brayns/common/tasks/TaskFunctor.h>

namespace brayns
{
/**
 * A task is an operation that can be scheduled (directly, async, ...) and has
 * support for progress reporting during the execution and cancellation of the
 * execution.
 */
class AbstractTask
{
public:
    virtual ~AbstractTask() = default;

    /**
     * Cancels the task if is either waiting to be scheduled or already running.
     * Will have no effect if the task already finished.
     *
     * @param done for asynchronous cancel processing, this function will be
     *             remembered and can be called via finishCancel()
     */
    void cancel(std::function<void()> done = {})
    {
        if (_cancelled)
            return;
        _cancelDone = done;
        _cancelled = true;
        _cancelToken.cancel();
        _cancel();
    }

    /**
     * Indicate that cancel processing has finished and call the function given
     * to cancel().
     */
    void finishCancel()
    {
        if (_cancelDone)
            _cancelDone();
    }

    /** @return true if the task has been cancelled. */
    bool canceled() const { return _cancelled; }
    /**
     * Schedule the execution of the task. Depending on the concrete task type,
     * the task could still be running though after construction.
     */
    virtual void schedule() = 0;

    /** @return access to the progress of task. */
    Progress progress{"Scheduling task ..."};

protected:
    async::cancellation_token _cancelToken;
    std::function<void()> _cancelDone;
    std::atomic_bool _cancelled{false};

private:
    virtual void _cancel() {}
};

/**
 * A task type which is directly scheduled after creation. Its result after
 * successful execution is of type T.
 *
 * If the functor is of type TaskFunctor, it will be provided with cancel
 * support and progress feedback possibility.
 */
template <typename T>
class Task : public AbstractTask
{
public:
    using Type = async::task<T>;

    /**
     * Create an empty task; use task() and async++ to do something meaningful.
     */
    Task() = default;

    /** Create and schedule a task with the given functor or lambda. */
    template <typename F>
    Task(F&& functor)
    {
        _task = async::spawn(_setupFunctor(std::move(functor)));
    }

    /** NOP for this task; tasks are running after construction. */
    void schedule() override
    { /* task is already running after construction */
    }

    /**
     * @return the result of tasks, or an exception in case of errors or
     *         cancellation.
     */
    T result() { return _task.get(); }
    /** @return access to the async++ task for chaining, assignment, etc. */
    auto& get() { return _task; }

protected:
    Type _task;

    template <typename F>
    auto&& _setupFunctor(F&& functor)
    {
        if (std::is_base_of<TaskFunctor, F>::value)
        {
            auto& taskFunctor = static_cast<TaskFunctor&>(functor);
            taskFunctor.setProgressFunc(
                std::bind(&Progress::update, std::ref(progress),
                          std::placeholders::_1, std::placeholders::_3));
            taskFunctor.setCancelToken(_cancelToken);
        }
        return std::move(functor);
    }
};

/**
 * A task type which allows for deferred scheduling after construction using
 * schedule().
 */
template <typename T>
class DeferredTask : public Task<T>
{
public:
    template <typename F>
    DeferredTask(F&& functor)
    {
        Task<T>::_task = _e.get_task().then(
            Task<T>::template _setupFunctor(std::move(functor)));
    }

    void schedule() final { _e.set(); }

private:
    async::event_task<void> _e;
};
} // namespace brayns

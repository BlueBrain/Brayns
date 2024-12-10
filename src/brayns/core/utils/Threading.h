/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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
#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>

namespace brayns
{
template<typename T>
class Queue
{
public:
    void push(T item)
    {
        auto lock = std::lock_guard(_mutex);

        _items.push_back(std::move(item));
        _condition.notify_all();
    }

    T wait()
    {
        auto lock = std::unique_lock(_mutex);

        if (_items.empty())
        {
            _condition.wait(lock, [this] { return !_items.empty(); });
        }

        auto item = std::move(_items.front());
        _items.pop_front();

        return item;
    }

private:
    std::mutex _mutex;
    std::condition_variable _condition;
    std::deque<T> _items;
};

class WorkerRunner
{
public:
    void stop()
    {
        _running = false;
        submit([] {});
    }

    void submit(std::function<void()> task)
    {
        _tasks.push(std::move(task));
    }

    void run()
    {
        while (true)
        {
            auto task = _tasks.wait();

            if (!_running)
            {
                return;
            }

            task();
        }
    }

private:
    std::atomic_bool _running = true;
    Queue<std::function<void()>> _tasks;
};

class Worker
{
public:
    explicit Worker(std::unique_ptr<WorkerRunner> runner, std::jthread thread):
        _runner(std::move(runner)),
        _thread(std::move(thread))
    {
    }

    ~Worker()
    {
        if (_runner)
        {
            _runner->stop();
        }
    }

    Worker(const Worker &) = delete;
    Worker(Worker &&) = default;
    Worker &operator=(const Worker &) = delete;
    Worker &operator=(Worker &&) = default;

    void submit(std::function<void()> task)
    {
        _runner->submit(std::move(task));
    }

private:
    std::unique_ptr<WorkerRunner> _runner;
    std::jthread _thread;
};

inline Worker startWorker()
{
    auto runner = std::make_unique<WorkerRunner>();
    auto thread = std::jthread([&runner = *runner] { runner.run(); });
    return Worker(std::move(runner), std::move(thread));
}
}

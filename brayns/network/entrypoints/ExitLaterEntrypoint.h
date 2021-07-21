/* Copyright (c) 2021 EPFL/Blue Brain Project
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
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

#include <brayns/network/entrypoint/Entrypoint.h>
#include <brayns/network/messages/ExitLaterMessage.h>

namespace brayns
{
class ExitLaterTask
{
public:
    ~ExitLaterTask() { cancel(); }

    void start(std::chrono::minutes duration)
    {
        cancel();
        _complete = false;
        _cancelled = false;
        _duration = duration;
        _thread = std::thread([this] { _run(); });
    }

    void cancel()
    {
        if (!_thread.joinable())
        {
            return;
        }
        _cancelled = true;
        _monitor.notify_all();
        _thread.join();
    }

    bool isComplete() const { return _complete; }

private:
    void _run()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _monitor.wait_for(lock, _duration);
        if (!_cancelled)
        {
            _complete = true;
        }
    }

    std::chrono::minutes _duration;
    std::thread _thread;
    std::mutex _mutex;
    std::condition_variable _monitor;
    std::atomic_bool _complete{false};
    std::atomic_bool _cancelled{false};
};

class ExitLaterEntrypoint : public Entrypoint<ExitLaterMessage, EmptyMessage>
{
public:
    virtual std::string getName() const override { return "exit-later"; }

    virtual std::string getDescription() const override
    {
        return "Schedules Brayns to shutdown after a given amount of minutes";
    }

    virtual bool isAsync() const override { return true; }

    virtual void onUpdate() override
    {
        if (!_task.isComplete())
        {
            return;
        }
        auto& engine = getApi().getEngine();
        engine.setKeepRunning(false);
        engine.triggerRender();
    }

    virtual void onRequest(const Request& request) override
    {
        auto& params = request.getParams();
        auto duration = std::chrono::minutes(params.minutes);
        if (duration.count())
        {
            _task.start(duration);
        }
        request.reply(EmptyMessage());
    }

private:
    ExitLaterTask _task;
};
} // namespace brayns
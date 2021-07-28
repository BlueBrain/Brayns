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
#include <brayns/network/entrypoint/EntrypointTask.h>
#include <brayns/network/messages/ExitLaterMessage.h>

namespace brayns
{
class ExitLaterTask : public EntrypointTask<ExitLaterMessage, EmptyMessage>
{
public:
    ExitLaterTask(Engine& engine)
        : _engine(&engine)
    {
    }

    virtual void run() override
    {
        std::unique_lock<std::mutex> lock(_mutex);
        auto duration = getParams().minutes;
        _monitor.wait_for(lock, std::chrono::minutes(duration));
    }

    virtual void onStart() override { reply(EmptyMessage()); }

    virtual void onComplete() override
    {
        _engine->setKeepRunning(false);
        _engine->triggerRender();
    }

    virtual void onError(std::exception_ptr) override {}

    virtual void onCancel() override { _monitor.notify_all(); }

private:
    Engine* _engine;
    std::mutex _mutex;
    std::condition_variable _monitor;
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

    virtual void onCreate() override
    {
        auto& engine = getApi().getEngine();
        _task = std::make_shared<ExitLaterTask>(engine);
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        if (params.minutes == 0)
        {
            return;
        }
        launchOrRestartTask(_task, request);
    }

private:
    std::shared_ptr<ExitLaterTask> _task;
};
} // namespace brayns
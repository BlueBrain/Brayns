/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <brayns/network/entrypoint/Entrypoint.h>
#include <brayns/network/messages/ExitLaterMessage.h>
#include <brayns/network/tasks/NetworkTask.h>
#include <brayns/network/tasks/NetworkTaskMonitor.h>

namespace brayns
{
class ExitLaterTask : public NetworkTask
{
public:
    ExitLaterTask(Engine& engine)
        : _engine(&engine)
    {
    }

    virtual ~ExitLaterTask() { _monitor.notify(); }

    void execute(uint32_t minutes)
    {
        cancelAndWait();
        _duration = std::chrono::minutes(minutes);
        start();
    }

    virtual void run() override { _monitor.waitFor(_duration); }

    virtual void onComplete() override
    {
        _engine->setKeepRunning(false);
        _engine->triggerRender();
    }

    virtual void onCancel() override { _monitor.notify(); }

private:
    std::chrono::minutes _duration;
    Engine* _engine;
    NetworkTaskMonitor _monitor;
};

class ExitLaterEntrypoint : public Entrypoint<ExitLaterMessage, EmptyMessage>
{
public:
    virtual std::string getName() const override { return "exit-later"; }

    virtual std::string getDescription() const override
    {
        return "Schedules Brayns to shutdown after a given amount of minutes";
    }

    virtual void onCreate() override
    {
        auto& engine = getApi().getEngine();
        _task = std::make_shared<ExitLaterTask>(engine);
    }

    virtual void onUpdate() override { _task->poll(); }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        auto& minutes = params.minutes;
        _task->execute(minutes);
        request.reply(EmptyMessage());
    }

private:
    std::shared_ptr<ExitLaterTask> _task;
};
} // namespace brayns
/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include "ExitLaterEntrypoint.h"

namespace brayns
{
ExitLaterTask::ExitLaterTask(Engine &engine)
    : _engine(engine)
{
}

ExitLaterTask::~ExitLaterTask()
{
    _monitor.notify();
}

void ExitLaterTask::quitAfter(std::chrono::minutes duration)
{
    cancelAndWait();
    _duration = duration;
    start();
}

void ExitLaterTask::run()
{
    _monitor.wait(_duration);
}

void ExitLaterTask::onComplete()
{
    _engine.setKeepRunning(false);
    _engine.triggerRender();
}

void ExitLaterTask::onCancel()
{
    _monitor.notify();
}

ExitLaterEntrypoint::ExitLaterEntrypoint(Engine &engine)
    : _task(engine)
{
}

std::string ExitLaterEntrypoint::getName() const
{
    return "exit-later";
}

std::string ExitLaterEntrypoint::getDescription() const
{
    return "Schedules Brayns to shutdown after a given amount of minutes";
}

void ExitLaterEntrypoint::onUpdate()
{
    _task.poll();
}

void ExitLaterEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    auto duration = std::chrono::minutes(params.minutes);
    _task.quitAfter(duration);
    request.reply(EmptyMessage());
}
} // namespace brayns

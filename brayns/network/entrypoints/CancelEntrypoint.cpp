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

#include "CancelEntrypoint.h"

namespace brayns
{
CancelEntrypoint::CancelEntrypoint(TaskManager &tasks):
    _tasks(tasks)
{
}

std::string CancelEntrypoint::getMethod() const
{
    return "cancel";
}

std::string CancelEntrypoint::getDescription() const
{
    return "Cancel the task started by the request with the given ID";
}

bool CancelEntrypoint::hasPriority() const
{
    return true;
}

void CancelEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    auto &id = params.id;
    auto &client = request.getClient();
    _tasks.cancel(client, id);
    request.reply(EmptyJson());
}
} // namespace brayns

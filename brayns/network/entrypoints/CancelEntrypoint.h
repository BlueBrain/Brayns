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

#include <brayns/network/entrypoint/Entrypoint.h>

namespace brayns
{
class CancelEntrypoint : public Entrypoint<CancelParams, EmptyMessage>
{
public:
    virtual std::string getName() const override { return "cancel"; }

    virtual std::string getDescription() const override
    {
        return "Cancel the request with the ID given in params";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        auto& id = params.id;
        auto& handle = request.getConnectionHandle();
        auto& tasks = getTasks();
        if (!tasks.cancel(handle, id))
        {
            throw EntrypointException("No task with ID '" + id +
                                      "' is running for this client");
        }
        request.reply(EmptyMessage());
    }
};
} // namespace brayns
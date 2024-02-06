/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include <brayns/network/entrypoint/Entrypoint.h>

class MockEntrypoint : public brayns::Entrypoint<int, int>
{
public:
    MockEntrypoint(std::string method = "test", int reply = 0, bool priority = false):
        _method(std::move(method)),
        _reply(reply),
        _priority(priority)
    {
    }

    bool isCalled() const
    {
        return _called;
    }

    bool isCancelled() const
    {
        return _cancelled;
    }

    bool isDisconnected() const
    {
        return _disconnected;
    }

    virtual std::string getMethod() const override
    {
        return _method;
    }

    virtual std::string getDescription() const override
    {
        return "description";
    }

    virtual void onRequest(const Request &request) override
    {
        _called = true;
        request.reply(_reply);
    }

    virtual void onDisconnect() override
    {
        _disconnected = true;
    }

    virtual void onCancel() override
    {
        _cancelled = true;
    }

private:
    std::string _method;
    int _reply;
    bool _priority;
    bool _called = false;
    bool _cancelled = false;
    bool _disconnected = false;
};

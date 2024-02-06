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

#include <brayns/network/INetworkInterface.h>

class MockInterface : public brayns::INetworkInterface
{
public:
    const std::vector<brayns::EntrypointRef> &getEntrypoints() const
    {
        return _entrypoints;
    }

    bool isPolled() const
    {
        return _polled;
    }

    bool isStoped() const
    {
        return _stopped;
    }

    virtual void registerEntrypoint(brayns::EntrypointRef entrypoint) override
    {
        _entrypoints.push_back(std::move(entrypoint));
    }

    virtual void poll() override
    {
        _polled = true;
    }

    virtual void stop() override
    {
        _stopped = true;
    }

private:
    std::vector<brayns::EntrypointRef> _entrypoints;
    bool _polled = false;
    bool _stopped = false;
};

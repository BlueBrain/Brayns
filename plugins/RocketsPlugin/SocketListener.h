/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 *                          Daniel.Nachbaur@epfl.ch
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

#include <rockets/socketBasedInterface.h>
#include <rockets/socketListener.h>

#include <map>
#include <uvw.hpp>

namespace brayns
{
/**
 * Implements a rockets::SocketListener to be integrated in a libuv event loop.
 */
class SocketListener : public rockets::SocketListener
{
public:
    SocketListener(rockets::SocketBasedInterface& interface);

    void setPostReceiveCallback(const std::function<void()>& callback)
    {
        _postReceive = callback;
    }

    void onNewSocket(const rockets::SocketDescriptor fd, int mode) final;

    void onUpdateSocket(const rockets::SocketDescriptor fd, int mode) final;

    void onDeleteSocket(const rockets::SocketDescriptor fd) final;

private:
    std::map<rockets::SocketDescriptor, std::shared_ptr<uvw::PollHandle>>
        _handles;
    rockets::SocketBasedInterface& _iface;
    std::function<void()> _postReceive;
};
}

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

#include "SocketListener.h"

#include <iostream>
#include <poll.h>

namespace brayns
{
SocketListener::SocketListener(rockets::SocketBasedInterface& interface)
    : _iface{interface}
{
    if (!uvw::Loop::getDefault()->alive())
        throw std::runtime_error("No libuv loop is alive");
}

void SocketListener::onNewSocket(const rockets::SocketDescriptor fd,
                                 const int mode)
{
    auto loop = uvw::Loop::getDefault();
    auto handle = loop->resource<uvw::PollHandle>(fd);
    _handles.emplace(fd, handle);

    uvw::Flags<uvw::PollHandle::Event> flags;
    if (mode & POLLIN)
        flags = flags | uvw::Flags<uvw::PollHandle::Event>(
                            uvw::PollHandle::Event::READABLE);
    if (mode & POLLOUT)
        flags = flags | uvw::Flags<uvw::PollHandle::Event>(
                            uvw::PollHandle::Event::WRITABLE);
    handle->on<uvw::PollEvent>([this, fd](const auto& event, auto&) {
        int flags_ = 0;
        if (event.flags() & uvw::PollHandle::Event::READABLE)
            flags_ |= POLLIN;
        if (event.flags() & uvw::PollHandle::Event::WRITABLE)
            flags_ |= POLLOUT;
        _iface.processSocket(fd, flags_);

        if ((event.flags() & uvw::PollHandle::Event::READABLE) && postReceive)
            postReceive();
    });

    handle->start(flags);
}

void SocketListener::onUpdateSocket(const rockets::SocketDescriptor fd,
                                    const int mode)
{
    uvw::Flags<uvw::PollHandle::Event> flags;
    if (mode & POLLIN)
        flags = flags | uvw::Flags<uvw::PollHandle::Event>(
                            uvw::PollHandle::Event::READABLE);
    if (mode & POLLOUT)
        flags = flags | uvw::Flags<uvw::PollHandle::Event>(
                            uvw::PollHandle::Event::WRITABLE);
    _handles[fd]->start(flags);
}

void SocketListener::onDeleteSocket(const rockets::SocketDescriptor fd)
{
    _handles[fd]->stop();
    _handles.erase(fd);
}
}

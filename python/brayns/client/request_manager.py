# Copyright 2015-2024 Blue Brain Project/EPFL
# All rights reserved. Do not distribute without permission.
#
# Responsible Author: adrien.fleury@epfl.ch
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License version 3.0 as published
# by the Free Software Foundation.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

import threading
from typing import Union

from .reply import Reply
from .request import Request


class _Monitor:

    def __init__(self) -> None:
        self._reply = None
        self._lock = threading.Condition()

    def get_reply(self, timeout: Union[float, None]) -> Reply:
        with self._lock:
            if self._reply is not None:
                return self._reply
            if not self._lock.wait(timeout):
                raise TimeoutError('Reply timeout')
        return self._reply

    def set_reply(self, reply: Reply) -> None:
        with self._lock:
            self._reply = reply
            self._lock.notify_all()


class RequestManager:

    def __init__(self) -> None:
        self._requests = {}

    def add_request(self, request: Request) -> None:
        if not request.is_notification():
            self._requests[request.request_id] = _Monitor()

    def add_reply(self, reply: Reply) -> None:
        request = self._requests.get(reply.request_id)
        if request is not None:
            request.set_reply(reply)

    def get_reply(
        self,
        request: Request,
        timeout: Union[float, None] = None
    ) -> Reply:
        request_id = request.request_id
        try:
            return self._requests[request_id].get_reply(timeout)
        finally:
            del self._requests[request_id]

    def clear(self) -> None:
        for request in self._requests.values():
            request.set_reply(
                Reply.from_error_message('Connection closed')
            )
        self._requests.clear()

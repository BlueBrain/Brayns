# Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

from brayns.network.websocket import WebSocket, WebSocketListener


class MockWebSocket(WebSocket):
    def __init__(self, listener: WebSocketListener) -> None:
        self.binary_request = b""
        self.binary_reply = b""
        self.text_request = ""
        self.text_reply = ""
        self._listener = listener
        self._closed = False

    @property
    def closed(self) -> bool:
        return self._closed

    def close(self) -> None:
        self._closed = True

    def poll(self, *_) -> None:
        if self.binary_reply:
            self._listener.on_binary(self.binary_reply)
        if self.text_reply:
            self._listener.on_text(self.text_reply)

    def send_binary(self, data: bytes) -> None:
        self.binary_request = data

    def send_text(self, data: str) -> None:
        self.text_request = data

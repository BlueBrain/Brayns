# Copyright (c) 2021 EPFL/Blue Brain Project
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

from typing import Union

from .websocket_client import WebsocketClient

from . import reply
from . import request


class JsonRpcClient:

    def __init__(self) -> None:
        self._manager = request.Manager()
        self._client = WebsocketClient(
            callback=self._on_frame_received
        )

    def connect(
        self,
        uri: str,
        secure: bool = False,
        cafile: Union[str, None] = None
    ) -> None:
        self._manager.clear()
        self._client.connect(
            uri=uri,
            secure=secure,
            cafile=cafile
        )

    def disconnect(self) -> None:
        self._manager.clear()
        self._client.disconnect()

    def send(self, message: request.Message) -> None:
        self._manager.add_request(message)
        self._client.send(request.to_json(message))

    def get_reply(
        self,
        request: request.Message,
        timeout: Union[None, float] = None
    ) -> reply.Message:
        return self._manager.get_reply(request, timeout)

    def _on_frame_received(self, data):
        try:
            self._manager.add_reply(reply.from_json(data))
        except reply.ParsingError:
            pass

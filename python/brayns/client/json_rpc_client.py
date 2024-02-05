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

from typing import Union

from .reply import Reply
from .reply_error import ReplyError
from .request import Request
from .request_manager import RequestManager
from .websocket_client import WebsocketClient


class JsonRpcClient:

    def __init__(self) -> None:
        self._manager = RequestManager()
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

    def send(self, request: Request) -> None:
        self._manager.add_request(request)
        self._client.send(request.to_json())

    def get_reply(
        self,
        request: Request,
        timeout: Union[None, float] = None
    ) -> Reply:
        return self._manager.get_reply(request, timeout)

    def _on_frame_received(self, data):
        try:
            self._manager.add_reply(Reply.from_json(data))
        except ReplyError:
            pass

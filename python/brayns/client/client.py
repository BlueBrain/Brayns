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

import logging
from typing import Any, Optional

from .jsonrpc.json_rpc_client import JsonRpcClient
from .request_future import RequestFuture
from .websocket.web_socket import WebSocket


class Client:
    """Brayns client implementation to connect to a renderer."""

    def __init__(
        self,
        uri: str,
        secure: bool = False,
        cafile: Optional[str] = None,
        loglevel=logging.INFO
    ) -> None:
        """Connect to the renderer using the given settings.

        :param uri: Renderer URI in format 'host:port'
        :type uri: str
        :param secure: True if SSL is enabled, defaults to False
        :type secure: bool, optional
        :param cafile: Additional certification authority, defaults to None
        :type cafile: Optional[str], optional
        :param loglevel: Log level, defaults to logging.INFO
        :type loglevel: int
        """
        self._client = JsonRpcClient(
            logger=logging.Logger('Brayns', loglevel),
            websocket=WebSocket(
                uri=uri,
                secure=secure,
                cafile=cafile
            )
        )

    def __enter__(self) -> None:
        """Allow using Brayns client in context manager."""
        return self

    def __exit__(self, *_) -> None:
        """Disconnect from Brayns renderer when exiting context manager."""
        self.disconnect()

    def disconnect(self) -> None:
        """Disconnect the client from the renderer.

        The client should not be used anymore after disconnection.
        """
        self._client.disconnect()

    def request(self, method: str, params: Any = None) -> Any:
        """Send a JSON-RPC request to the renderer.

        Raise a RequestError if an error message is received.

        :param method: method name
        :type method: str
        :param params: request params, defaults to None
        :type params: Any, optional
        :return: reply result
        :rtype: Any
        """
        return self.task(method, params).get_result()

    def task(self, method: str, params: Any = None) -> RequestFuture:
        """Send a JSON-RPC request to the renderer with progress support.

        Raise a RequestError if an error message is received.

        :param method: method name
        :type method: str
        :param params: request params, defaults to None
        :type params: Any, optional
        :return: future to monitor the request
        :rtype: RequestFuture
        """
        return self._client.send(method, params)

# Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

from logging import Logger
from ssl import SSLContext
from typing import Protocol, Self

from websockets.client import WebSocketClientProtocol, connect
from websockets.exceptions import WebSocketException


class WebSocketError(Exception): ...


class ServiceUnavailable(Exception): ...


class WebSocket(Protocol):
    async def __aenter__(self) -> Self:
        return self

    async def __aexit__(self, *_) -> None:
        await self.close()

    @property
    def host(self) -> str: ...

    @property
    def port(self) -> int: ...

    async def close(self) -> None: ...

    async def send(self, data: bytes | str) -> None: ...

    async def receive(self) -> bytes | str: ...


def _slice_if_needed(
    data: bytes | str, max_size: int
) -> bytes | str | list[bytes | str]:
    size = len(data)

    if size < max_size:
        return data

    return [data[i : i + max_size] for i in range(0, size, max_size)]


class _WebSocket(WebSocket):
    def __init__(
        self, websocket: WebSocketClientProtocol, max_frame_size: int, logger: Logger
    ) -> None:
        self._websocket = websocket
        self._max_frame_size = max_frame_size
        self._logger = logger

    @property
    def host(self) -> str:
        return self._websocket.remote_address[0]

    @property
    def port(self) -> int:
        return self._websocket.remote_address[1]

    async def close(self) -> None:
        self._logger.info("Closing websocket connection")

        try:
            await self._websocket.close()
        except WebSocketException as e:
            self._logger.warning("Failed to close connection: %s", e)
            raise WebSocketError(str(e))

        self._logger.info("Websocket connection closed")

    async def send(self, data: bytes | str) -> None:
        self._logger.info("Sending frame of %d bytes", len(data))
        self._logger.debug("Frame content: %s", data)

        sliced = _slice_if_needed(data, self._max_frame_size)

        if isinstance(sliced, list):
            self._logger.info("Frame too big, fragmented in %d parts", len(sliced))

        try:
            await self._websocket.send(sliced)
        except WebSocketException as e:
            self._logger.warning("Failed to send frame: %s", e)
            raise WebSocketError(str(e))

        self._logger.info("Frame sent")

    async def receive(self) -> bytes | str:
        self._logger.info("Waiting for frame")

        try:
            data = await self._websocket.recv()
        except WebSocketException as e:
            self._logger.info("Receive error: %s", e)
            raise WebSocketError(str(e))

        self._logger.info("Received frame of %d bytes", len(data))
        self._logger.debug("Frame content: %s", data)

        return data


async def connect_websocket(
    url: str, ssl: SSLContext | None, max_frame_size: int, logger: Logger
) -> WebSocket:
    logger.info("Connection to websocket server at URL %s", url)

    try:
        websocket = await connect(url, ssl=ssl, max_size=max_frame_size)
    except WebSocketException as e:
        logger.warning("Connection failed: %s", e)
        raise WebSocketError(str(e))
    except OSError as e:
        logger.warning("Service not found (probably not ready): %s", e)
        raise ServiceUnavailable(str(e))

    wrapper = _WebSocket(websocket, max_frame_size, logger)

    logger.info("Connected to %s:%d", wrapper.host, wrapper.port)

    return wrapper

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

import asyncio
from logging import Logger
from ssl import SSLContext
from typing import Any, NamedTuple
from .json_rpc import (
    JsonRpcErrorResponse,
    JsonRpcRequest,
    JsonRpcResponse,
    JsonRpcError,
    compose_request,
    parse_response,
    JsonRpcId,
    Response,
)
from .websocket import ServiceUnavailable, WebSocket, connect_websocket


class JsonRpcBuffer:
    def __init__(self) -> None:
        self._responses = dict[JsonRpcId, Response | None]()

    def is_running(self, request_id: JsonRpcId) -> bool:
        return request_id in self._responses

    def is_done(self, request_id: JsonRpcId) -> bool:
        if request_id not in self._responses:
            raise ValueError("Unknown request ID")

        return self._responses[request_id] is not None

    def add_request(self, request_id: JsonRpcId) -> None:
        self._responses[request_id] = None

    def add_global_error(self, error: JsonRpcError) -> None:
        response = JsonRpcErrorResponse(None, error)

        for request_id in self._responses:
            self._responses[request_id] = response

    def add_response(self, message: Response) -> None:
        if message.id is None:
            self.add_global_error(message.error)
            return

        if message.id not in self._responses:
            raise ValueError("No requests match given response ID")

        if self._responses[message.id] is not None:
            raise ValueError("Two responses received with same ID")

        self._responses[message.id] = message

    def get_response(self, request_id: JsonRpcId) -> Response | None:
        if not self.is_done(request_id):
            return None

        return self._responses.pop(request_id)


class Result(NamedTuple):
    value: Any
    binary: bytes


class JsonRpcFuture:
    def __init__(
        self, request_id: JsonRpcId | None, websocket: WebSocket, buffer: JsonRpcBuffer
    ) -> None:
        self._request_id = request_id
        self._websocket = websocket
        self._buffer = buffer

    @property
    def request_id(self) -> JsonRpcId | None:
        return self._request_id

    @property
    def done(self) -> bool:
        if self._request_id is None:
            return True

        return self._buffer.is_done(self._request_id)

    async def poll(self) -> None:
        data = await self._websocket.receive()
        message = parse_response(data)
        self._buffer.add_response(message)

    async def wait_for_result(self) -> Result:
        if self._request_id is None:
            raise ValueError("Cannot wait for result of requests without ID")

        while True:
            response = self._buffer.get_response(self._request_id)

            if response is None:
                await self.poll()
                continue

            if isinstance(response, JsonRpcResponse):
                return Result(response.result, response.binary)

            if isinstance(response, JsonRpcErrorResponse):
                raise response.error

            raise TypeError("Invalid response type")


class Connection:
    def __init__(self, websocket: WebSocket) -> None:
        self._websocket = websocket
        self._buffer = JsonRpcBuffer()

    async def send(self, request: JsonRpcRequest) -> JsonRpcFuture:
        data = compose_request(request)

        await self._websocket.send(data)

        return JsonRpcFuture(request.id, self._websocket, self._buffer)

    async def start(
        self, method: str, params: Any = None, binary: bytes = b""
    ) -> JsonRpcFuture:
        request_id = 0

        while self._buffer.is_running(request_id):
            request_id += 1

        request = JsonRpcRequest(method, params, binary, request_id)

        return await self.send(request)

    async def request(
        self, method: str, params: Any = None, binary: bytes = b""
    ) -> Result:
        future = await self.start(method, params, binary)

        return await future.wait_for_result()


async def connect(
    url: str,
    ssl: SSLContext,
    max_frame_size: int,
    max_attempts: int | None = 1,
    sleep_between_attempts: float = 1,
    logger: Logger | None = None,
) -> Connection:
    if logger is None:
        logger = Logger("Brayns")

    attempt = 0

    while True:
        try:
            logger.info("Connection attempt %d", attempt)
            websocket = await connect_websocket(url, ssl, max_frame_size, logger)
            break
        except ServiceUnavailable as e:
            logger.warn("Connection attempt failed: %s", e)
            if max_attempts is not None and attempt >= max_attempts:
                logger.warn("Max connection attempts reached, aborted")
                raise

        await asyncio.sleep(sleep_between_attempts)

    logger.info("Connection suceeded")

    return Connection(websocket)

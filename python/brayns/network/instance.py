# Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

from __future__ import annotations

from typing import Any, Protocol, TypeVar

from .jsonrpc import JsonRpcFuture, JsonRpcReply, JsonRpcRequest

T = TypeVar("T", bound="Instance")


class Instance(Protocol):
    """Object used to interact with a connected instance of the backend.

    Contains the connection state with a backend instance and provide methods to
    interact with it.

    An instance is a running process of braynsService executable that runs a
    websocket server and performs the rendering tasks.

    The backend instance can be running on the local machine or a remote one.

    Once finished using an instance, user must call disconnect() on it or use
    its context manager to do it automatically when it goes out of scope.

    The recommended way of doing this is the context manager because it is safe
    regarding exceptions (ie with instance: ...).

    If a websocket communication error occurs, a WebSocketError is raised.

    If a JSON-RPC error message is replied, a RequestError is raised.

    For non-blocking requests, the future is iterable and yield RequestProgess
    objects. Call wait_for_result() to wait for the result.
    """

    def __enter__(self: T) -> T:
        """Context manager enter, just return self."""
        return self

    def __exit__(self, *_) -> None:
        """Context manager exit, close the connection."""
        self.disconnect()

    @property
    def connected(self) -> bool:
        """Check if the instance is connected.

        :return: Connection state.
        :rtype: bool
        """
        return True

    def disconnect(self) -> None:
        """Disconnect instance (mandatory once done with it)."""
        pass

    def request(self, method: str, params: Any = None) -> Any:
        """Send a request to the instance and wait its result.

        Generate automatically the JSON-RPC ID using integers.

        :param method: JSON-RPC method (entrypoint name).
        :type method: str
        :param params: JSON-RPC params, defaults to None
        :type params: Any, optional
        :return: JSON-RPC result.
        :rtype: Any
        """
        reply = self.execute(method, params)
        return reply.result

    def execute(
        self, method: str, params: Any = None, binary: bytes = b""
    ) -> JsonRpcReply:
        """Extended version of request to accept and return binary data.

        :param method: JSON-RPC method.
        :type method: str
        :param params: JSON-RPC params, defaults to None
        :type params: Any, optional
        :param binary: Binary data to send with the request, defaults to b''
        :type binary: bytes, optional
        :return: JSON-RPC result and binary data received with the reply.
        :rtype: tuple[Any, bytes]
        """
        task = self.task(method, params, binary)
        return task.wait_for_reply()

    def task(
        self, method: str, params: Any = None, binary: bytes = b""
    ) -> JsonRpcFuture:
        """Send a request to the instance in a non-blocking way.

        Generate automatically the JSON-RPC ID using integers.

        Use the future returned to wait for progresses or results.

        :param method: JSON-RPC method.
        :type method: str
        :param params: JSON-RPC params, defaults to None
        :type params: Any, optional
        :return: Future object to monitor the request.
        :rtype: RequestFuture
        """
        id = 0
        while self.is_running(id):
            id += 1
        request = JsonRpcRequest(id, method, params, binary)
        return self.send(request)

    def is_running(self, id: int | str) -> bool:
        """Check if the request with the given ID is running.

        :param id: JSON-RPC request ID.
        :type id: int | str
        :return: Running state.
        :rtype: bool
        """
        return False

    def send(self, request: JsonRpcRequest) -> JsonRpcFuture:
        """Send a request in a non blocking way.

        This method is the most basic one, it doesn't generate any ID and can be
        used asynchronously (doesn't block until result is received).

        :param request: JSON-RPC request to send.
        :type request: JsonRpcRequest
        :return: Future object to monitor the request.
        :rtype: RequestFuture
        """
        raise NotImplementedError()

    def poll(self, block: bool) -> None:
        """Check if messages are received from the instance.

        When a message is received, it will update all existing futures.

        The futures call this method in blocking mode when iterated.

        :param block: Wait until a message is received if True.
        :type block: bool
        """
        pass

    def cancel(self, id: int | str) -> None:
        """Cancel request with given ID.

        Send a cancellation request to the instance for a given task.

        :param id: JSON-RPC request ID.
        :type id: int | str
        :raises RequestError: Task is not running.
        """
        pass

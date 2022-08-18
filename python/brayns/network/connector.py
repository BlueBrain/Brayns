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

from __future__ import annotations

import logging
import time
from collections.abc import Callable
from dataclasses import dataclass, field
from typing import cast

from.client import Client
from.instance import Instance
from.jsonrpc.json_rpc_manager import JsonRpcManager
from.listener import Listener
from.logger import Logger
from.websocket.service_unavailable_error import ServiceUnavailableError
from.websocket.ssl_client_context import SslClientContext
from.websocket.web_socket_client import WebSocketClient
from.websocket.web_socket_connector import WebSocketConnector


@dataclass
class Connector:
    """Used to connect to a braynsService instance.

    A connector must at least have the URI the braynsService instance was
    started with.

    URI is always in format host:port, wss:// or ws:// will be added depending
    if an SSL context is provided (SSL is disabled if context is None).

    SSL context can be provided for a secure connection. If the instance uses a
    certificate signed by a CA installed on the local machine, the default value
    of brayns.SslClientContext() can be used. Otherwise, the CA file or path
    must be provided in the SSL context constructor.

    Binary messages received from the instance are not JSON-RPC requests but can
    be handled using an optional callback (see `binary_handler`).

    If you don't know when your instance of braynsService will be ready when you
    call connect(), you can set `max_attempts` to None to try to connect in loop
    until it works (or with a maximum count and a delay to have a timeout).

    :param uri: Instance URI with format 'host:port'.
    :type uri: str
    :param ssl_context: SSL context if secure, defaults to None.
    :type ssl_context: SslClientContext | None, optional
    :param binary_handler: Callback for binary, defaults to (bytes) -> None.
    :type binary_handler: Callable[[bytes], None], optional
    :param logger: Instance logger, defaults to brayns.Logger().
    :type logger: logging.Logger, optional
    :param max_attempts: Max connection attempts, defaults to 1.
    :type max_attempts: int | None, optional
    :param attempt_period: Delay in seconds between attempts, defaults to 0.1.
    :type attempt_period: float, optional
    """

    uri: str
    ssl_context: SslClientContext | None = None
    binary_handler: Callable[[bytes], None] = lambda _: None
    logger: logging.Logger = field(default_factory=Logger)
    max_attempts: int | None = 1
    attempt_period: float = 0.1

    def connect(self) -> Instance:
        """Connect to instance and return it.

        Try to connect `max_attempts` times waiting `attempt_period` between two
        tries. If it fails, ServiceUnavailableError will be raised.

        :raises WebSocketError
        :return: Connected braynsService instance.
        :rtype: Instance
        """
        manager = JsonRpcManager(self.logger)
        listener = Listener(self.logger, self.binary_handler, manager)
        connector = WebSocketConnector(self.uri, listener, self.ssl_context)
        websocket = self._open_websocket(connector)
        return Client(websocket, self.logger, manager)

    def _open_websocket(self, connector: WebSocketConnector) -> WebSocketClient:
        self.logger.info('Connection to instance at "%s".', self.uri)
        try:
            websocket = self._try_connect(connector)
        except Exception as e:
            self.logger.info('Connection failed: "%s".', e)
            raise
        self.logger.info('Successfully connected.')
        return websocket

    def _try_connect(self, connector: WebSocketConnector) -> WebSocketClient:
        if self.max_attempts is None:
            return self._try_connect_forever(connector)
        return self._try_connect_with_max_attempts(connector)

    def _try_connect_forever(self, connector: WebSocketConnector) -> WebSocketClient:
        while True:
            self.logger.debug('Connection attempt.')
            try:
                return connector.connect()
            except ServiceUnavailableError:
                time.sleep(self.attempt_period)

    def _try_connect_with_max_attempts(self, connector: WebSocketConnector) -> WebSocketClient:
        max_attempts = cast(int, self.max_attempts)
        count = 1
        while True:
            self.logger.debug('Connection attempt %d/%d.', count, max_attempts)
            try:
                return connector.connect()
            except ServiceUnavailableError:
                if count == max_attempts:
                    self.logger.info('Max attempts reached.')
                    raise
                time.sleep(self.attempt_period)
            count += 1
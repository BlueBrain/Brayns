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

    uri: str
    ssl_context: SslClientContext | None = None
    binary_handler: Callable[[bytes], None] = lambda _: None
    logger: logging.Logger = field(default_factory=Logger)
    max_attempts: int | None = 1
    attempt_period: float = 0.1

    def connect(self) -> Instance:
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

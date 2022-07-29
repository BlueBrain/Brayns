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
import time
from collections.abc import Callable
from dataclasses import dataclass, field
from typing import Optional

from brayns.instance.client import Client
from brayns.instance.instance import Instance
from brayns.instance.jsonrpc.json_rpc_manager import JsonRpcManager
from brayns.instance.listener import Listener
from brayns.instance.logger import Logger
from brayns.instance.websocket.connection_failed_error import ConnectionFailedError
from brayns.instance.websocket.ssl_client_context import SslClientContext
from brayns.instance.websocket.web_socket_connector import WebSocketConnector


@dataclass
class Connector:

    uri: str
    ssl_context: Optional[SslClientContext] = None
    binary_handler: Callable[[bytes], None] = lambda _: None
    logger: logging.Logger = field(default_factory=Logger)

    def connect(self, max_attempts: Optional[int] = 1, attempt_period: float = 0.1) -> Instance:
        count = 0
        while True:
            self.logger.debug('Connection attempt %s.', count)
            try:
                return self._connect()
            except ConnectionFailedError as e:
                count += 1
                self.logger.debug('Connection failed: "%s".', e)
                if max_attempts is not None and count >= max_attempts:
                    self.logger.error('Max connection attempts reached.')
                    raise e
                time.sleep(attempt_period)
            except Exception as e:
                self.logger.error('Connection refused: "%s".', e)
                raise e

    def _connect(self) -> Instance:
        manager = JsonRpcManager(self.logger)
        listener = Listener(self.logger, self.binary_handler, manager)
        connector = WebSocketConnector(self.uri, listener, self.ssl_context)
        websocket = connector.connect()
        self.logger.info('Successfully connected to "%s".', self.uri)
        client = Client(websocket, self.logger, manager)
        return client

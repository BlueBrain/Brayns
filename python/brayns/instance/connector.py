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
import sys
import time
from collections.abc import Callable
from typing import Optional

from brayns.instance.client import Client
from brayns.instance.instance import Instance
from brayns.instance.jsonrpc.json_rpc_manager import JsonRpcManager
from brayns.instance.listener import Listener
from brayns.instance.websocket.connection_failed_error import ConnectionFailedError
from brayns.instance.websocket.ssl_client_context import SslClientContext
from brayns.instance.websocket.web_socket_connector import WebSocketConnector


class Connector:

    @staticmethod
    def get_default_logger(level: int = logging.WARN) -> logging.Logger:
        logger = logging.Logger('Brayns', level)
        logger.setLevel(level)
        handler = logging.StreamHandler(sys.stdout)
        logger.addHandler(handler)
        format = '[%(name)s][%(levelname)s] %(message)s'
        formatter = logging.Formatter(format)
        handler.setFormatter(formatter)
        return logger

    def __init__(
        self,
        uri: str,
        ssl: Optional[SslClientContext] = None,
        binary_handler: Callable[[bytes], None] = lambda _: None,
        logger: Optional[logging.Logger] = None
    ) -> None:
        self._logger = self.get_default_logger() if logger is None else logger
        self._manager = JsonRpcManager(self._logger)
        self._connector = WebSocketConnector(
            uri=uri,
            ssl=ssl,
            listener=Listener(self._logger, binary_handler, self._manager)
        )

    def connect(self, max_attempts: Optional[int] = 1, attempt_period: float = 0.1) -> Instance:
        count = 0
        while True:
            try:
                return self._connect()
            except ConnectionFailedError as e:
                count += 1
                if max_attempts is not None and count >= max_attempts:
                    raise e
                time.sleep(attempt_period)

    def _connect(self) -> Instance:
        websocket = self._connector.connect()
        client = Client(websocket, self._logger, self._manager)
        return client

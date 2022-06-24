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
from typing import Callable, Optional

from brayns.core.version import Version
from brayns.instance.client import Client
from brayns.instance.instance import Instance
from brayns.instance.jsonrpc.json_rpc_manager import JsonRpcManager
from brayns.instance.listener import Listener
from brayns.instance.websocket.web_socket_client import WebSocketClient
from brayns.instance.websocket.web_socket_connector import WebSocketConnector
from brayns.instance.websocket.web_socket_error import WebSocketError
from brayns.version import DEV_VERSION, __version__


def connect(
    uri: str,
    secure: bool = False,
    cafile: Optional[str] = None,
    on_binary: Callable[[bytes], None] = lambda _: None,
    log_level: int = logging.WARN,
    log_handler: Optional[logging.Handler] = None,
    max_attempts: Optional[int] = None
) -> Instance:
    logger = _create_logger(log_level, log_handler)
    manager = JsonRpcManager.create(logger)
    listener = Listener(logger, on_binary, manager)
    connector = WebSocketConnector(uri, listener, secure, cafile)
    websocket = _open_websocket(connector, max_attempts)
    client = Client(websocket, logger, manager)
    _check_version(client, logger)
    return client


def _create_logger(level: int = logging.WARN, handler: Optional[logging.Handler] = None) -> logging.Logger:
    logger = logging.Logger('Brayns', logging.WARN)
    logger.setLevel(level)
    if handler is None:
        handler = logging.StreamHandler(sys.stdout)
    logger.addHandler(handler)
    format = '[%(name)s][%(levelname)s] %(message)s'
    formatter = logging.Formatter(format)
    handler.setFormatter(formatter)
    return logger


def _open_websocket(connector: WebSocketConnector, max_attempts: Optional[int]) -> WebSocketClient:
    count = 0
    while True:
        try:
            return connector.connect()
        except WebSocketError as e:
            count += 1
            if max_attempts is not None and count >= max_attempts:
                raise e
        time.sleep(0.1)


def _check_version(instance: Instance, logger: logging.Logger) -> None:
    local = __version__
    logger.info('Python package version: %s.', local)
    version = Version.from_instance(instance)
    remote = version.tag
    logger.info('Instance version: %s.', remote)
    if local == DEV_VERSION:
        return
    if remote != local:
        raise RuntimeError(f'Version mismatch {remote=} {local=}')

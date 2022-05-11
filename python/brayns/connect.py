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
from typing import Optional

from brayns.core.version import Version
from brayns.instance.client import Client
from brayns.instance.instance import Instance
from brayns.instance.jsonrpc.json_rpc_client import JsonRpcClient
from brayns.instance.websocket.web_socket_client import WebSocketClient
from brayns.version import DEV_VERSION, __version__


def connect(
    uri: str,
    secure: bool = False,
    cafile: Optional[str] = None,
    logger: Optional[logging.Logger] = None
) -> Instance:
    logger = _get_logger(logger)
    client = _connect(uri, secure, cafile, logger)
    _check_version(client)
    return client


def _get_logger(logger: Optional[logging.Logger]) -> logging.Logger:
    if logger is not None:
        return logger
    logger = logging.Logger('Brayns', logging.WARN)
    logger.addHandler(logging.StreamHandler(sys.stdout))
    return logger


def _connect(uri: str, secure: bool, cafile: Optional[str], logger: logging.Logger) -> Instance:
    logger.info('Connecting to instance at %s.', uri)
    websocket = WebSocketClient.connect(uri, secure, cafile)
    json_rpc_client = JsonRpcClient(websocket, logger)
    return Client(json_rpc_client)


def _check_version(instance: Instance) -> None:
    local = __version__
    if local == DEV_VERSION:
        return
    version = Version.from_instance(instance)
    remote = version.tag
    if remote != local:
        raise RuntimeError(f'Version mismatch {remote=} {local=}')

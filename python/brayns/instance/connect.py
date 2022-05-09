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

from brayns.instance.client import Client
from brayns.instance.instance import Instance
from brayns.instance.jsonrpc.json_rpc_client import JsonRpcClient
from brayns.instance.websocket.web_socket_client import WebSocketClient


def connect(
    uri: str,
    secure: bool = False,
    cafile: Optional[str] = None,
    logger: Optional[logging.Logger] = None,
) -> Instance:
    if logger is None:
        logger = logging.Logger('Brayns', logging.WARN)
        logger.addHandler(logging.StreamHandler(sys.stdout))
    logger.info('Connecting to instance at %s.', uri)
    websocket = WebSocketClient.connect(uri, secure, cafile)
    json_rpc_client = JsonRpcClient(websocket, logger)
    return Client(json_rpc_client)

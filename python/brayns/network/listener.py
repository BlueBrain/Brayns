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

import logging

from .jsonrpc import JsonRpcManager
from .websocket import WebSocketListener


class Listener(WebSocketListener):
    def __init__(self, logger: logging.Logger, manager: JsonRpcManager) -> None:
        self._logger = logger
        self._manager = manager

    def on_binary(self, data: bytes) -> None:
        self._logger.info("Binary frame received of %d bytes.", len(data))
        self._logger.debug('Reply binary frame data: "%s".', data)
        self._manager.process_binary(data)

    def on_text(self, data: str) -> None:
        self._logger.info("Text frame received of %d chars.", len(data))
        self._logger.debug('Reply text frame data: "%s".', data)
        self._manager.process_text(data)

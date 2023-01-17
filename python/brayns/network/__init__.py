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

"""
Subpackage to connect to a Brayns service instance (backend).

An instance is a wrapper around a websocket connection and a JSON-RPC context.

It provides functionalities to send JSON-RPC requests and receive replies with a
Brayns instance.
"""

from .connector import Connector
from .future import Future
from .instance import Instance
from .jsonrpc import (
    JsonRpcError,
    JsonRpcFuture,
    JsonRpcProgress,
    JsonRpcReply,
    JsonRpcRequest,
)
from .logger import Logger
from .websocket import (
    ConnectionClosedError,
    InvalidServerCertificateError,
    ProtocolError,
    ServiceUnavailableError,
    SslClientContext,
    WebSocketError,
)

__all__ = [
    "ConnectionClosedError",
    "Connector",
    "Future",
    "Instance",
    "InvalidServerCertificateError",
    "JsonRpcError",
    "JsonRpcFuture",
    "JsonRpcProgress",
    "JsonRpcReply",
    "JsonRpcRequest",
    "Logger",
    "ProtocolError",
    "ServiceUnavailableError",
    "SslClientContext",
    "WebSocketError",
]

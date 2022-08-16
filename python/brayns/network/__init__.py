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

"""
Subpackage to connect to a Brayns service instance (backend).

This package is a wrapper around a websocket connection and a JSON-RPC context.

It provides functionalities to send JSON-RPC requests and receive replies with a
Brayns instance.
"""

from .connector import Connector
from .instance import Instance
from .jsonrpc.json_rpc_request import JsonRpcRequest
from .jsonrpc.request_error import RequestError
from .jsonrpc.request_future import RequestFuture
from .jsonrpc.request_progress import RequestProgress
from .logger import Logger
from .websocket.connection_closed_error import ConnectionClosedError
from .websocket.invalid_server_certificate_error import InvalidServerCertificateError
from .websocket.protocol_error import ProtocolError
from .websocket.service_unavailable_error import ServiceUnavailableError
from .websocket.ssl_client_context import SslClientContext
from .websocket.web_socket_error import WebSocketError

__all__ = [
    'ConnectionClosedError',
    'Connector',
    'Instance',
    'InvalidServerCertificateError',
    'JsonRpcRequest',
    'Logger',
    'ProtocolError',
    'RequestError',
    'RequestFuture',
    'RequestProgress',
    'ServiceUnavailableError',
    'SslClientContext',
    'WebSocketError',
]

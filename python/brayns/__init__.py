# Copyright (c) 2015-2024 EPFL/Blue Brain Project
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
Brayns Python package.

This package provides an API to interact with Brayns service.
"""

from .api.core.service import (
    Endpoint,
    Task,
    TaskInfo,
    TaskOperation,
    Version,
    cancel_task,
    get_endpoint,
    get_methods,
    get_task,
    get_task_result,
    get_tasks,
    get_version,
    stop_service,
)
from .network.connection import Connection, FutureResponse, Request, Response, connect
from .network.json_rpc import (
    JsonRpcError,
    JsonRpcErrorResponse,
    JsonRpcId,
    JsonRpcRequest,
    JsonRpcResponse,
    JsonRpcSuccessResponse,
)
from .network.websocket import ServiceUnavailable, WebSocketError
from .version import VERSION
from .utils.logger import create_logger
from .api.core.objects import (
    Object,
    Metadata,
    get_object,
    get_all_objects,
    remove_objects,
    clear_objects,
    create_object,
)

__version__ = VERSION
"""Version tag of brayns Python package (major.minor.patch)."""

__all__ = [
    "cancel_task",
    "clear_objects",
    "connect",
    "Connection",
    "create_logger",
    "create_object",
    "Endpoint",
    "FutureResponse",
    "get_all_objects",
    "get_endpoint",
    "get_methods",
    "get_object",
    "get_task_result",
    "get_task",
    "get_tasks",
    "get_version",
    "JsonRpcError",
    "JsonRpcErrorResponse",
    "JsonRpcId",
    "JsonRpcRequest",
    "JsonRpcResponse",
    "JsonRpcSuccessResponse",
    "Metadata",
    "Object",
    "remove_objects",
    "Request",
    "Response",
    "ServiceUnavailable",
    "stop_service",
    "Task",
    "TaskInfo",
    "TaskOperation",
    "Version",
    "WebSocketError",
]

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

from .api.core.objects import (
    Object,
    clear_objects,
    create_empty_object,
    get_all_objects,
    get_object,
    remove_objects,
    update_object,
)
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
from .utils.logger import create_logger
from .utils.vector import Vector2, Vector3, Vector4
from .utils.quaternion import Quaternion
from .utils.rotation import Rotation, euler, get_rotation_between, axis_angle
from .utils.view import (
    View,
    X,
    Y,
    Z,
    FRONT_VIEW,
    BACK_VIEW,
    TOP_VIEW,
    BOTTOM_VIEW,
    RIGHT_VIEW,
    LEFT_VIEW,
)
from .version import VERSION

__version__ = VERSION
"""Version tag of brayns Python package (major.minor.patch)."""

__all__ = [
    "axis_angle",
    "BACK_VIEW",
    "BOTTOM_VIEW",
    "cancel_task",
    "clear_objects",
    "connect",
    "Connection",
    "create_empty_object",
    "create_logger",
    "Endpoint",
    "euler",
    "FRONT_VIEW",
    "FutureResponse",
    "get_all_objects",
    "get_endpoint",
    "get_methods",
    "get_object",
    "get_rotation_between",
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
    "LEFT_VIEW",
    "Object",
    "Quaternion",
    "remove_objects",
    "Request",
    "Response",
    "RIGHT_VIEW",
    "Rotation",
    "ServiceUnavailable",
    "stop_service",
    "Task",
    "TaskInfo",
    "TaskOperation",
    "TOP_VIEW",
    "update_object",
    "Vector2",
    "Vector3",
    "Vector4",
    "Version",
    "View",
    "WebSocketError",
    "X",
    "Y",
    "Z",
]

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

from .api.core.camera import (
    Camera,
    CameraId,
    CameraSettings,
    DepthOfField,
    OrthographicCamera,
    OrthographicSettings,
    PerspectiveCamera,
    PerspectiveSettings,
    Stereo,
    StereoMode,
    create_orthographic_camera,
    create_panoramic_camera,
    create_perspective_camera,
    get_camera_settings,
    get_orthographic_camera,
    get_orthographic_settings,
    get_panoramic_camera,
    get_panoramic_settings,
    get_perspective_camera,
    get_perspective_distance,
    get_perspective_settings,
    update_camera_settings,
    update_orthographic_settings,
    update_panoramic_settings,
    update_perspective_settings,
)
from .api.core.framebuffer import (
    Accumulation,
    Framebuffer,
    FramebufferChannel,
    FramebufferFormat,
    FramebufferId,
    FramebufferSettings,
    create_framebuffer,
    get_framebuffer,
    get_framebuffer_info,
)
from .api.core.image import (
    JpegChannel,
    PngChannel,
    read_framebuffer,
    read_framebuffer_as_exr,
    read_framebuffer_as_jpeg,
    read_framebuffer_as_png,
)
from .api.core.image_operation import (
    ImageOperation,
    ImageOperationId,
    ToneMapper,
    ToneMapperSettings,
    create_tone_mapper,
    get_tone_mapper,
    get_tone_mapper_settings,
    update_tone_mapper_settings,
)
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
    cancel_all_tasks,
    cancel_task,
    get_endpoint,
    get_methods,
    get_task,
    get_task_result,
    get_tasks,
    get_version,
    stop_service,
)
from .api.core.transfer_function import (
    LinearTransferFunction,
    LinearTransferFunctionSettings,
    TransferFunction,
    TransferFunctionId,
    create_linear_transfer_function,
    get_linear_transfer_function,
    get_linear_transfer_function_settings,
    update_linear_transfer_function_settings,
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
from .utils.box import Box1, Box2, Box3
from .utils.color import Color3, Color4
from .utils.logger import create_logger
from .utils.quaternion import Quaternion
from .utils.rotation import Rotation, axis_angle, euler, get_rotation_between
from .utils.vector import Vector2, Vector3, Vector4
from .utils.view import View, X, Y, Z
from .version import VERSION

__version__ = VERSION
"""Version tag of brayns Python package (major.minor.patch)."""

__all__ = [
    "Accumulation",
    "axis_angle",
    "Box1",
    "Box2",
    "Box3",
    "Camera",
    "CameraId",
    "CameraSettings",
    "cancel_all_tasks",
    "cancel_task",
    "clear_objects",
    "Color3",
    "Color4",
    "connect",
    "Connection",
    "create_empty_object",
    "create_framebuffer",
    "create_linear_transfer_function",
    "create_logger",
    "create_orthographic_camera",
    "create_panoramic_camera",
    "create_perspective_camera",
    "create_tone_mapper",
    "DepthOfField",
    "Endpoint",
    "euler",
    "Framebuffer",
    "FramebufferChannel",
    "FramebufferFormat",
    "FramebufferId",
    "FramebufferSettings",
    "FutureResponse",
    "get_all_objects",
    "get_camera_settings",
    "get_endpoint",
    "get_framebuffer_info",
    "get_framebuffer",
    "get_linear_transfer_function_settings",
    "get_linear_transfer_function",
    "get_methods",
    "get_object",
    "get_orthographic_camera",
    "get_orthographic_settings",
    "get_panoramic_camera",
    "get_panoramic_settings",
    "get_perspective_camera",
    "get_perspective_distance",
    "get_perspective_settings",
    "get_rotation_between",
    "get_task_result",
    "get_task",
    "get_tasks",
    "get_tone_mapper_settings",
    "get_tone_mapper",
    "get_version",
    "ImageOperation",
    "ImageOperationId",
    "JpegChannel",
    "JsonRpcError",
    "JsonRpcErrorResponse",
    "JsonRpcId",
    "JsonRpcRequest",
    "JsonRpcResponse",
    "JsonRpcSuccessResponse",
    "LinearTransferFunction",
    "LinearTransferFunctionSettings",
    "Object",
    "OrthographicCamera",
    "OrthographicSettings",
    "PerspectiveCamera",
    "PerspectiveSettings",
    "PngChannel",
    "Quaternion",
    "read_framebuffer_as_exr",
    "read_framebuffer_as_jpeg",
    "read_framebuffer_as_png",
    "read_framebuffer",
    "remove_objects",
    "Request",
    "Response",
    "Rotation",
    "ServiceUnavailable",
    "Stereo",
    "StereoMode",
    "stop_service",
    "Task",
    "TaskInfo",
    "TaskOperation",
    "ToneMapper",
    "ToneMapperSettings",
    "TransferFunction",
    "TransferFunctionId",
    "update_camera_settings",
    "update_linear_transfer_function_settings",
    "update_object",
    "update_orthographic_settings",
    "update_panoramic_settings",
    "update_perspective_settings",
    "update_tone_mapper_settings",
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

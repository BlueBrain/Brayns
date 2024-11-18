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
    GetCameraResult,
    GetOrthographicCameraResult,
    GetPanoramicCameraResult,
    GetPerspectiveCameraResult,
    OrthographicCamera,
    PanoramicCamera,
    PerspectiveCamera,
    Stereo,
    UpdateCameraParams,
    UpdateOrthographicCameraParams,
    UpdatePanoramicCameraParams,
    UpdatePerspectiveCameraParams,
    create_orthographic_camera,
    create_panoramic_camera,
    create_perspective_camera,
    get_camera,
    get_orthographic_camera,
    get_panoramic_camera,
    get_perspective_camera,
    update_camera,
    update_orthographic_camera,
    update_panoramic_camera,
    update_perspective_camera,
)
from .api.core.framebuffer import (
    CreateFramebufferParams,
    Framebuffer,
    FramebufferChannel,
    FramebufferFormat,
    GetFramebufferResult,
    UpdateFramebufferParams,
    clear_framebuffer,
    create_framebuffer,
    get_framebuffer,
    update_framebuffer,
)
from .api.core.geometry import (
    CreateTriangleMeshParams,
    GetTriangleMeshResult,
    create_quad_mesh,
    create_triangle_mesh,
    get_quad_mesh,
    get_triangle_mesh,
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
    CreateToneMapperParams,
    GetToneMapperResult,
    ImageOperation,
    ToneMapper,
    UpdateToneMapperParams,
    create_tone_mapper,
    get_tone_mapper,
    update_tone_mapper,
)
from .api.core.objects import (
    CreateObjectParams,
    EmptyObject,
    GetObjectResult,
    Object,
    UpdateObjectParams,
    clear_objects,
    create_empty_object,
    get_all_objects,
    get_object,
    remove_objects,
    update_object,
)
from .api.core.service import (
    Endpoint,
    Version,
    get_endpoint,
    get_methods,
    get_version,
    stop_service,
)
from .api.core.transfer_function import (
    CreateLinearTransferFunctionParams,
    GetLinearTransferFunctionResult,
    LinearTransferFunction,
    TransferFunction,
    UpdateLinearTransferFunctionParams,
    create_linear_transfer_function,
    get_linear_transfer_function,
    update_linear_transfer_function,
)
from .api.core.volume import (
    CreateRegularVolumeParams,
    GetRegularVolumeResult,
    RegularVolume,
    UpdateRegularVolumeParams,
    Volume,
    VolumeFilter,
    VoxelType,
    create_regular_volume,
    get_regular_volume,
    update_regular_volume,
)
from .network.connection import Connection, FutureResponse, Response, connect
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
    "axis_angle",
    "Box1",
    "Box2",
    "Box3",
    "Camera",
    "clear_framebuffer",
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
    "create_quad_mesh",
    "create_regular_volume",
    "create_tone_mapper",
    "create_triangle_mesh",
    "CreateFramebufferParams",
    "CreateLinearTransferFunctionParams",
    "CreateObjectParams",
    "CreateRegularVolumeParams",
    "CreateToneMapperParams",
    "CreateTriangleMeshParams",
    "EmptyObject",
    "Endpoint",
    "euler",
    "Framebuffer",
    "FramebufferChannel",
    "FramebufferFormat",
    "FutureResponse",
    "get_all_objects",
    "get_camera",
    "get_endpoint",
    "get_framebuffer",
    "get_framebuffer",
    "get_linear_transfer_function",
    "get_linear_transfer_function",
    "get_methods",
    "get_object",
    "get_orthographic_camera",
    "get_orthographic_camera",
    "get_panoramic_camera",
    "get_panoramic_camera",
    "get_perspective_camera",
    "get_perspective_camera",
    "get_quad_mesh",
    "get_regular_volume",
    "get_rotation_between",
    "get_tone_mapper",
    "get_tone_mapper",
    "get_triangle_mesh",
    "get_version",
    "GetCameraResult",
    "GetFramebufferResult",
    "GetLinearTransferFunctionResult",
    "GetObjectResult",
    "GetOrthographicCameraResult",
    "GetPanoramicCameraResult",
    "GetPerspectiveCameraResult",
    "GetRegularVolumeResult",
    "GetToneMapperResult",
    "GetTriangleMeshResult",
    "ImageOperation",
    "JpegChannel",
    "JsonRpcError",
    "JsonRpcErrorResponse",
    "JsonRpcId",
    "JsonRpcRequest",
    "JsonRpcResponse",
    "JsonRpcSuccessResponse",
    "LinearTransferFunction",
    "Object",
    "OrthographicCamera",
    "PanoramicCamera",
    "PerspectiveCamera",
    "PngChannel",
    "Quaternion",
    "read_framebuffer_as_exr",
    "read_framebuffer_as_jpeg",
    "read_framebuffer_as_png",
    "read_framebuffer",
    "RegularVolume",
    "remove_objects",
    "Response",
    "Rotation",
    "ServiceUnavailable",
    "Stereo",
    "stop_service",
    "ToneMapper",
    "TransferFunction",
    "update_camera",
    "update_framebuffer",
    "update_linear_transfer_function",
    "update_object",
    "update_orthographic_camera",
    "update_panoramic_camera",
    "update_perspective_camera",
    "update_regular_volume",
    "update_tone_mapper",
    "UpdateCameraParams",
    "UpdateFramebufferParams",
    "UpdateLinearTransferFunctionParams",
    "UpdateObjectParams",
    "UpdateOrthographicCameraParams",
    "UpdatePanoramicCameraParams",
    "UpdatePerspectiveCameraParams",
    "UpdateRegularVolumeParams",
    "UpdateToneMapperParams",
    "Vector2",
    "Vector3",
    "Vector4",
    "Version",
    "View",
    "Volume",
    "VolumeFilter",
    "VoxelType",
    "WebSocketError",
    "X",
    "Y",
    "Z",
]

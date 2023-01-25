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
Brayns utilities.

This subpackage contains all functionalities that are commonly used inside and
outside the main package and are not directly related to the Web API.

It includes helpers for algebra, imaging or coloring and the base exception.
"""

from .bounds import Bounds, deserialize_bounds, merge_bounds, serialize_bounds
from .color import Color3, Color4, parse_hex_color
from .error import Error
from .image import ImageFormat, Resolution, parse_image_format
from .json_schema import JsonSchema, JsonType, deserialize_schema, serialize_schema
from .plane_equation import PlaneEquation
from .quaternion import Quaternion
from .rotation import CameraRotation, ModelRotation, Rotation, euler
from .transform import Transform, deserialize_transform, serialize_transform
from .vector import Axis, Vector, Vector2, Vector3, componentwise_max, componentwise_min

__all__ = [
    "Axis",
    "Bounds",
    "CameraRotation",
    "Color3",
    "Color4",
    "componentwise_max",
    "componentwise_min",
    "deserialize_bounds",
    "deserialize_schema",
    "deserialize_transform",
    "Error",
    "euler",
    "ImageFormat",
    "JsonSchema",
    "JsonType",
    "merge_bounds",
    "ModelRotation",
    "parse_hex_color",
    "parse_image_format",
    "PlaneEquation",
    "Quaternion",
    "Resolution",
    "Rotation",
    "serialize_bounds",
    "serialize_schema",
    "serialize_transform",
    "Transform",
    "Vector",
    "Vector2",
    "Vector3",
]

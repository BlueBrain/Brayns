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
Brayns utilities.

This subpackage contains all functionalities that are commonly used inside and
outside the main package and are not directly related to the Web API.

It includes helpers for algebra, imaging or coloring and the base exception.
"""

from .bounds import *
from .color import *
from .exceptions import *
from .image import *
from .plane import *
from .transform import *
from .vector import *
from .view import *

__all__ = [
    'Axis',
    'Bounds',
    'Color3',
    'Color4',
    'componentwise_max',
    'componentwise_min',
    'Error',
    'euler',
    'Fovy',
    'ImageFormat',
    'merge_bounds',
    'ModelRotation',
    'parse_hex_color',
    'parse_image_format',
    'PlaneEquation',
    'Quaternion',
    'Resolution',
    'Rotation',
    'Transform',
    'Vector2',
    'Vector3',
    'View',
]

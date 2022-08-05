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

import math

from brayns.core.transform.quaternion import Quaternion
from brayns.core.vector.vector3 import Vector3


def euler_to_quaternion(euler: Vector3, degrees: bool = False) -> Quaternion:
    if degrees:
        euler = Vector3.unpack(math.radians(i) for i in euler)
    euler /= 2
    cx, cy, cz = Vector3.unpack(math.cos(i) for i in euler)
    sx, sy, sz = Vector3.unpack(math.sin(i) for i in euler)
    return Quaternion(
        sx * cy * cz - cx * sy * sz,
        cx * sy * cz + sx * cy * sz,
        cx * cy * sz - sx * sy * cz,
        cx * cy * cz + sx * sy * sz
    )

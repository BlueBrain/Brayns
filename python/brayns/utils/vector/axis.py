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

from .vector3 import Vector3


class Axis:
    """Helper class to store the principal axes of Brayns coordinate system."""

    @classmethod
    @property
    def x(cls) -> Vector3:
        return Vector3(1.0, 0.0, 0.0)

    @classmethod
    @property
    def right(cls) -> Vector3:
        return cls.x

    @classmethod
    @property
    def left(cls) -> Vector3:
        return -cls.right

    @classmethod
    @property
    def y(cls) -> Vector3:
        return Vector3(0.0, 1.0, 0.0)

    @classmethod
    @property
    def up(cls) -> Vector3:
        return cls.y

    @classmethod
    @property
    def down(cls) -> Vector3:
        return -cls.up

    @classmethod
    @property
    def z(cls) -> Vector3:
        return Vector3(0.0, 0.0, 1.0)

    @classmethod
    @property
    def front(cls) -> Vector3:
        return cls.z

    @classmethod
    @property
    def back(cls) -> Vector3:
        return -cls.front

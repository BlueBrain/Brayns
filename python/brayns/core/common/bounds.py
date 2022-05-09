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

from dataclasses import dataclass

from brayns.core.common.vector3 import Vector3


@dataclass
class Bounds:

    min: Vector3
    max: Vector3

    @staticmethod
    def deserialize(message: dict) -> 'Bounds':
        return Bounds(
            min=Vector3(*message['min']),
            max=Vector3(*message['max'])
        )

    @property
    def center(self) -> Vector3:
        return (self.min + self.max) / 2

    @property
    def size(self) -> Vector3:
        return self.max - self.min

    @property
    def width(self) -> float:
        return self.size.x

    @property
    def height(self) -> float:
        return self.size.y

    @property
    def depth(self) -> float:
        return self.size.z

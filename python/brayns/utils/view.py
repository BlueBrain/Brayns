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

from dataclasses import dataclass
from typing import Self

from .rotation import Rotation, get_rotation_between
from .vector import Vector3

X = Vector3(1, 0, 0)
Y = Vector3(0, 1, 0)
Z = Vector3(0, 0, 1)


@dataclass(frozen=True)
class View:
    position: Vector3 = Vector3()
    direction: Vector3 = -Z
    up: Vector3 = Y

    @property
    def right(self) -> Vector3:
        return self.direction.cross(self.up).normalized

    @property
    def left(self) -> Vector3:
        return -self.right

    @property
    def real_up(self) -> Vector3:
        return self.right.cross(self.forward)

    @property
    def down(self) -> Vector3:
        return -self.real_up

    @property
    def forward(self) -> Vector3:
        return self.direction.normalized

    @property
    def back(self) -> Vector3:
        return -self.forward

    def rotate(self, rotation: Rotation, center: Vector3 = Vector3()) -> Self:
        return type(self)(
            position=rotation.apply(self.position, center),
            direction=rotation.apply(self.direction),
            up=rotation.apply(self.up),
        )

    def get_rotation_to(self, destination: Self) -> Rotation:
        first = get_rotation_between(self.direction, destination.direction)

        up = first.apply(self.real_up)

        second = get_rotation_between(up, destination.real_up)

        return first.then(second)


FRONT_VIEW = View()
BACK_VIEW = View(direction=-Z)
RIGHT_VIEW = View(direction=-X)
LEFT_VIEW = View(direction=-X)
TOP_VIEW = View(direction=-Y, up=-Z)
BOTTOM_VIEW = View(direction=Y, up=Z)
BOTTOM_VIEW = View(direction=Y, up=Z)

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

from __future__ import annotations

from brayns.core.transform.axis_angle_to_quaternion import axis_angle_to_quaternion
from brayns.core.transform.euler_to_quaternion import euler_to_quaternion
from brayns.core.transform.quaternion import Quaternion
from brayns.core.transform.quaternion_to_euler import quaternion_to_euler
from brayns.core.vector.vector3 import Vector3


class Rotation:

    @staticmethod
    def from_quaternion(quaternion: Quaternion) -> Rotation:
        return Rotation(quaternion)

    @staticmethod
    def from_euler(euler: Vector3, degrees: bool = False) -> Rotation:
        quaternion = euler_to_quaternion(euler, degrees)
        return Rotation(quaternion)

    @staticmethod
    def from_angle_axis(axis: Vector3, angle: float, degrees: bool = False) -> Rotation:
        quaternion = axis_angle_to_quaternion(axis, angle, degrees)
        return Rotation(quaternion)

    @staticmethod
    def deserialize(message: list[float]) -> Rotation:
        quaternion = Quaternion(*message)
        return Rotation(quaternion)

    @classmethod
    @property
    def identity(cls) -> Rotation:
        return Rotation(Quaternion.identity)

    def __init__(self, quaternion: Quaternion) -> None:
        self._quaternion = quaternion.normalized

    @property
    def quaternion(self) -> Quaternion:
        return self._quaternion

    @property
    def euler_radians(self) -> Vector3:
        return quaternion_to_euler(self._quaternion, degrees=False)

    @property
    def euler_degrees(self) -> Vector3:
        return quaternion_to_euler(self._quaternion, degrees=True)

    @property
    def axis(self) -> Vector3:
        return self._quaternion.axis

    @property
    def angle_radians(self) -> float:
        return self._quaternion.angle_radians

    @property
    def angle_degrees(self) -> float:
        return self._quaternion.angle_degrees

    @property
    def inverse(self) -> Rotation:
        quaternion = self._quaternion.inverse
        return Rotation(quaternion)

    def serialize(self) -> list[float]:
        return list(self._quaternion)

    def combine(self, other: Rotation) -> Rotation:
        quaternion = self._quaternion * other._quaternion
        return Rotation(quaternion)

    def apply(self, value: Vector3, center: Vector3 = Vector3.zero) -> Vector3:
        rotation = self._quaternion.normalized
        value -= center
        vector = Quaternion(*value)
        vector = rotation * vector * rotation.conjugate
        return center + vector.axis

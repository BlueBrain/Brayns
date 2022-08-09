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

import math

from brayns.core.transform.quaternion import Quaternion
from brayns.core.vector.vector3 import Vector3


class Rotation:

    @staticmethod
    def from_quaternion(quaternion: Quaternion) -> Rotation:
        return Rotation(quaternion)

    @staticmethod
    def from_euler(euler: Vector3, degrees: bool = False) -> Rotation:
        quaternion = _euler_to_quaternion(euler, degrees)
        return Rotation(quaternion)

    @staticmethod
    def from_axis_angle(axis: Vector3, angle: float, degrees: bool = False) -> Rotation:
        quaternion = _axis_angle_to_quaternion(axis, angle, degrees)
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

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, Rotation):
            return False
        return self._quaternion == other.quaternion

    @property
    def quaternion(self) -> Quaternion:
        return self._quaternion

    @property
    def euler_radians(self) -> Vector3:
        return _quaternion_to_euler(self._quaternion, degrees=False)

    @property
    def euler_degrees(self) -> Vector3:
        return _quaternion_to_euler(self._quaternion, degrees=True)

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


def _axis_angle_to_quaternion(axis: Vector3, angle: float, degrees: bool) -> Quaternion:
    if degrees:
        angle = math.radians(angle)
    half_angle = angle / 2
    vector = axis.normalized * math.sin(half_angle)
    w = math.cos(half_angle)
    return Quaternion(vector.x, vector.y, vector.z, w)


def _euler_to_quaternion(euler: Vector3, degrees: bool) -> Quaternion:
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


def _quaternion_to_euler(quaternion: Quaternion, degrees: bool) -> Vector3:
    q = quaternion.normalized
    euler = Vector3(_get_x(q), _get_y(q), _get_z(q))
    if degrees:
        return Vector3.unpack(math.degrees(i) for i in euler)
    return euler


def _get_x(quaternion: Quaternion) -> float:
    q = quaternion
    sx_cy = 2 * (q.w * q.x + q.y * q.z)
    cx_cy = 1 - 2 * (q.x * q.x + q.y * q.y)
    return math.atan2(sx_cy, cx_cy)


def _get_y(quaternion: Quaternion) -> float:
    q = quaternion
    sy = 2 * (q.w * q.y - q.z * q.x)
    if abs(sy) >= 1:
        return math.copysign(math.pi / 2, sy)
    return math.asin(sy)


def _get_z(quaternion: Quaternion) -> float:
    q = quaternion
    sz_cy = 2 * (q.w * q.z + q.x * q.y)
    cz_cy = 1 - 2 * (q.y * q.y + q.z * q.z)
    return math.atan2(sz_cy, cz_cy)

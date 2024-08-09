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
import math
from typing import Self

from .quaternion import Quaternion
from .vector import Vector3


@dataclass(frozen=True)
class Rotation:
    quaternion: Quaternion = Quaternion()

    @property
    def euler_radians(self) -> Vector3:
        return _quaternion_to_euler(self.quaternion)

    @property
    def euler_degrees(self) -> Vector3:
        return Vector3.unpack(math.degrees(i) for i in self.euler_radians)

    @property
    def axis(self) -> Vector3:
        return self.quaternion.axis

    @property
    def angle_radians(self) -> float:
        return self.quaternion.angle_radians

    @property
    def angle_degrees(self) -> float:
        return self.quaternion.angle_degrees

    @property
    def inverse(self) -> Self:
        return type(self)(self.quaternion.conjugate)

    def then(self, other: Self) -> Self:
        return type(self)(other.quaternion * self.quaternion)

    def apply(self, value: Vector3, center: Vector3 = Vector3()) -> Vector3:
        value -= center

        quaternion = self.quaternion
        vector = Quaternion(*value, 0)

        vector = quaternion * vector * quaternion.conjugate

        return center + vector.xyz


def euler(x: float, y: float, z: float, degrees: bool = False) -> Rotation:
    angles = Vector3(x, y, z)

    if degrees:
        angles = Vector3.unpack(math.radians(i) for i in angles)

    quaternion = _euler_to_quaternion(angles)

    return Rotation(quaternion)


def axis_angle(x: float, y: float, z: float, angle: float, degrees: bool = False) -> Rotation:
    axis = Vector3(x, y, z)

    if degrees:
        angle = math.radians(angle)

    quaternion = _axis_angle_to_quaternion(axis, angle)

    return Rotation(quaternion)


def get_rotation_between(source: Vector3, destination: Vector3) -> Rotation:
    quaternion = _get_quaternion_between(source, destination)

    return Rotation(quaternion)


def _axis_angle_to_quaternion(axis: Vector3, angle: float) -> Quaternion:
    half_angle = angle / 2

    xyz = axis.normalized * math.sin(half_angle)
    w = math.cos(half_angle)

    return Quaternion(*xyz, w)


def _euler_to_quaternion(euler: Vector3) -> Quaternion:
    half_angles = euler / 2

    cx, cy, cz = Vector3.unpack(math.cos(i) for i in half_angles)
    sx, sy, sz = Vector3.unpack(math.sin(i) for i in half_angles)

    return Quaternion(
        sx * cy * cz - cx * sy * sz,
        cx * sy * cz + sx * cy * sz,
        cx * cy * sz - sx * sy * cz,
        cx * cy * cz + sx * sy * sz,
    )


def _quaternion_to_euler(q: Quaternion) -> Vector3:
    return Vector3(_get_x(q), _get_y(q), _get_z(q))


def _get_x(q: Quaternion) -> float:
    sx_cy = 2 * (q.w * q.x + q.y * q.z)
    cx_cy = 1 - 2 * (q.x * q.x + q.y * q.y)

    return math.atan2(sx_cy, cx_cy)


def _get_y(q: Quaternion) -> float:
    sy = 2 * (q.w * q.y - q.z * q.x)

    if abs(sy) >= 1:
        return math.copysign(math.pi / 2, sy)

    return math.asin(sy)


def _get_z(q: Quaternion) -> float:
    sz_cy = 2 * (q.w * q.z + q.x * q.y)
    cz_cy = 1 - 2 * (q.y * q.y + q.z * q.z)

    return math.atan2(sz_cy, cz_cy)


def _get_quaternion_between(u: Vector3, v: Vector3) -> Quaternion:
    angle = _get_angle_between(u, v)

    if angle == 0:
        return Quaternion()

    if angle == math.pi:
        axis = _get_orthogonal(u).normalized
        return Quaternion(axis.x, axis.y, axis.z, 0)

    axis = u.cross(v)

    return _axis_angle_to_quaternion(axis, angle)


def _get_angle_between(u: Vector3, v: Vector3) -> float:
    u = u.normalized
    v = v.normalized

    return math.acos(u.dot(v))


def _get_orthogonal(v: Vector3) -> Vector3:
    if v.x != 0:
        x = (v.y + v.z) / v.x
        return Vector3(x, -1, -1)

    if v.y != 0:
        y = (v.x + v.z) / v.y
        return Vector3(-1, y, -1)

    if v.z != 0:
        z = (v.x + v.y) / v.z
        return Vector3(-1, -1, z)

    raise ValueError(v)

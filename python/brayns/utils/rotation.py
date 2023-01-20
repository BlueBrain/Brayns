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

from __future__ import annotations

import math

from .quaternion import Quaternion
from .vector import Vector3


class Rotation:
    """Arbitrary 3D rotation using normalized quaternion internally."""

    @staticmethod
    def from_quaternion(quaternion: Quaternion) -> Rotation:
        """Construct a rotation using a quaternion (will be normalized).

        :param quaternion: Quaternion representing rotation.
        :type quaternion: Quaternion
        :return: Rotation object.
        :rtype: Rotation
        """
        return Rotation(quaternion)

    @staticmethod
    def from_euler(euler: Vector3, degrees: bool = False) -> Rotation:
        """Construct a rotation from euler angle in XYZ order.

        :param euler: XYZ euler angles.
        :type euler: Vector3
        :param degrees: Euler are provided in degrees, defaults to False.
        :type degrees: bool, optional
        :return: Rotation object.
        :rtype: Rotation
        """
        if degrees:
            euler = Vector3.unpack(math.radians(i) for i in euler)
        quaternion = _euler_to_quaternion(euler)
        return Rotation(quaternion)

    @staticmethod
    def from_axis_angle(axis: Vector3, angle: float, degrees: bool = False) -> Rotation:
        """Construct a rotation of angle around axis.

        Axis must not be zero.

        :param axis: Rotation axis (will be normalized).
        :type axis: Vector3
        :param angle: Rotation angle.
        :type angle: float
        :param degrees: Angle is provided in degrees, defaults to False.
        :type degrees: bool, optional
        :return: Rotation object.
        :rtype: Rotation
        """
        if degrees:
            angle = math.radians(angle)
        quaternion = _axis_angle_to_quaternion(axis, angle)
        return Rotation(quaternion)

    @staticmethod
    def between(source: Vector3, destination: Vector3) -> Rotation:
        """Compute the minimal arc rotation between two vectors.

        Vectors must not be zero.

        :param source: Original direction.
        :type source: Vector3
        :param destination: Final direction.
        :type destination: Vector3
        :return: Rotation object.
        :rtype: Rotation
        """
        quaternion = _get_quaternion_between(source, destination)
        return Rotation.from_quaternion(quaternion)

    @classmethod
    @property
    def identity(cls) -> Rotation:
        """Construct a rotation with no effects.

        :return: Identity rotation.
        :rtype: Rotation
        """
        return Rotation(Quaternion.identity)

    def __init__(self, quaternion: Quaternion) -> None:
        """Direct construction, prefer from_quaternion."""
        self._quaternion = quaternion.normalized

    def __eq__(self, other: object) -> bool:
        """Equality check using internal quaternion."""
        if not isinstance(other, Rotation):
            return False
        return self._quaternion == other.quaternion

    @property
    def quaternion(self) -> Quaternion:
        """Get rotation as normalized quaternion.

        :return: Normalized quaternion.
        :rtype: Quaternion
        """
        return self._quaternion

    @property
    def euler_radians(self) -> Vector3:
        """Get rotation as euler angles XYZ in radians.

        :return: Euler angles.
        :rtype: Vector3
        """
        return _quaternion_to_euler(self._quaternion)

    @property
    def euler_degrees(self) -> Vector3:
        """Get rotation as euler angles XYZ in degrees.

        :return: Euler angles.
        :rtype: Vector3
        """
        return Vector3.unpack(math.degrees(i) for i in self.euler_radians)

    @property
    def axis(self) -> Vector3:
        """Get rotation axis.

        :return: Normalized rotation axis.
        :rtype: Vector3
        """
        return self._quaternion.axis

    @property
    def angle_radians(self) -> float:
        """Get rotation angle in radians.

        :return: Rotation angle.
        :rtype: Vector3
        """
        return self._quaternion.angle_radians

    @property
    def angle_degrees(self) -> float:
        """Get rotation angle in degrees.

        :return: Rotation angle.
        :rtype: Vector3
        """
        return self._quaternion.angle_degrees

    @property
    def inverse(self) -> Rotation:
        """Get rotation such as self * self.inverse == Rotation.identity.

        :return: Inverse rotation.
        :rtype: Rotation
        """
        quaternion = self._quaternion.inverse
        return Rotation(quaternion)

    def then(self, other: Rotation) -> Rotation:
        """Combine rotations to be equivalent to self then other.

        :param other: Rotation to apply after self.
        :type other: Rotation
        :return: Rotation object.
        :rtype: Rotation
        """
        quaternion = other._quaternion * self._quaternion
        return Rotation(quaternion)

    def apply(self, value: Vector3, center: Vector3 = Vector3.zero) -> Vector3:
        """Apply rotation on value around center.

        :param value: Vector to rotate.
        :type value: Vector3
        :param center: Rotation center, defaults to Vector3.zero.
        :type center: Vector3, optional
        :return: Rotated value.
        :rtype: Vector3
        """
        quaternion = self._quaternion
        value -= center
        vector = Quaternion(*value)
        vector = quaternion * vector * quaternion.conjugate
        return center + vector.xyz


def euler(x: float, y: float, z: float, degrees: bool = False) -> Rotation:
    """Shortcut to build a rotation from euler angles.

    :param x: X rotation.
    :type x: float
    :param y:  Y rotation.
    :type y: float
    :param z:  Z rotation.
    :type z: float
    :param degrees: Wether given angles are in degrees, defaults to False
    :type degrees: bool, optional
    :return: Rotation corresponding to angles.
    :rtype: Rotation
    """
    angles = Vector3(x, y, z)
    return Rotation.from_euler(angles, degrees)


class ModelRotation:
    """Helper class to store model rotations to reach different views.

    All rotations are relative to front view (X-right, Y-up, Z-front).
    """

    @classmethod
    @property
    def front(cls) -> Rotation:
        return Rotation.identity

    @classmethod
    @property
    def back(cls) -> Rotation:
        return euler(0, 180, 0, degrees=True)

    @classmethod
    @property
    def top(cls) -> Rotation:
        return euler(90, 0, 0, degrees=True)

    @classmethod
    @property
    def bottom(cls) -> Rotation:
        return euler(-90, 0, 0, degrees=True)

    @classmethod
    @property
    def right(cls) -> Rotation:
        return euler(0, -90, 0, degrees=True)

    @classmethod
    @property
    def left(cls) -> Rotation:
        return euler(0, 90, 0, degrees=True)


class CameraRotation:
    """Helper class to store camera rotations to reach different views.

    All rotations are relative to front view (X-right, Y-up, Z-front).
    """

    @classmethod
    @property
    def front(cls) -> Rotation:
        return Rotation.identity

    @classmethod
    @property
    def back(cls) -> Rotation:
        return euler(0, 180, 0, degrees=True)

    @classmethod
    @property
    def top(cls) -> Rotation:
        return euler(-90, 0, 0, degrees=True)

    @classmethod
    @property
    def bottom(cls) -> Rotation:
        return euler(90, 0, 0, degrees=True)

    @classmethod
    @property
    def right(cls) -> Rotation:
        return euler(0, 90, 0, degrees=True)

    @classmethod
    @property
    def left(cls) -> Rotation:
        return euler(0, -90, 0, degrees=True)


def _axis_angle_to_quaternion(axis: Vector3, angle: float) -> Quaternion:
    axis = axis.normalized
    half_angle = angle / 2
    vector = axis * math.sin(half_angle)
    w = math.cos(half_angle)
    return Quaternion(vector.x, vector.y, vector.z, w)


def _euler_to_quaternion(euler: Vector3) -> Quaternion:
    euler /= 2
    cx, cy, cz = Vector3.unpack(math.cos(i) for i in euler)
    sx, sy, sz = Vector3.unpack(math.sin(i) for i in euler)
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
        return Quaternion.identity
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

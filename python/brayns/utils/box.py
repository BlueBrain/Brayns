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

from collections.abc import Sequence
from typing import Generic, NamedTuple, Self, TypeVar

from .vector import Vector, Vector2, Vector3

T = TypeVar("T", bound=Vector)


class Box(NamedTuple, Generic[T]):
    min: T
    max: T

    @classmethod
    def vector_type(cls) -> type[T]:
        raise NotImplementedError()

    @classmethod
    def full(cls, value: float) -> Self:
        t = cls.vector_type()
        upper = t.full(value)
        lower = -upper

        return cls(lower, upper)

    @classmethod
    def zeros(cls) -> Self:
        return cls.full(0)

    @classmethod
    def ones(cls) -> Self:
        return cls.full(1)

    @classmethod
    def at(cls, center: T, size: T) -> Self:
        half_size = size / 2

        return cls(
            min=center - half_size,
            max=center + half_size,
        )

    @classmethod
    def of(cls, size: T) -> Self:
        t = cls.vector_type()
        center = t.zeros()

        return cls.at(center, size)

    @classmethod
    def around(cls, points: Sequence[T]) -> Self:
        t = cls.vector_type()

        return cls(
            min=t.componentwise_min(points),
            max=t.componentwise_max(points),
        )

    @classmethod
    def merge(cls, boxes: Sequence[Self]) -> Self:
        t = cls.vector_type()

        return cls(
            min=t.componentwise_min([box.min for box in boxes]),
            max=t.componentwise_max([box.max for box in boxes]),
        )

    def __contains__(self, point: T) -> bool:
        if not all(i <= limit for i, limit in zip(point, self.max)):
            return False

        if not all(i >= limit for i, limit in zip(point, self.min)):
            return False

        return True

    def __bool__(self) -> bool:
        return all(i < j for i, j in zip(self.min, self.max))

    @property
    def center(self) -> T:
        return (self.min + self.max) / 2

    @property
    def size(self) -> T:
        return self.max - self.min

    def translate(self, translation: T) -> Self:
        return type(self)(
            min=self.min + translation,
            max=self.max + translation,
        )

    def scale(self, value: T) -> Self:
        return type(self)(
            min=value * self.min,
            max=value * self.max,
        )


class Box2(Box[Vector2]):
    @classmethod
    def vector_type(cls) -> type[Vector2]:
        return Vector2

    @property
    def width(self) -> float:
        return self.size.x

    @property
    def height(self) -> float:
        return self.size.y

    @property
    def corners(self) -> list[Vector2]:
        return [
            self.min,
            Vector2(self.min.x, self.max.y),
            Vector2(self.max.x, self.min.y),
            self.max,
        ]


class Box3(Box[Vector3]):
    @classmethod
    def vector_type(cls) -> type[Vector3]:
        return Vector3

    @classmethod
    def extrude(cls, xy: Box2, zmin: float = 0.0, zmax: float = 0.0) -> Self:
        return cls(
            min=Vector3(*xy.min, zmin),
            max=Vector3(*xy.max, zmax),
        )

    @property
    def width(self) -> float:
        return self.size.x

    @property
    def height(self) -> float:
        return self.size.y

    @property
    def depth(self) -> float:
        return self.size.z

    @property
    def xy(self) -> Box2:
        return Box2(self.min.xy, self.max.xy)

    @property
    def corners(self) -> list[Vector3]:
        return [
            self.min,
            Vector3(self.min.x, self.min.y, self.max.z),
            Vector3(self.min.x, self.max.y, self.min.z),
            Vector3(self.min.x, self.max.y, self.max.z),
            Vector3(self.max.x, self.min.y, self.min.z),
            Vector3(self.max.x, self.min.y, self.max.z),
            Vector3(self.max.x, self.max.y, self.min.z),
            self.max,
        ]

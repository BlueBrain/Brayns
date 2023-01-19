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

from abc import ABC, abstractmethod
from dataclasses import dataclass, field
from typing import Any, TypeVar

from brayns.network import Instance
from brayns.utils import Bounds, Color4, PlaneEquation, Vector3, serialize_bounds

from .model import Model, deserialize_model

T = TypeVar("T", bound="Geometry")


@dataclass
class Geometry(ABC):
    """Base class of all geometry types.

    The color attribute is white by default and is not in the constructor, use
    with_color to chain the color settings with the construction.

    :param color: Geometry color, defaults to white.
    :type color: Color4
    """

    color: Color4 = field(default=Color4.white, init=False)

    @classmethod
    @property
    @abstractmethod
    def method(cls) -> str:
        """JSON-RPC method to add geometries of the derived type.

        :return: JSON-RPC method.
        :rtype: str
        """
        pass

    @abstractmethod
    def get_additional_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        pass

    def with_color(self: T, color: Color4) -> T:
        """Helper method to quickly change the color of the geometry.

        Example: ``sphere = brayns.Sphere(1).with_color(my_color)``.

        :param color: Geometry color.
        :type color: Color4
        :return: self
        :rtype: type(self)
        """
        self.color = color
        return self

    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            "geometry": self.get_additional_properties(),
            "color": list(self.color),
        }


@dataclass
class BoundedPlane(Geometry):
    """Axis-aligned bounded plane.

    :param equation: Plane equation coefficients.
    :type equation: PlaneEquation
    :param bounds: Axis aligned bounds to limit the plane.
    :type bounds: Bounds
    """

    equation: PlaneEquation
    bounds: Bounds

    def __new__(cls, equation: PlaneEquation, bounds: Bounds) -> BoundedPlane:
        if min(bounds.size) <= 0:
            raise ValueError("All bound dimensions must be greater than 0")
        return object.__new__(cls)

    @classmethod
    @property
    def method(cls) -> str:
        """JSON-RPC method to add bounded planes.

        :return: JSON-RPC method.
        :rtype: str
        """
        return "add-bounded-planes"

    def get_additional_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            "coefficients": list(self.equation),
            "bounds": serialize_bounds(self.bounds),
        }


@dataclass
class Box(Geometry):
    """3D box.

    :param min: Minimum XYZ.
    :type min: Vector3
    :param max: Maximum XYZ.
    :type max: Vector3
    """

    min: Vector3
    max: Vector3

    @classmethod
    @property
    def method(cls) -> str:
        """JSON-RPC method to add boxes.

        :return: JSON-RPC method.
        :rtype: str
        """
        return "add-boxes"

    def get_additional_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return serialize_bounds(self)


@dataclass
class Capsule(Geometry):
    """Capsule (cylinder with slope and half spheres at extremities).

    :param start_point: Cylinder base XYZ.
    :type start_point: Vector3
    :param start_radius: Cylinder and sphere base radius.
    :type start_radius: float
    :param end_point: Cylinder end XYZ.
    :type end_point: Vector3
    :param end_radius: Cylinder and sphere end radius.
    :type end_radius: float
    """

    start_point: Vector3
    start_radius: float
    end_point: Vector3
    end_radius: float

    @classmethod
    @property
    def method(cls) -> str:
        """JSON-RPC method to add capsules.

        :return: JSON-RPC method.
        :rtype: str
        """
        return "add-capsules"

    def get_additional_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            "p0": list(self.start_point),
            "r0": self.start_radius,
            "p1": list(self.end_point),
            "r1": self.end_radius,
        }


@dataclass
class Plane(Geometry):
    """Infinite plane.

    :param equation: Plane equation coefficients.
    :type equation: PlaneEquation
    """

    equation: PlaneEquation

    @classmethod
    @property
    def method(cls) -> str:
        """JSON-RPC method to add planes.

        :return: JSON-RPC method.
        :rtype: str
        """
        return "add-planes"

    def get_additional_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            "coefficients": list(self.equation),
        }


@dataclass
class Sphere(Geometry):
    """Sphere with radius and position.

    :param radius: Radius.
    :type radius: float
    :param center: Center XYZ, defaults to origin.
    :type center: Vector3
    """

    radius: float
    center: Vector3 = Vector3.zero

    @classmethod
    @property
    def method(cls) -> str:
        """JSON-RPC method to add spheres.

        :return: JSON-RPC method.
        :rtype: str
        """
        return "add-spheres"

    def get_additional_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            "center": list(self.center),
            "radius": self.radius,
        }


def add_geometries(instance: Instance, geometries: list[T]) -> Model:
    """Create a model from a list of geometries.

    All geometries must have the same type.

    Model witout geometries are not supported.

    :param instance: Instance.
    :type instance: Instance
    :param geometries: Geometries to add (boxes, capsules, etc...).
    :type geometries: list[T]
    :raises ValueError: List is empty.
    :return: Model created from the geometries.
    :rtype: Model
    """
    if not geometries:
        raise ValueError("Cannot create a model with no geometries")
    method = geometries[0].method
    params = [geometry.get_properties() for geometry in geometries]
    result = instance.request(method, params)
    return deserialize_model(result)

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

from abc import ABC, abstractmethod
from dataclasses import dataclass
from typing import Any, TypeVar

from brayns.network import Instance
from brayns.utils import Bounds, PlaneEquation, Vector3, serialize_bounds

from .model import Model, deserialize_model


@dataclass
class ClippingGeometry(ABC):
    """Base class used for clipping geometries."""

    @classmethod
    @property
    @abstractmethod
    def method(cls) -> str:
        """Get the JSON-RCP method to add the clipping geometry.

        :return: JSON-RPC method.
        :rtype: str
        """
        pass

    @abstractmethod
    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        pass


@dataclass
class ClippingBoundedPlane(ClippingGeometry):
    """Clip plane spatially limited by an axis-aligned bounding box.

    :param equation: Plane equation coefficients.
    :type equation: PlaneEquation
    :param bounds: Axis-aligned bounds to limit the plane.
    :type bounds: Bounds
    """

    equation: PlaneEquation
    bounds: Bounds

    @classmethod
    @property
    def method(cls) -> str:
        """Get the JSON-RPC method to add clipping bounded planes.

        :return: JSON-RPC method.
        :rtype: str
        """
        return "add-clipping-bounded-planes"

    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            "coefficients": list(self.equation),
            "bounds": serialize_bounds(self.bounds),
        }


@dataclass
class ClippingBox(ClippingGeometry):
    """Clipping box.

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
        """JSON-RPC method to add clipping boxes.

        :return: JSON-RPC method.
        :rtype: str
        """
        return "add-clipping-boxes"

    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return serialize_bounds(self)


@dataclass
class ClippingCapsule(ClippingGeometry):
    """Clipping capsule (cylinder with slope and half spheres at extremities).

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
        return "add-clipping-capsules"

    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            "p0": list(self.start_point),
            "r0": self.start_radius,
            "p1": list(self.end_point),
            "r1": self.end_radius,
        }


@dataclass
class ClippingPlane(ClippingGeometry):
    """Infinite clipping plane.

    :param equation: Plane equation coefficients.
    :type equation: PlaneEquation
    """

    equation: PlaneEquation

    @classmethod
    @property
    def method(cls) -> str:
        """JSON-RPC method to add clipping planes.

        :return: JSON-RPC method.
        :rtype: str
        """
        return "add-clipping-planes"

    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            "coefficients": list(self.equation),
        }


@dataclass
class ClippingSphere(ClippingGeometry):
    """Clipping sphere.

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
        """JSON-RPC method to add clipping spheres.

        :return: JSON-RPC method.
        :rtype: str
        """
        return "add-clipping-spheres"

    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            "center": list(self.center),
            "radius": self.radius,
        }


@dataclass
class ClipPlane(ClippingGeometry):
    """Clip plane.

    Everything below the given plane is clipped (see PlaneEquation).

    Multiple clip planes can be combined to slice a model in complex ways.

    :param equation: Plane equation coefficients.
    :type equation: PlaneEquation
    """

    equation: PlaneEquation

    @classmethod
    @property
    def method(cls) -> str:
        """Get the JSON-RPC method to add the clipping geometry.

        :return: JSON-RPC method.
        :rtype: str
        """
        return "add-clip-plane"

    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            "coefficients": list(self.equation),
        }


T = TypeVar("T", bound=ClippingGeometry)


def add_clipping_geometries(instance: Instance, cliping_geometries: list[T]) -> Model:
    """Create a model from a list of clipping geometries.

    All geometries must have the same type.

    Model witout geometries are not supported.

    :param instance: Instance.
    :type instance: Instance
    :param cliping_geometries: Clipping geometries to add (boxes, capsules, etc...).
    :type cliping_geometries: list[T]
    :raises ValueError: List is empty.
    :return: Model created from the geometries.
    :rtype: Model
    """
    if not cliping_geometries:
        raise ValueError("Cannot create a model with no clipping geometries")
    method = cliping_geometries[0].method
    params = [geometry.get_properties() for geometry in cliping_geometries]
    result = instance.request(method, params)
    return deserialize_model(result)


def add_clipping_geometry(instance: Instance, geometry: ClippingGeometry) -> Model:
    """Add a clipping geometry to the given instance and return its model.

    :param instance: Instance.
    :type instance: Instance
    :param geometry: Clipping geometry to add.
    :type geometry: ClippingGeometry
    :return: Clipping model.
    :rtype: Model
    """
    method = geometry.method
    params = geometry.get_properties()
    result = instance.request(method, params)
    return deserialize_model(result)


def clear_clipping_geometries(instance: Instance) -> None:
    """Clear all clipping geometries from the given instance.

    :param instance: Instance.
    :type instance: Instance
    """
    instance.request("clear-clip-planes")

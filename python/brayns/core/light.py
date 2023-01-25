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
from typing import Any

from brayns.network import Instance
from brayns.utils import Axis, Color3, Vector3

from .model import Model, deserialize_model


@dataclass
class Light(ABC):
    """Base class for all light types.

    :param intensity: Light intensity, defaults to 1.
    :type intensity: float, optional
    :param color: Light color, defaults to white.
    :type color: Color3, optional
    """

    intensity: float = 1.0
    color: Color3 = Color3.white

    @classmethod
    @property
    @abstractmethod
    def name(cls) -> str:
        """Get the light name.

        :return: Light name.
        :rtype: str
        """
        pass

    @abstractmethod
    def get_additional_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        pass

    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            "intensity": self.intensity,
            "color": list(self.color),
            **self.get_additional_properties(),
        }


@dataclass
class AmbientLight(Light):
    """Ambient light with no particular properties."""

    @classmethod
    @property
    def name(cls) -> str:
        """Get the light name.

        :return: Light name.
        :rtype: str
        """
        return "ambient"

    def get_additional_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {}


@dataclass
class DirectionalLight(Light):
    """Light shining in a given direction.

    :param direction: Light emission direction, defaults to -Z.
    :type direction: Vector3, optional
    """

    direction: Vector3 = Axis.back

    @classmethod
    @property
    def name(cls) -> str:
        """Get the light name.

        :return: Light name.
        :rtype: str
        """
        return "directional"

    def get_additional_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            "direction": list(self.direction),
        }


@dataclass
class QuadLight(Light):
    """Rectangular light.

    Emission direction is the positive side (see emission_direction).

    :param bottom_left: Bottom left corner, defaults to origin.
    :type bottom_left: Vector3
    :param edge1: First edge, defaults to +X.
    :type edge1: Vector3, optional
    :param edge2: Second edge, defaults to +Y.
    :type edge2: Vector3, optional
    """

    bottom_left: Vector3 = Vector3.zero
    edge1: Vector3 = Axis.right
    edge2: Vector3 = Axis.up

    @classmethod
    @property
    def name(cls) -> str:
        """Return light name.

        :return: Light name.
        :rtype: str
        """
        return "quad"

    @property
    def emission_direction(self) -> Vector3:
        """Get the emission direction of the light.

        Equal to edge1 x edge2 normalized.

        :return: Emission direction.
        :rtype: Vector3
        """
        return self.edge1.cross(self.edge2).normalized

    def get_additional_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            "position": list(self.bottom_left),
            "edge1": list(self.edge1),
            "edge2": list(self.edge2),
        }


def add_light(instance: Instance, light: Light) -> Model:
    """Add a given light to an instance and return its model.

    :param instance: Instance.
    :type instance: Instance
    :param light: Light to add.
    :type light: Light
    :return: Light model.
    :rtype: Model
    """
    name = light.name
    params = light.get_properties()
    result = instance.request(f"add-light-{name}", params)
    return deserialize_model(result)


def clear_lights(instance: Instance) -> None:
    """Remove all lights from an instance.

    :param instance: Instance.
    :type instance: Instance
    """
    instance.request("clear-lights")

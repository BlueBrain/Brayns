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
from brayns.utils import Color3

T = TypeVar("T", bound="Material")


@dataclass
class Material(ABC):
    """Base class for all material types.

    Material are applied on model to change their aspect (but not their color).
    """

    @classmethod
    @property
    @abstractmethod
    def name(cls) -> str:
        """Get the material name.

        :return: Material name
        :rtype: str
        """
        pass

    @abstractmethod
    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        pass

    @abstractmethod
    def update_properties(self, message: dict[str, Any]) -> None:
        """Low level API to deserialize from JSON."""
        pass

    @classmethod
    def from_properties(cls: type[T], message: dict[str, Any]) -> T:
        """Low level API to deserialize from JSON."""
        material = cls()
        material.update_properties(message)
        return material


@dataclass
class CarPaintMaterial(Material):
    """Car paint material.

    :param flake_density: Flake density (0-1), defaults to no flakes.
    :type flake_density: float, optional
    """

    flake_density: float = 0.0

    @classmethod
    @property
    def name(cls) -> str:
        """Get the material name.

        :return: Material name
        :rtype: str
        """
        return "carpaint"

    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {"flake_density": self.flake_density}

    def update_properties(self, message: dict[str, Any]) -> None:
        """Low level API to deserialize from JSON."""
        self.flake_density = message["flake_density"]


@dataclass
class EmissiveMaterial(Material):
    """Emissive material.

    :param intensity: Light emission intensity, defaults to 1.
    :type intensity: float, optional
    :param color: Light color, defaults to white.
    :type color: Color3, optional
    """

    intensity: float = 1.0
    color: Color3 = Color3.white

    @classmethod
    @property
    def name(cls) -> str:
        """Get the material name.

        :return: Material name
        :rtype: str
        """
        return "emissive"

    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            "intensity": self.intensity,
            "color": list(self.color),
        }

    def update_properties(self, message: dict[str, Any]) -> None:
        """Low level API to deserialize from JSON."""
        self.intensity = message["intensity"]
        self.color = Color3.unpack(message["color"])


@dataclass
class GlassMaterial(Material):
    """Glass material.

    :param refraction_index: Refraction index, defaults to 1.5.
    :type refraction_index: float, optional
    """

    refraction_index: float = 1.5

    @classmethod
    @property
    def name(cls) -> str:
        """Get the material name.

        :return: Material name
        :rtype: str
        """
        return "glass"

    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {"index_of_refraction": self.refraction_index}

    def update_properties(self, message: dict[str, Any]) -> None:
        """Low level API to deserialize from JSON."""
        self.refraction_index = message["index_of_refraction"]


@dataclass
class MatteMaterial(Material):
    """Matte material.

    :param opacity: Opacity (0-1), defaults to fully opaque.
    :type opacity: float, optional
    """

    opacity: float = 1.0

    @classmethod
    @property
    def name(cls) -> str:
        """Get the material name.

        :return: Material name
        :rtype: str
        """
        return "matte"

    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {"opacity": self.opacity}

    def update_properties(self, message: dict[str, Any]) -> None:
        """Low level API to deserialize from JSON."""
        self.opacity = message["opacity"]


@dataclass
class MetalMaterial(Material):
    """Metal material.

    :param roughness: Roughness of the metal, defaults to 1.
    :type roughness: float, optional
    """

    roughness: float = 1.0

    @classmethod
    @property
    def name(cls) -> str:
        """Get the material name.

        :return: Material name
        :rtype: str
        """
        return "metal"

    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {"roughness": self.roughness}

    def update_properties(self, message: dict[str, Any]) -> None:
        """Low level API to deserialize from JSON."""
        self.roughness = message["roughness"]


@dataclass
class PhongMaterial(Material):
    """Phong material used by default, works with all renderers.

    :param opacity: Opacity (0-1), defaults to fully opaque.
    :type opacity: float, optional
    """

    opacity: float = 1.0

    @classmethod
    @property
    def name(cls) -> str:
        """Get the material name.

        :return: Material name
        :rtype: str
        """
        return "phong"

    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {"opacity": self.opacity}

    def update_properties(self, message: dict[str, Any]) -> None:
        """Low level API to deserialize from JSON."""
        self.opacity = message["opacity"]


@dataclass
class PlasticMaterial(Material):
    """Plastic material.

    :param opacity: Opacity (0-1), defaults to fully opaque.
    :type opacity: float, optional
    """

    opacity: float = 1.0

    @classmethod
    @property
    def name(cls) -> str:
        """Get the material name.

        :return: Material name
        :rtype: str
        """
        return "plastic"

    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {"opacity": self.opacity}

    def update_properties(self, message: dict[str, Any]) -> None:
        """Low level API to deserialize from JSON."""
        self.opacity = message["opacity"]


def get_material_name(instance: Instance, model_id: int) -> str:
    """Get the name of the material applied on the given model.

    Can be used to check that a given material is applied on a model:

    :param instance: Instance.
    :type instance: Instance
    :param model_id: ID of the model to check.
    :type model_id: int
    :return: Material name.
    :rtype: str
    """
    params = {"id": model_id}
    return instance.request("get-material-type", params)


def get_material(instance: Instance, model_id: int, material_type: type[T]) -> T:
    """Get the material applied on the given model.

    ``material_type`` must be the current model material type.

    :param instance: Instance.
    :type instance: Instance
    :param model_id: ID of the model.
    :type model_id: int
    :param material_type: Material type.
    :type material_type: type[T]
    :return: Material applied on ``model``.
    :rtype: T
    """
    name = material_type.name
    params = {"id": model_id}
    result = instance.request(f"get-material-{name}", params)
    return material_type.from_properties(result)


def set_material(instance: Instance, model_id: int, material: Material) -> None:
    """Apply the given material to the given model.

    :param instance: Instance.
    :type instance: Instance
    :param model_id: Model ID.
    :type model_id: int
    :param material: Material to apply on model.
    :type material: Material
    """
    name = material.name
    params = {
        "model_id": model_id,
        "material": material.get_properties(),
    }
    instance.request(f"set-material-{name}", params)

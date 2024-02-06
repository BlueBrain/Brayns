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
class GhostMaterial(Material):
    """Ghost material."""

    @classmethod
    @property
    def name(cls) -> str:
        """Get the material name.

        :return: Material name
        :rtype: str
        """
        return "ghost"

    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {}

    def update_properties(self, message: dict[str, Any]) -> None:
        """Low level API to deserialize from JSON."""
        pass


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


@dataclass
class PrincipledMaterial(Material):
    """Principled material.

    :param edge_color: Surface edge tint for metallic surfaces.
    :type edge_color: float, optional
    :param metallic: Alpha parameter between dielectric and metallic.
    :type metallic: float, optional
    :param diffuse: Diffuse reflection weight.
    :type diffuse: float, optional
    :param specular: Specular reflection/transmission weight.
    :type specular: float, optional
    :param ior: Dielectric index of refraction.
    :type ior: float, optional
    :param transmission: Specular transmission weight.
    :type transmission: float, optional
    :param transmission_color: Transmission attenuation color.
    :type transmission_color: Color3, optional
    :param transmission_depth: Surface distance for pure transmission color.
    :type transmission_depth: float, optional
    :param roughness: Diffuse and specular reflection roughness.
    :type roughness: float, optional
    :param anisotropy: Specular anisotropy reflection weight.
    :type anisotropy: float, optional
    :param anisotropy_rotation: Rotation of the specular anisotropy reflection.
    :type anisotropy_rotation: float, optional
    :param thin: Specified wether the object is solid or thin (hollow).
    :type thin: bool, optional
    :param thickness: Thickness of the object if thin = True.
    :type thickness: float, optional
    :param back_light: Weight of reflection and transmission if thin = True.
    :type back_light: float, optional
    :param coat: Clear coat weight.
    :type coat: float, optional
    :param coat_ior: Clear coat index of refraction.
    :type coat_ior: float, optional
    :param coat_color: Clear coat color.
    :type coat_color: Color3, optional
    :param coat_thickness: Clear coat thickness.
    :type coat_thickness: float, optional
    :param coat_roughness: Clear coat diffuse/specular reflection roughness.
    :type coat_roughness: float, optional
    :param sheen: Sheen effect weight.
    :type sheen: float, optional
    :param sheen_color: Sheen color.
    :type sheen_color: Color3, optional
    :param sheen_tint: Alpha from white to sheen color for sheen effect.
    :type sheen_tint: float, optional
    :param sheen_roughness: Sheen diffuse/specular reflection roughness.
    :type sheen_roughness: float, optional
    """

    edge_color: Color3 = Color3(1, 1, 1)
    metallic: float = 0
    diffuse: float = 1
    specular: float = 1
    ior: float = 1
    transmission: float = 0
    transmission_color: Color3 = Color3(1, 1, 1)
    transmission_depth: float = 1
    roughness: float = 1
    anisotropy: float = 0
    anisotropy_rotation: float = 0
    thin: bool = False
    thickness: float = 1
    back_light: float = 0
    coat: float = 0
    coat_ior: float = 1.5
    coat_color: Color3 = Color3(1, 1, 1)
    coat_thickness: float = 1
    coat_roughness: float = 0
    sheen: float = 0
    sheen_color: Color3 = Color3(1, 1, 1)
    sheen_tint: float = 0
    sheen_roughness: float = 0.2

    @classmethod
    @property
    def name(cls) -> str:
        """Get the material name.

        :return: Material name
        :rtype: str
        """
        return "principled"

    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            "edge_color": list(self.edge_color),
            "metallic": self.metallic,
            "diffuse": self.diffuse,
            "specular": self.specular,
            "ior": self.ior,
            "transmission": self.transmission,
            "transmission_color": list(self.transmission_color),
            "transmission_depth": self.transmission_depth,
            "roughness": self.roughness,
            "anisotropy": self.anisotropy,
            "anisotropy_rotation": self.anisotropy_rotation,
            "thin": self.thin,
            "thickness": self.thickness,
            "back_light": self.back_light,
            "coat": self.coat,
            "coat_ior": self.coat_ior,
            "coat_color": list(self.coat_color),
            "coat_thickness": self.coat_thickness,
            "coat_roughness": self.coat_roughness,
            "sheen": self.sheen,
            "sheen_color": list(self.sheen_color),
            "sheen_tint": self.sheen_tint,
            "sheen_roughness": self.sheen_roughness,
        }

    def update_properties(self, message: dict[str, Any]) -> None:
        """Low level API to deserialize from JSON."""
        self.edge_color = Color3.unpack(message["edge_color"])
        self.metallic = message["metallic"]
        self.diffuse = message["diffuse"]
        self.specular = message["specular"]
        self.ior = message["ior"]
        self.transmission = message["transmission"]
        self.transmission_color = Color3.unpack(message["transmission_color"])
        self.transmission_depth = message["transmission_depth"]
        self.roughness = message["roughness"]
        self.anisotropy = message["anisotropy"]
        self.anisotropy_rotation = message["anisotropy_rotation"]
        self.thin = message["thin"]
        self.thickness = message["thickness"]
        self.back_light = message["back_light"]
        self.coat = message["coat"]
        self.coat_ior = message["coat_ior"]
        self.coat_color = Color3.unpack(message["coat_color"])
        self.coat_thickness = message["coat_thickness"]
        self.coat_roughness = message["coat_roughness"]
        self.sheen = message["sheen"]
        self.sheen_color = Color3.unpack(message["sheen_color"])
        self.sheen_tint = message["sheen_tint"]
        self.sheen_roughness = message["sheen_roughness"]


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

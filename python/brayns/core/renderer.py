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
from brayns.utils import Color4

T = TypeVar("T", bound="Renderer")


@dataclass
class Renderer(ABC):
    """Base class for all renderer types.

    Accumulation reduce the aliasing but increase the computation time.

    Ray bounces allow non-emissive objects to light other objects.

    :param samples_per_pixel: Accumulation, defaults to 1.
    :type samples_per_pixel: int, optional.
    :param max_ray_bounces: Ray bounces, defaults to 3.
    :type max_ray_bounces: int, optional.
    :param background_color: Background color, defaults to BBP transparent.
    :type background_color: Color4, optional.
    """

    samples_per_pixel: int = 1
    max_ray_bounces: int = 3
    background_color: Color4 = Color4.bbp_background.transparent

    @classmethod
    @property
    @abstractmethod
    def name(cls) -> str:
        """Get renderer name.

        :return: Renderer name.
        :rtype: str
        """
        pass

    @abstractmethod
    def get_additional_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        pass

    @abstractmethod
    def update_additional_properties(self, message: dict[str, Any]) -> None:
        """Low level API to deserialize from JSON."""
        pass

    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            "samples_per_pixel": self.samples_per_pixel,
            "max_ray_bounces": self.max_ray_bounces,
            "background_color": list(self.background_color),
            **self.get_additional_properties(),
        }

    @classmethod
    def from_properties(cls: type[T], message: dict[str, Any]) -> T:
        """Low level API to deserialize from JSON."""
        renderer = cls()
        renderer.update_properties(message)
        return renderer

    def get_properties_with_name(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            "name": self.name,
            "params": self.get_properties(),
        }

    def update_properties(self, message: dict[str, Any]) -> None:
        """Low level API to deserialize from JSON."""
        self.samples_per_pixel = message["samples_per_pixel"]
        self.max_ray_bounces = message["max_ray_bounces"]
        self.background_color = Color4(*message["background_color"])
        self.update_additional_properties(message)


@dataclass
class InteractiveRenderer(Renderer):
    """Default renderer used for fast rendering (streaming, tests).

    :param enable_shadows: Enable shadows, defaults to True.
    :type enable_shadows: bool, optional.
    :param ambient_occlusion_samples: AO samples, defaults to 0.
    :type ambient_occlusion_samples: int, optional.
    """

    enable_shadows: bool = True
    ambient_occlusion_samples: int = 0

    @classmethod
    @property
    def name(cls) -> str:
        """Get renderer name.

        :return: Renderer name.
        :rtype: str
        """
        return "interactive"

    def get_additional_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            "enable_shadows": self.enable_shadows,
            "ao_samples": self.ambient_occlusion_samples,
        }

    def update_additional_properties(self, message: dict[str, Any]) -> None:
        """Low level API to deserialize from JSON."""
        self.enable_shadows = message["enable_shadows"]
        self.ambient_occlusion_samples = message["ao_samples"]


@dataclass
class ProductionRenderer(Renderer):
    """Production renderer for expensive quality rendering.

    Overrides default parameters for higher quality.

    :param samples_per_pixel: Accumulation, defaults to 128.
    :type samples_per_pixel: int, optional.
    :param max_ray_bounces: Ray bounces, defaults to 7.
    :type max_ray_bounces: int, optional.
    """

    samples_per_pixel: int = 128
    max_ray_bounces: int = 7

    @classmethod
    @property
    def name(cls) -> str:
        """Get renderer name.

        :return: Renderer name.
        :rtype: str
        """
        return "production"

    def get_additional_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {}

    def update_additional_properties(self, message: dict[str, Any]) -> None:
        """Low level API to deserialize from JSON."""
        pass


def get_renderer_name(instance: Instance) -> str:
    """Retreive the name of the current renderer of an instance.

    :param instance: Instance.
    :type instance: Instance
    :return: Renderer name.
    :rtype: str
    """
    return instance.request("get-renderer-type")


def get_renderer(instance: Instance, renderer_type: type[T]) -> T:
    """Retreive the current renderer from an instance.

    Current renderer and given renderer type must be the same.

    Current renderer can be queried using ``get_rendere_name``.

    :param instance: Instance.
    :type instance: Instance
    :param renderer_type: Type of the current renderer.
    :type renderer_type: type[T]
    :return: Current renderer.
    :rtype: T
    """
    name = renderer_type.name
    result = instance.request(f"get-renderer-{name}")
    return renderer_type.from_properties(result)


def set_renderer(instance: Instance, renderer: Renderer) -> None:
    """Set the current renderer of an instance.

    :param instance: Instance.
    :type instance: Instance
    :param renderer: New renderer.
    :type renderer: Renderer
    """
    name = renderer.name
    params = renderer.get_properties()
    instance.request(f"set-renderer-{name}", params)

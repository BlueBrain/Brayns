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

from abc import ABC, abstractmethod
from dataclasses import dataclass
from typing import Any, TypeVar

from brayns.core.color import Color4

T = TypeVar('T', bound='Renderer')


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

    @classmethod
    @abstractmethod
    def deserialize(cls: type[T], message: dict[str, Any]) -> T:
        """Low level API to deserialize from JSON."""
        pass

    @property
    @abstractmethod
    def additional_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        pass

    @classmethod
    def deserialize_with(cls: type[T], message: dict[str, Any], **kwargs) -> T:
        """Low level API to deserialize from JSON."""
        return cls(
            samples_per_pixel=message['samples_per_pixel'],
            max_ray_bounces=message['max_ray_bounces'],
            background_color=Color4(*message['background_color']),
            **kwargs
        )

    @property
    def base_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            'samples_per_pixel': self.samples_per_pixel,
            'max_ray_bounces': self.max_ray_bounces,
            'background_color': list(self.background_color)
        }

    def serialize(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return self.base_properties | self.additional_properties

    def serialize_with_name(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            'name': self.name,
            'params': self.serialize()
        }

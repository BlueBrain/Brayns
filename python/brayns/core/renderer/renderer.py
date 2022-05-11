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
from typing import TypeVar

from brayns.core.common.color import Color
from brayns.instance.instance import Instance

T = TypeVar('T', bound='Renderer')


@dataclass
class Renderer(ABC):

    samples_per_pixel: int = 1
    max_ray_bounces: int = 3
    background_color: Color = Color.bbp_background.transparent

    @classmethod
    @property
    @abstractmethod
    def name(self) -> str:
        pass

    @classmethod
    @abstractmethod
    def deserialize(cls: type[T], message: dict) -> T:
        pass

    @abstractmethod
    def serialize(self) -> dict:
        pass

    @staticmethod
    def get_main_renderer_name(instance: Instance) -> str:
        return instance.request('get-renderer-type')

    @classmethod
    def from_instance(cls: type[T], instance: Instance) -> T:
        result = instance.request(f'get-renderer-{cls.name}')
        return cls.deserialize(result)

    @classmethod
    def is_main_renderer(cls, instance: Instance) -> bool:
        return cls.name == Renderer.get_main_renderer_name(instance)

    def use_as_main_renderer(self, instance: Instance) -> None:
        params = self.serialize()
        instance.request(f'set-renderer-{self.name}', params)

    @classmethod
    def _from_dict(cls: type[T], message: dict, **kwargs) -> T:
        return cls(
            samples_per_pixel=message['samples_per_pixel'],
            max_ray_bounces=message['max_ray_bounces'],
            background_color=Color(*message['background_color']),
            **kwargs
        )

    def _to_dict(self, properties: dict) -> dict:
        return {
            'samples_per_pixel': self.samples_per_pixel,
            'max_ray_bounces': self.max_ray_bounces,
            'background_color': list(self.background_color)
        } | properties

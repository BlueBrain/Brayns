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

T = TypeVar('T', bound='Light')


@dataclass
class Light(ABC):

    color: Color = Color.white
    intensity: float = 1.0
    visible: bool = True

    @classmethod
    @property
    @abstractmethod
    def name(self) -> str:
        pass

    @abstractmethod
    def serialize(self) -> dict:
        pass

    @staticmethod
    def remove(instance: Instance, ids: list[int]) -> None:
        params = {'ids': ids}
        instance.request('remove-lights', params)

    @staticmethod
    def clear(instance: Instance) -> None:
        instance.request('clear-lights')

    def add(self, instance: Instance) -> int:
        params = self.serialize()
        return instance.request(f'add-light-{self.name}', params)

    def _to_dict(self, properties: dict) -> dict:
        return {
            'color': list(self.color)[:3],
            'intensity': self.intensity,
            'visible': self.visible,
        } | properties

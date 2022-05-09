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
from dataclasses import dataclass, field, replace
from typing import TypeVar

from brayns.core.common.color import Color
from brayns.core.model.model import Model
from brayns.instance.instance import Instance

T = TypeVar('T', bound='Geometry')


@dataclass
class Geometry(ABC):

    color: Color = field(init=False, default=Color.white)

    @classmethod
    @property
    @abstractmethod
    def name(cls) -> str:
        pass

    @property
    @abstractmethod
    def properties(self) -> dict:
        pass

    @classmethod
    def add(cls: type[T], instance: Instance, geometries: list[T]) -> Model:
        params = [
            geometry.serialize()
            for geometry in geometries
        ]
        result = instance.request(f'add-{cls.name}', params)
        return Model.deserialize(result)

    def serialize(self) -> dict:
        return {
            'geometry': self.properties,
            'color': list(self.color)
        }

    def with_color(self: T, color: Color) -> T:
        geometry = replace(self)
        geometry.color = color
        return geometry

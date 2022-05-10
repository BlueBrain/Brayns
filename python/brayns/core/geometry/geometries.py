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
from typing import Generic, TypeVar

from brayns.core.common.color import Color
from brayns.core.model.model import Model
from brayns.instance.instance import Instance

T = TypeVar('T')


@dataclass
class Geometries(ABC, Generic[T]):

    items: list[tuple[T, Color]]

    @classmethod
    @property
    @abstractmethod
    def name(cls) -> str:
        pass

    @classmethod
    @abstractmethod
    def serialize_geometry(cls, geometry: T) -> dict:
        pass

    def add(self, instance: Instance) -> Model:
        params = self.serialize()
        result = instance.request(f'add-{self.name}', params)
        return Model.deserialize(result)

    def serialize(self) -> list[dict]:
        return [
            {
                'geometry': self.serialize_geometry(item[0]),
                'color': list(item[1])
            }
            for item in self.items
        ]

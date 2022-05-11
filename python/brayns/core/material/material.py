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

T = TypeVar('T', bound='Material')


@dataclass
class Material(ABC):

    color: Color = Color.white

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
    def get_material_name(instance: Instance, model_id: int) -> str:
        params = {'id': model_id}
        return instance.request('get-material-type', params)

    @classmethod
    def from_model(cls: type[T], instance: Instance, model_id: int) -> T:
        params = {'id': model_id}
        result = instance.request(f'get-material-{cls.name}', params)
        return cls.deserialize(result)

    @classmethod
    def is_applied(cls, instance: Instance, model_id: int) -> bool:
        return cls.name == Material.get_material_name(instance, model_id)

    def apply(self, instance: Instance, model_id: int) -> None:
        params = {
            'model_id': model_id,
            'material': self.serialize()
        }
        instance.request(f'set-material-{self.name}', params)

    @classmethod
    def _from_dict(cls: type[T], message: dict, **kwargs) -> T:
        return cls(
            color=Color(*message['color']),
            **kwargs
        )

    def _to_dict(self, properties: dict) -> dict:
        return {
            'color': list(self.color)[:3],
        } | properties

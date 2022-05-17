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

from dataclasses import dataclass
from typing import Optional

from brayns.core.common.bounds import Bounds
from brayns.core.common.transform import Transform
from brayns.instance.instance import Instance


@dataclass
class Model:

    id: int
    bounds: Bounds
    metadata: dict
    visible: bool
    transform: Transform

    @staticmethod
    def from_instance(instance: Instance, id: int) -> 'Model':
        result = instance.request('get-model', {'id': id})
        return Model.deserialize(result)

    @staticmethod
    def deserialize(message: dict) -> 'Model':
        return Model(
            id=message['model_id'],
            bounds=Bounds.deserialize(message['bounds']),
            metadata=message['metadata'],
            visible=message['is_visible'],
            transform=Transform.deserialize(message['transform'])
        )

    @staticmethod
    def get_all(instance: Instance) -> list['Model']:
        scene = Model._get_scene(instance)
        return [
            Model.deserialize(model)
            for model in scene['models']
        ]

    @staticmethod
    def get_bounds(instance: Instance) -> Bounds:
        scene = Model._get_scene(instance)
        return Bounds.deserialize(scene['bounds'])

    @staticmethod
    def remove(instance: Instance, ids: list[int]) -> None:
        instance.request('remove-model', {'ids': ids})

    @staticmethod
    def clear(instance: Instance) -> None:
        instance.request('clear-models')

    @staticmethod
    def update(
        instance: Instance,
        id: int,
        visible: Optional[bool] = None,
        transform: Optional[Transform] = None
    ) -> 'Model':
        model = {}
        params = {
            'model_id': id,
            'model': model
        }
        if visible is not None:
            model['is_visible'] = visible
        if transform is not None:
            model['transform'] = transform.serialize()
        result = instance.request('update-model', params)
        return Model.deserialize(result)

    @staticmethod
    def _get_scene(instance: Instance) -> dict:
        return instance.request('get-scene')

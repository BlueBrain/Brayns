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

from brayns.core.api.json_schema import JsonSchema
from brayns.instance.instance import Instance


@dataclass
class Entrypoint:

    name: str
    description: str
    plugin: str
    asynchronous: bool
    params: Optional[JsonSchema] = None
    result: Optional[JsonSchema] = None

    @staticmethod
    def from_method(instance: Instance, name: str) -> 'Entrypoint':
        params = {'endpoint': name}
        result = instance.request('schema', params)
        return Entrypoint.deserialize(result)

    @staticmethod
    def deserialize(message: dict) -> 'Entrypoint':
        return Entrypoint(
            name=message['title'],
            description=message['description'],
            plugin=message['plugin'],
            asynchronous=message['async'],
            params=_parse_schema(message, 'params'),
            result=_parse_schema(message, 'returns')
        )

    @staticmethod
    def get_all_methods(instance: Instance) -> list[str]:
        return instance.request('registry')

    @staticmethod
    def get_all(instance: Instance) -> list['Entrypoint']:
        return [
            Entrypoint.from_method(instance, method)
            for method in Entrypoint.get_all_methods(instance)
        ]


def _parse_schema(message: dict, key: str) -> JsonSchema:
    value = message.get(key)
    if value is None:
        return None
    return JsonSchema.deserialize(value)

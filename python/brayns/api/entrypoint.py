# Copyright 2015-2024 Blue Brain Project/EPFL
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

from dataclasses import dataclass, field

from .params import Params
from .result import Result


@dataclass
class Entrypoint:
    """Represent an entrypoint registered in the renderer (ex: get-camera).

    :param name: Entrypoint name (hyphen)
    :type name: str
    :param description: Entrypoint description
    :type description: str
    :param plugin: Name of the plugin loading the entrypoint
    :type plugin: str
    :param params: Entrypoint params schemas formatted as in Python method
    :type params: List[Schema]
    :param result: Entrypoint result schema
    :type result: Schema, can be None
    :param unpack_params: True if the params are unpacked in Python method
    :type unpack_params: bool, defaults to True
    """

    name: str
    description: str
    plugin: str = ''
    params: Params = field(default_factory=Params)
    result: Result = field(default_factory=Result)

    @staticmethod
    def from_schema(schema: dict):
        """Build an entrypoint instance using its parsed JSON schema.

        Entrypoint params are formatted as they appear in Python methods.

        i.e. they are sorted with required ones first, then by name.

        If there is a single argument with no name, it will be called "params".

        :param schema: JSON schema parsed as dict
        :type schema: dict
        :return: Entrypoint instance
        :rtype: Entrypoint
        """
        return Entrypoint(
            name=_get_name(schema),
            description=_get_description(schema),
            plugin=_get_plugin(schema),
            params=_get_params(schema),
            result=_get_result(schema)
        )


def _get_name(schema: dict) -> str:
    return schema['title']


def _get_description(schema: dict) -> str:
    return schema.get('description', '')


def _get_plugin(schema: dict) -> str:
    return schema['plugin']


def _get_params(schema: dict):
    return Params.from_dicts(schema.get('params', []))


def _get_result(schema: dict):
    return Result.from_dict(schema.get('returns', {}))

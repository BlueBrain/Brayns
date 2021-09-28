# Copyright (c) 2021 EPFL/Blue Brain Project
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
from typing import Iterable, List, Union

from .schema import Schema


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
    """

    name: str
    description: str
    plugin: str = ''
    params: List[Schema] = field(default_factory=list)
    result: Union[Schema, None] = None

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
    params = schema.get('params', [])
    if not params:
        return []
    param = Schema.from_dict(params[0])
    if param.read_only:
        return []
    if not param.properties:
        return [_format_single_param(param)]
    return [
        child
        for child in _format_params(param).properties
    ]


def _format_single_param(param: Schema):
    if not param.name:
        param.name = 'params'
    return param


def _format_params(params: Schema):
    params.properties = _sort_properties(
        _format_params(schema)
        for schema in params.properties
        if not schema.read_only
    )
    return params


def _sort_properties(properties: Iterable[Schema]):
    return sorted(
        properties,
        key=lambda param: (not param.required, param.name)
    )


def _get_result(schema: dict):
    returns = schema.get('returns', {})
    if not returns:
        return None
    result = Schema.from_dict(returns)
    if result.write_only:
        return None
    return _format_result(result)


def _format_result(result: Schema):
    result.properties = _sort_properties(
        _format_result(schema)
        for schema in result.properties
        if not schema.write_only
    )
    return result

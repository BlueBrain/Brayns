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

"""Helper module to build automatically Sphinx documentation."""

import collections
import pathlib
import re
from typing import Dict, Iterable, List, Union

from .. import connect
from ..api import entrypoint_loader
from ..api.entrypoint import Entrypoint
from ..api.schema import Schema
from ..client.abstract_client import AbstractClient


def save_rst_doc(
    uri: str,
    folder: str,
    pattern: str = 'api_{plugin}_methods.rst',
    secure: bool = False,
    cafile: Union[str, None] = None
) -> None:
    """Build and save directly the RST documentation per plugin.

    The output file pattern will be formatted using the plugin name with
    'plugin' as keyword argument.

    Plugin name is lowered with removed spaces.

    :param uri: URI of the renderer to document
    :type uri: str
    :param folder: Output folder
    :type folder: str
    :param pattern: Output files pattern, defaults to 'api_{plugin}_methods.rst'
    :type pattern: str, optional
    :param secure: Renderer use SSL, defaults to False
    :type secure: bool, optional
    :param cafile: Renderer certificate if SSL enabled, defaults to None
    :type cafile: Union[str, None], optional
    """
    with connect(uri, secure, cafile) as client:
        for plugin, doc in build_rst_doc(client).items():
            with open(
                pathlib.Path(folder) / pattern.format(
                    plugin=plugin.replace(' ', '').lower()
                ),
                'w'
            ) as rst:
                rst.write(doc)


def build_rst_doc(client: AbstractClient) -> Dict[str, str]:
    """Build Sphinx documentation using the given client.

    The client must be connected to the renderer about which the documentation
    will be generated.

    :param client: Client connected to the renderer
    :type client: AbstractClient
    :return: Dict plugin -> RST doc
    :rtype: Dict[str, str]
    """
    return {
        plugin: _build_rst_doc(plugin, entrypoint)
        for plugin, entrypoint in _get_entrypoints_by_plugin(client).items()
    }


_HEADER = '''{title}
{underline}

The Brayns python client API is automatically generated when connecting to a
running backend service as shown in :ref:`usepythonclient-label`.

The Core plugin is always loaded so the related entrypoints are always
available.

Other plugins might register additional entrypoints but must be loaded in the
renderer to be available.

The functions below are generated using the entrypoints of the {plugin} plugin.

All arguments are keyword arguments extracted from the entrypoint params.

Renderer errors will be raised with an instance of brayns.ReplyError. This one
can be used to extract error code, description and some optional additional data
(mainly used to store JSON errors).

Example usage of some generated methods:

.. code-block: python
    import brayns

    with brayns.connect(uri='localhost:5000') as client:
        print(client.get_camera())
        client.set_camera(current='orthographic')
        print(client.get_camera())

'''

_METHOD = '''
{name}
{underline}

{description}.

Parameters:

{params}

Return value:

{result}

'''


class SchemaFormatter:

    _SCHEMA = '{indent}* {name}``{typename}``{description}{children}'

    def __init__(self, indent=2) -> None:
        self._indent = indent
        self._depth = 0

    def format_schema(self, schema: Schema):
        self._depth = 0
        return self._format(schema)

    def format_schemas(self, schemas: Iterable[Schema]):
        self._depth = 0
        return self._format_schemas(schemas)

    def _format(self, schema: Schema):
        if self._is_one_of(schema):
            return self._format_one_of(schema)
        if self._has_properties(schema):
            return self._format_properties(schema)
        if self._is_array(schema):
            return self._format_array(schema)
        if self._is_object(schema):
            return self._format_object(schema)
        return self._format_primitive(schema)

    def _format_schema(
        self,
        schema: Schema,
        description: str = '',
        children: Iterable[Schema] = []
    ):
        description = description or schema.description
        if description:
            description = f'. {description}.'
        return self._SCHEMA.format(
            indent=self._get_indent(),
            name=self._get_name(schema),
            typename=schema.typename,
            description=description,
            children=self._format_children(children)
        )

    def _get_indent(self):
        return self._indent * self._depth * ' '

    def _get_name(self, schema: Schema):
        return f'``{schema.name}``: ' if schema.name else ''

    def _extend_description(self, schema: Schema, description: str):
        if not schema.description:
            return description
        return schema.description + '. ' + description

    def _format_children(self, schemas: Iterable[Schema]):
        self._depth += 1
        description = self._format_schemas(schemas)
        self._depth -= 1
        return ('\n\n' + description + '\n') if description else ''

    def _format_schemas(self, schemas: Iterable[Schema]):
        return '\n'.join(
            self._format(schema)
            for schema in schemas
        )

    def _is_one_of(self, schema: Schema):
        return bool(schema.one_of)

    def _format_one_of(self, schema: Schema):
        return self._format_schema(
            schema=schema,
            description=self._extend_description(
                schema,
                'Can be one of the following objects depending on the '
                'renderer configuration'
            ),
            children=schema.one_of
        )

    def _has_properties(self, schema: Schema):
        return bool(schema.properties)

    def _format_properties(self, schema: Schema):
        return self._format_schema(
            schema=schema,
            description=self._extend_description(
                schema,
                'The object has the following properties'
            ),
            children=schema.properties
        )

    def _is_array(self, schema: Schema):
        return schema.items is not None

    def _format_array(self, schema: Schema):
        return self._format_schema(
            schema=schema,
            children=[schema.items]
        )

    def _is_object(self, schema: Schema):
        return schema.additional_properties is not None

    def _format_object(self, schema: Schema):
        return self._format_schema(
            schema=schema,
            children=[schema.additional_properties]
        )

    def _format_primitive(self, schema: Schema):
        return self._format_schema(schema)


def _build_rst_doc(plugin: str, entrypoints: List[Entrypoint]):
    return re.sub(
        pattern=r'\n{3,}',
        repl=r'\n\n',
        string=_build_raw_rst(plugin, entrypoints)
    )


def _build_raw_rst(plugin: str, entrypoints: List[Entrypoint]):
    title = f'{plugin} API methods'
    return _HEADER.format(
        title=title,
        underline=len(title) * '-',
        plugin=plugin
    ) + '----\n\n'.join(
        _format_entrypoint(entrypoint)
        for entrypoint in entrypoints
    )


def _get_entrypoints_by_plugin(client: AbstractClient):
    entrypoints = collections.defaultdict(list)
    for entrypoint in entrypoint_loader.load_all_entrypoints(client):
        entrypoints[entrypoint.plugin].append(entrypoint)
    for plugin_entrypoints in entrypoints.values():
        plugin_entrypoints.sort(
            key=lambda entrypoint: entrypoint.name
        )
    return entrypoints


def _format_params(entrypoint: Entrypoint):
    return (
        SchemaFormatter().format_schemas(entrypoint.params.schemas)
        if entrypoint.params
        else 'This method takes no parameters.'
    )


def _format_result(entrypoint: Entrypoint):
    return (
        SchemaFormatter().format_schema(entrypoint.result.schema)
        if entrypoint.result
        else 'This method has no return values.'
    )


def _format_entrypoint(entrypoint: Entrypoint):
    name = entrypoint.name.replace('-', '_')
    return _METHOD.format(
        name=name,
        underline=len(name) * '~',
        description=entrypoint.description,
        params=_format_params(entrypoint),
        result=_format_result(entrypoint)
    )

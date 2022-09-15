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

from __future__ import annotations

import json
import pathlib
import sys
from collections import defaultdict
from typing import Any

import brayns

SUMMARY_FILENAME = 'api.rst'

SUMMARY = '''
.. _jsonrpcapi-label:

JSON-RPC API reference
======================

.. toctree::
    :maxdepth: 4{items}
'''.strip()

FILENAME = 'api_{plugin}_methods.rst'

TITLE = '{plugin} API methods'

HEADER = '''
.. _api{label}-label:

{title}
{underline}

This page references the entrypoints of the {plugin} plugin.
'''.strip()

ENTRYPOINT = '''
{name}
{underline}

{description}.{asynchronous}

**Params**:

{params}

**Result**:

{result}{separator}
'''.strip()

ASYNCHRONOUS = '''
This entrypoint is asynchronous, it means that it can take a long time and send
progress notifications.
'''.strip()

NO_PARAMS = '''
This entrypoint has no params, the "params" field can hence be omitted or null.
'''.strip()

NO_RESULT = '''
This entrypoint has no result, the "result" field is still present but is always
null.
'''.strip()

SCHEMA = '''
.. jsonschema::

    {data}
'''.strip()

SEPARATOR = '\n\n----'


def build_from_argv() -> None:
    python = pathlib.Path(__file__).parent.parent
    directory = python / 'doc' / 'source' / 'jsonrpcapi'
    uri = 'localhost:5000'
    argv = sys.argv
    if len(argv) > 1:
        directory = pathlib.Path(argv[1])
    if len(argv) > 2:
        uri = argv[2]
    build_from_uri(uri, directory)


def build_from_uri(uri: str, directory: pathlib.Path) -> None:
    connector = brayns.Connector(uri)
    with connector.connect() as instance:
        build_from_instance(instance, directory)


def build_from_instance(instance: brayns.Instance, directory: pathlib.Path) -> None:
    entrypoints = brayns.get_entrypoints(instance)
    return build_from_entrypoints(entrypoints, directory)


def build_from_entrypoints(entrypoints: list[brayns.Entrypoint], directory: pathlib.Path) -> None:
    plugins = defaultdict[str, list[brayns.Entrypoint]](list)
    for entrypoint in entrypoints:
        plugins[entrypoint.plugin].append(entrypoint)
    for entrypoints in plugins.values():
        entrypoints.sort(key=lambda entrypoint: entrypoint.method)
    build_from_plugins(plugins, directory)


def build_from_plugins(plugins: dict[str, list[brayns.Entrypoint]], directory: pathlib.Path) -> None:
    directory.mkdir(exist_ok=True)
    filenames = list[str]()
    for plugin, entrypoints in plugins.items():
        filename = FILENAME.format(plugin=plugin.lower().replace(' ', ''))
        filenames.append(filename)
        build_plugin(plugin, entrypoints, directory, filename)
    build_summary(directory, filenames)


def build_summary(directory: pathlib.Path, filenames: list[str]) -> None:
    summary = format_summary(filenames) + '\n'
    path = directory / SUMMARY_FILENAME
    with path.open('w') as file:
        file.write(summary)


def format_summary(filenames: list[str]) -> str:
    items = format_summary_items(filenames)
    return SUMMARY.format(items=items)


def format_summary_items(filenames: list[str]) -> str:
    if not filenames:
        return ''
    separator = '\n    '
    return '\n' + separator + separator.join(
        filename.replace('.rst', '')
        for filename in filenames
    )


def build_plugin(plugin: str, entrypoints: list[brayns.Entrypoint], directory: pathlib.Path, filename: str) -> None:
    data = format_plugin(plugin, entrypoints) + '\n'
    path = directory / filename
    with path.open('w') as file:
        file.write(data)


def format_plugin(plugin: str, entrypoints: list[brayns.Entrypoint]) -> str:
    title = TITLE.format(plugin=plugin)
    header = HEADER.format(
        label=plugin.lower().replace(' ', ''),
        title=title,
        underline=len(title) * '-',
        plugin=plugin,
    )
    api = format_entrypoints(entrypoints)
    return f'{header}\n\n{api}'


def format_entrypoints(entrypoints: list[brayns.Entrypoint]) -> str:
    docs = list[str]()
    for entrypoint in entrypoints[:-1]:
        doc = format_entrypoint(entrypoint)
        docs.append(doc)
    last = format_entrypoint(entrypoints[-1], separator=False)
    docs.append(last)
    return '\n\n'.join(docs)


def format_entrypoint(entrypoint: brayns.Entrypoint, separator: bool = True) -> str:
    return ENTRYPOINT.format(
        name=entrypoint.method,
        underline=len(entrypoint.method) * '~',
        description=entrypoint.description,
        asynchronous=f'\n\n{ASYNCHRONOUS}' if entrypoint.asynchronous else '',
        params=format_params(entrypoint.params),
        result=format_result(entrypoint.result),
        separator=SEPARATOR if separator else '',
    )


def format_params(schema: brayns.JsonSchema | None) -> str:
    if schema is None:
        return NO_PARAMS
    return format_schema(schema)


def format_result(schema: brayns.JsonSchema | None) -> str:
    if schema is None:
        return NO_RESULT
    return format_schema(schema)


def format_schema(schema: brayns.JsonSchema) -> str:
    message = serialize_schema(schema)
    message.pop('title', None)
    data = json.dumps(message, indent=4)
    data = data.replace('\n', '\n    ')
    return SCHEMA.format(data=data)


def serialize_schema(schema: brayns.JsonSchema) -> dict[str, Any]:
    message = dict[str, Any]()
    if schema.title:
        message['title'] = schema.title
    if schema.description:
        message['description'] = schema.description
    if schema.type is not brayns.JsonType.UNDEFINED:
        message['type'] = schema.type.value
    if schema.read_only:
        message['readOnly'] = schema.read_only
    if schema.write_only:
        message['writeOnly'] = schema.write_only
    if schema.default is not None:
        message['default'] = schema.default
    if schema.minimum is not None:
        message['minimum'] = schema.minimum
    if schema.maximum is not None:
        message['maximum'] = schema.maximum
    if schema.items is not None:
        message['items'] = serialize_schema(schema.items)
    if schema.min_items is not None:
        message['minItems'] = schema.min_items
    if schema.max_items is not None:
        message['maxItems'] = schema.max_items
    if schema.properties:
        message['properties'] = serialize_properties(schema)
    if schema.required:
        message['required'] = schema.required
    if schema.additional_properties is not None:
        message['additionalProperties'] = serialize_additional(schema)
    if schema.one_of:
        message['oneOf'] = serialize_one_of(schema)
    if schema.enum:
        message['enum'] = schema.enum
    return message


def serialize_properties(schema: brayns.JsonSchema) -> dict[str, Any]:
    return {
        key: serialize_schema(value)
        for key, value in schema.properties.items()
    }


def serialize_additional(schema: brayns.JsonSchema) -> bool | dict[str, Any] | None:
    properties = schema.additional_properties
    if properties is None:
        return None
    if isinstance(properties, bool):
        return properties
    return serialize_schema(properties)


def serialize_one_of(schema: brayns.JsonSchema) -> list[dict[str, Any]]:
    return [
        serialize_schema(one_of)
        for one_of in schema.one_of
    ]


if __name__ == '__main__':
    build_from_argv()

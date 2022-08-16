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
    message = schema.serialize()
    message.pop('title', None)
    data = json.dumps(message, indent=4)
    data = data.replace('\n', '\n    ')
    return SCHEMA.format(data=data)


if __name__ == '__main__':
    build_from_argv()

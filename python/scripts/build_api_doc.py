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

import pathlib
import sys
from collections import defaultdict
from typing import Optional

import brayns

FILENAME = 'api_{plugin}_methods.rst'

TITLE = '{plugin} API methods'

HEADER = '''
{title}
{underline}

This page references the entrypoints of the {plugin} plugin.

The Core plugin is always loaded so the related entrypoints are always
available.

Other plugins like CircuitExplorer or DTI might register additional entrypoints
but they must be loaded when starting the renderer instance to be available.

All entrypoints use a JSON-RPC protocol with the following scheme:

Request:
{
    "jsonrpc": "2.0",
    "id": 123,
    "method": "name",
    "params": {
        "field": 456
    }
}

Reply:
{
    "jsonrpc": "2.0",
    "id": 123,
    "result": {
        "another_field": 789
    }
}

Error:
{
    "jsonrpc": "2.0",
    "id": 123,
    "error": {
        "code": 30,
        "message": "An error occured",
        "data": "Optional additional data"
    }
}

Notification (progress):
{
    "jsonrpc": "2.0",
    "params": {
        "id": 123,
        "operation": "Loading stuff",
        "amount": 0.5
    }
}

The following sections describe each entrypoints of the {plugin} plugin with
their params and result schemas.
'''.strip()

ENTRYPOINT = '''
{name}
{underline}

{description}.{asynchronous}

Params:

{params}

Result:

{result}{separator}
'''.strip()

ASYNCHRONOUS = '''
This entrypoint is asynchronous, it means that it can take a long time and send
progress notifications.
'''.strip()

NO_PARAMS = '''
This entrypoint has no params, the "params" field can hence be omitted or set to
null.
'''.strip()

NO_RESULT = '''
This entrypoint has no result, the "result" field is still present but is always
null.
'''.strip()

SEPARATOR = '\n\n----'


def build_from_uri(uri: str, directory: str) -> None:
    with brayns.connect(uri) as instance:
        build_from_instance(instance, directory)


def build_from_instance(instance: brayns.Instance, directory: str) -> None:
    entrypoints = brayns.Entrypoint.get_all(instance)
    return build_from_entrypoints(entrypoints, directory)


def build_from_entrypoints(entrypoints: list[brayns.Entrypoint], directory: str) -> None:
    plugins = defaultdict[str, list[brayns.Entrypoint]](list)
    for entrypoint in entrypoints:
        plugins[entrypoint.plugin].append(entrypoint)
    build_from_plugins(plugins, directory)


def build_from_plugins(plugins: dict[str, brayns.Entrypoint], directory: str) -> None:
    for plugin, entrypoints in plugins:
        data = format_plugin(plugin, entrypoints)
        path = pathlib.Path(directory) / FILENAME.format(plugin=plugin)
        with open(path, 'w') as file:
            file.write(data)


def format_plugin(plugin: str, entrypoints: list[brayns.Entrypoint]) -> str:
    title = TITLE.format(plugin=plugin)
    header = HEADER.format(
        title=title,
        underline=get_underline(title),
        plugin=plugin
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
        name=entrypoint.name,
        underline=get_underline(entrypoint.name),
        description=entrypoint.description,
        asynchronous=ASYNCHRONOUS if entrypoint.asynchronous else '',
        params=format_params(entrypoint.params),
        result=format_result(entrypoint.result),
        separator=SEPARATOR if separator else ''
    )


def format_params(schema: Optional[brayns.JsonSchema]) -> str:
    if schema is None:
        return NO_PARAMS
    return format_schema(schema)


def format_result(schema: Optional[brayns.JsonSchema]) -> str:
    if schema is None:
        return NO_RESULT
    return format_schema(schema)


def format_schema(schema: brayns.JsonSchema) -> str:
    pass


def get_underline(title: str) -> str:
    return len(title) * '~'


if __name__ == '__main__':
    uri = 'localhost:5000'
    directory = pathlib.Path(__file__).parent.parent / 'doc' / 'source'
    argv = sys.argv
    if len(argv) > 1:
        uri = argv[1]
    if len(argv) > 2:
        directory = pathlib.Path(argv[2])
    build_from_uri(uri, str(directory))

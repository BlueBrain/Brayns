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
.. _api-label:

JSON-RPC API reference
======================

This page references the entrypoints of Brayns JSON-RPC API.

The available entrypoints depend on the plugins loaded in the instance and are hence grouped by plugin.

The Core plugin is always loaded as long as the instance is running a websocket server (--uri provided).

All entrypoints use a JSON-RPC protocol (see specifications `here <https://www.jsonrpc.org/specification>`_).

Here are examples of the messages used by Brayns

Request
-------

The request message includes the protocol version, an optional ID, a method name and optional params.

The protocol version must always be 2.0.

The Request ID can be ignored if you don't care about the reply.

The Request ID must be integer or string otherwise.

The method is the name of the entrypoint (for example "get-camera-look-at").

The params can be anything depending on the entrypoint params schema.

.. code-block:: json

    {{
        "jsonrpc": "2.0",
        "id": 0,
        "method": "test",
        "params": {{
            "field": 123
        }}
    }}

Reply
-----

The reply is sent only if the request ID is not null nor omitted.

The protocol version must always be 2.0.

The ID is the one of the corresponding request it is replying to.

The result field is always present but can be null if nothing is replied.

The result can be anything depending on the entrypoint result schema.

.. code-block:: json

    {{
        "jsonrpc": "2.0",
        "id": 0,
        "result": {{
            "another_field": 456
        }}
    }}

Error
-----

The error message is sent instead of the reply if an error occurs.

The protocol version must always be 2.0.

The ID is the same as it would have been for the reply.

The error field is always present and differentiate it from a reply.

The error field contains a code, a description and optional data.

The additional data is mainly used in case of invalid params to detail the schema error(s) with a list of strings.

.. code-block:: json

    {{
        "jsonrpc": "2.0",
        "id": 0,
        "error": {{
            "code": 12,
            "message": "Something happened",
            "data": [
                "The field a.b.c is missing."
            ]
        }}
    }}

Notification
------------

The notification message is used by asynchronous entrypoints to send progress messages.

The protocol version must always be 2.0.

Notifications have no ID by contrast with replies and errors.

Notification params are always progress messages.

Progress messages contain the ID of the request being processed, a description and a progress amount.

The progress amount is a number between 0 and 1.

.. code-block:: json

    {{
        "jsonrpc": "2.0",
        "params": {{
            "id": 0,
            "operation": "Processing stuff...",
            "amount": 0.5
        }}
    }}

.. toctree::
    :hidden:{items}
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
This entrypoint is asynchronous, it means that it can take a long time and send progress notifications.
'''.strip()

NO_PARAMS = '''
This entrypoint has no params, the "params" field can hence be omitted or null.
'''.strip()

NO_RESULT = '''
This entrypoint has no result, the "result" field is still present but is always null.
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

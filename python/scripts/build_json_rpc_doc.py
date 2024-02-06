# Copyright (c) 2015-2024 EPFL/Blue Brain Project
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
import sys
from collections import defaultdict
from dataclasses import dataclass, field
from pathlib import Path

import brayns

SUMMARY_FILENAME = "api.rst"

SUMMARY = """
.. _jsonrpcapi-label:

JSON-RPC API reference
======================

.. toctree::
    :maxdepth: 4{items}
""".strip()

FILENAME = "api_{plugin}_methods.rst"

TITLE = "{plugin} API"

HEADER = """
.. _api{label}-label:

{title}
{underline}

This page references the loaders and entrypoints registered by the {plugin} plugin.
""".strip()

CONTENT = """
{header}

Loaders
-------

{loaders}

Entrypoints
-----------

{entrypoints}
""".strip()

LOADER = """
{name}
{underline}

Can load the following formats: {extensions}.

{binary}

{schema}{separator}
""".strip()

NO_LOADERS = "This plugin does not register any loaders."

BINARY = "This loader supports loading binary data using 'upload-model'."

NO_BINARY = "This loader does not support loading binary data using 'upload-model'."

ENTRYPOINT = """
{name}
{underline}

{description}.{asynchronous}{deprecated}

**Params**:

{params}

**Result**:

{result}{separator}
""".strip()

NO_ENTRYPOINTS = "This plugin does not register any entrypoints."

ASYNCHRONOUS = """
This entrypoint is asynchronous, it means that it can take a long time and send
progress notifications.
""".strip()

DEPRECATED = """
.. attention::

    This entrypoint is DEPRECATED, it will be removed or renamed in the next
    major release.
""".strip()

NO_PARAMS = """
This entrypoint has no params, the "params" field can hence be omitted or null.
""".strip()

NO_RESULT = """
This entrypoint has no result, the "result" field is still present but is always
null.
""".strip()

SCHEMA = """
.. jsonschema::

    {data}
""".strip()

SEPARATOR = "\n\n----"


@dataclass
class Plugin:
    loaders: list[brayns.LoaderInfo] = field(default_factory=list)
    entrypoints: list[brayns.Entrypoint] = field(default_factory=list)


def build_from_argv() -> None:
    python = Path(__file__).parent.parent
    directory = python / "doc" / "source" / "jsonrpcapi"
    uri = "localhost:5000"
    argv = sys.argv
    if len(argv) > 1:
        directory = Path(argv[1])
    if len(argv) > 2:
        uri = argv[2]
    build_from_uri(uri, directory)


def build_from_uri(uri: str, directory: Path) -> None:
    connector = brayns.Connector(uri)
    with connector.connect() as instance:
        build_from_instance(instance, directory)


def build_from_instance(instance: brayns.Instance, directory: Path) -> None:
    plugins = defaultdict[str, Plugin](Plugin)
    for loader in brayns.get_loaders(instance):
        plugins[loader.plugin].loaders.append(loader)
    for entrypoint in brayns.get_entrypoints(instance):
        plugins[entrypoint.plugin].entrypoints.append(entrypoint)
    for plugin in plugins.values():
        plugin.loaders.sort(key=lambda loader: loader.name)
        plugin.entrypoints.sort(key=lambda entrypoint: entrypoint.method)
    build_from_plugins(plugins, directory)


def plugin_id(name: str) -> str:
    return name.lower().replace(" ", "")


def build_from_plugins(plugins: dict[str, Plugin], directory: Path) -> None:
    directory.mkdir(parents=True, exist_ok=True)
    filenames = list[str]()
    for name, plugin in plugins.items():
        filename = FILENAME.format(plugin=plugin_id(name))
        filenames.append(filename)
        build_plugin(name, plugin, directory, filename)
    build_summary(directory, filenames)


def build_summary(directory: Path, filenames: list[str]) -> None:
    summary = format_summary(filenames) + "\n"
    path = directory / SUMMARY_FILENAME
    with path.open("w") as file:
        file.write(summary)


def format_summary(filenames: list[str]) -> str:
    items = format_summary_items(filenames)
    return SUMMARY.format(items=items)


def format_summary_items(filenames: list[str]) -> str:
    if not filenames:
        return ""
    separator = "\n    "
    return (
        "\n"
        + separator
        + separator.join(filename.replace(".rst", "") for filename in filenames)
    )


def build_plugin(name: str, plugin: Plugin, directory: Path, filename: str) -> None:
    data = format_plugin(name, plugin) + "\n"
    path = directory / filename
    with path.open("w") as file:
        file.write(data)


def format_plugin(name: str, plugin: Plugin) -> str:
    title = TITLE.format(plugin=name)
    header = HEADER.format(
        label=plugin_id(name),
        title=title,
        underline=len(title) * "=",
        plugin=name,
    )
    loaders = format_loaders(plugin.loaders)
    entrypoints = format_entrypoints(plugin.entrypoints)
    return CONTENT.format(header=header, loaders=loaders, entrypoints=entrypoints)


def format_loaders(loaders: list[brayns.LoaderInfo]) -> str:
    if not loaders:
        return NO_LOADERS
    docs = list[str]()
    for loader in loaders[:-1]:
        doc = format_loader(loader)
        docs.append(doc)
    last = format_loader(loaders[-1], separator=False)
    docs.append(last)
    return "\n\n".join(docs)


def format_loader(loader: brayns.LoaderInfo, separator: bool = True) -> str:
    return LOADER.format(
        name=loader.name,
        underline=len(loader.name) * "~",
        plugin=loader.plugin,
        extensions=", ".join(f"**{extension}**" for extension in loader.extensions),
        binary=BINARY if loader.binary else NO_BINARY,
        schema=format_schema(loader.schema),
        separator=SEPARATOR if separator else "",
    )


def format_entrypoints(entrypoints: list[brayns.Entrypoint]) -> str:
    if not entrypoints:
        return NO_ENTRYPOINTS
    docs = list[str]()
    for entrypoint in entrypoints[:-1]:
        doc = format_entrypoint(entrypoint)
        docs.append(doc)
    last = format_entrypoint(entrypoints[-1], separator=False)
    docs.append(last)
    return "\n\n".join(docs)


def format_entrypoint(entrypoint: brayns.Entrypoint, separator: bool = True) -> str:
    return ENTRYPOINT.format(
        name=entrypoint.method,
        underline=len(entrypoint.method) * "~",
        description=entrypoint.description,
        asynchronous=f"\n\n{ASYNCHRONOUS}" if entrypoint.asynchronous else "",
        deprecated=f"\n\n{DEPRECATED}" if entrypoint.deprecated else "",
        params=format_params(entrypoint.params),
        result=format_result(entrypoint.result),
        separator=SEPARATOR if separator else "",
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
    message = brayns.serialize_schema(schema)
    message.pop("title", None)
    data = json.dumps(message, indent=4)
    data = data.replace("\n", "\n    ")
    return SCHEMA.format(data=data)


if __name__ == "__main__":
    build_from_argv()

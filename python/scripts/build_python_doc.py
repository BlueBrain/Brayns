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

import inspect
import pathlib
import sys
from types import ModuleType

import brayns

API_FILENAME = 'api.rst'

API = '''
Python API reference
====================

.. toctree::
    :maxdepth: 4

{subpackages}
'''.strip()

SUBPACKAGE_FILENAME = '{name}.rst'

SUBPACKAGE = '''
{name}
{underline}

.. automodule:: {module}

Classes
-------

{classes}

Functions
---------

{functions}
'''.strip()


AUTOMODULE = '''
.. automodule:: {module}
    :members: {member}
    :undoc-members:
    :show-inheritance:
'''.strip()


def build_from_argv() -> None:
    python = pathlib.Path(__file__).parent.parent
    directory = python / 'doc' / 'source' / 'pythonapi'
    argv = sys.argv
    if len(argv) > 1:
        directory = pathlib.Path(argv[1])
    build(directory)


def build(directory: pathlib.Path) -> None:
    directory.mkdir(exist_ok=True)
    subpackages = get_subpackages()
    build_summary(directory, subpackages)
    for subpackage in subpackages:
        build_subpackage(directory, subpackage)


def get_subpackages() -> list[str]:
    python = pathlib.Path(__file__).parent.parent
    directory = python / 'brayns'
    return sorted(
        child.name
        for child in directory.glob('*')
        if child.is_dir()
        if not child.name.startswith('__')
    )


def build_summary(directory: pathlib.Path, subpackages: list[str]) -> None:
    path = directory / API_FILENAME
    summary = format_subpackages_summary(subpackages)
    data = API.format(subpackages=summary)
    with path.open('wt') as file:
        file.write(data)


def format_subpackages_summary(subpackages: list[str]) -> str:
    return '\n'.join(
        f'    {subpackage}'
        for subpackage in subpackages
    )


def build_subpackage(directory: pathlib.Path, subpackage: str) -> None:
    path = directory / f'{subpackage}.rst'
    data = format_subpackage(subpackage)
    with path.open('wt') as file:
        file.write(data)


def format_subpackage(subpackage: str) -> str:
    underline = len(subpackage) * '='
    module = f'brayns.{subpackage}'
    obj = getattr(brayns, subpackage)
    items = get_subpackage_items(obj)
    classes = get_classes(items)
    functions = get_functions(items)
    return SUBPACKAGE.format(
        name=subpackage,
        underline=underline,
        module=module,
        classes=format_items(classes),
        functions=format_items(functions),
    )


def format_items(items: list[str]) -> str:
    if not items:
        return 'None'
    return '\n\n'.join(
        format_item(item)
        for item in items
    )


def format_item(item: str) -> str:
    obj = getattr(brayns, item)
    module = obj.__module__
    return AUTOMODULE.format(
        module=module,
        member=item,
    )


def get_subpackage_items(module: ModuleType) -> list[str]:
    return [
        item
        for item in module.__all__
        if item in brayns.__all__
    ]


def get_classes(items: list[str]) -> list[str]:
    return sorted(
        item
        for item in items
        if is_class(item)
    )


def is_class(item: str) -> bool:
    obj = getattr(brayns, item)
    return inspect.isclass(obj)


def get_functions(items: list[str]) -> list[str]:
    return sorted(
        item
        for item in items
        if is_function(item)
    )


def is_function(item: str) -> bool:
    obj = getattr(brayns, item)
    return inspect.isfunction(obj)


if __name__ == '__main__':
    build_from_argv()

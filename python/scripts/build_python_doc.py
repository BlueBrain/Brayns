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

import sys
from collections import defaultdict
from pathlib import Path
from types import ModuleType

import brayns

API_FILENAME = "api.rst"

API = """
.. _pythonapi-label:

Python API reference
====================

.. toctree::
    :maxdepth: 4

{subpackages}
""".strip()

SUBPACKAGE_FILENAME = "{name}.rst"

SUBPACKAGE = """
{title}
{underline}

.. automodule:: {module}

{items}
""".strip()


AUTOMODULE = """
{title}
{underline}

.. automodule:: {module}
    :members: {members}
    :undoc-members:
    :show-inheritance:
""".strip()


def build_from_argv() -> None:
    python = Path(__file__).parent.parent
    directory = python / "doc" / "source" / "pythonapi"
    argv = sys.argv
    if len(argv) > 1:
        directory = Path(argv[1])
    build(directory)


def build(directory: Path) -> None:
    directory.mkdir(exist_ok=True)
    subpackages = get_subpackages()
    build_summary(directory, subpackages)
    for subpackage in subpackages:
        build_subpackage(directory, subpackage)


def get_subpackages() -> list[str]:
    python = Path(__file__).parent.parent
    directory = python / "brayns"
    return sorted(
        child.name
        for child in directory.glob("*")
        if child.is_dir()
        if not child.name.startswith("__")
    )


def build_summary(directory: Path, subpackages: list[str]) -> None:
    path = directory / API_FILENAME
    summary = format_subpackages_summary(subpackages)
    data = API.format(subpackages=summary)
    with path.open("wt") as file:
        file.write(data)


def format_subpackages_summary(subpackages: list[str]) -> str:
    return "\n".join(f"    {subpackage}" for subpackage in subpackages)


def build_subpackage(directory: Path, subpackage: str) -> None:
    path = directory / f"{subpackage}.rst"
    data = format_subpackage(subpackage)
    with path.open("wt") as file:
        file.write(data)


def format_subpackage(subpackage: str) -> str:
    obj: ModuleType = getattr(brayns, subpackage)
    return SUBPACKAGE.format(
        title=subpackage,
        underline=len(subpackage) * "=",
        module=f"brayns.{subpackage}",
        items=format_subpackage_modules(obj),
    )


def format_subpackage_modules(subpackage: ModuleType) -> str:
    group = group_items_by_module(subpackage)
    return "\n\n".join(format_module(module, items) for module, items in group.items())


def group_items_by_module(subpackage: ModuleType) -> dict[str, list[str]]:
    items: list[str] = subpackage.__all__
    items = [item for item in items if item in brayns.__all__]
    result = defaultdict[str, list[str]](list)
    for item in items:
        obj = getattr(subpackage, item)
        result[obj.__module__].append(item)
    return result


def format_module(module: str, items: list[str]) -> str:
    title = module.split(".")[-1]
    return AUTOMODULE.format(
        title=title,
        underline=len(title) * "-",
        module=module,
        members=", ".join(items),
    )


if __name__ == "__main__":
    build_from_argv()

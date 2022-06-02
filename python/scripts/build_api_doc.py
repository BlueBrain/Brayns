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

import brayns

HEADER = '''
{plugin} API methods
----------------

This page references the entrypoints of the {plugin} plugin.

The Core plugin is always loaded so the related entrypoints are always
available.

Other plugins like CircuitExplorer or DTI might register additional entrypoints
but they must be loaded when starting the renderer instance to be available.

The functions below are generated using the entrypoints of the Core plugin.

All arguments are keyword arguments extracted from the entrypoint params.

Renderer errors will be raised with an instance of brayns.ReplyError. This one
can be used to extract error code, description and some optional additional data
(mainly used to store JSON errors).
'''.strip()


def build_from_uri(uri: str, directory: str) -> None:
    with brayns.connect(uri) as instance:
        build_from_instance(instance, directory)


def build_from_instance(instance: brayns.Instance, directory: str) -> None:
    entrypoints = brayns.Entrypoint.get_all(instance)
    return build_from_entrypoints(entrypoints, directory)


def build_from_entrypoints(entrypoints: list[brayns.Entrypoint], directory: str) -> None:
    pass


if __name__ == '__main__':
    uri = 'localhost:5000'
    directory = pathlib.Path(__file__).parent.parent / 'doc' / 'source'
    argv = sys.argv
    if len(argv) > 1:
        uri = argv[1]
    if len(argv) > 2:
        directory = pathlib.Path(argv[2])
    build_from_uri(uri, str(directory))

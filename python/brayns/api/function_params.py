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

"""Helper module to format function params of an entrypoint."""

from .entrypoint import Entrypoint
from .schema import Schema


def from_entrypoint(entrypoint: Entrypoint):
    """Create a string representing function params of an entrypoint.

    Example: {
        params: {a: int, b: str}
    }
    gives 'a: int, b: str'

    :param entrypoint: Function source
    :type entrypoint: Entrypoint
    :return: Function params as string
    :rtype: str
    """
    return ', '.join(
        _format_param(param)
        for param in entrypoint.params
    )


def _format_param(param: Schema):
    name = param.name
    typename = param.typename
    default = '' if param.required else ' = None'
    return f'{name}: {typename}{default}'

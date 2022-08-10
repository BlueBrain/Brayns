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

from .color3 import Color3


def parse_hex_color(value: str) -> Color3:
    """Parse an hexadecimal color string to Color3.

    The string can be just digits (0a12f5), prefixed with a hash (#0a12f5) or
    with 0x (0x0a12f5).

    :param value: Color code.
    :type value: str
    :return: Color parsed.
    :rtype: Color3
    """
    value = _sanitize(value)
    return Color3(
        _normalize(value[0:2]),
        _normalize(value[2:4]),
        _normalize(value[4:6])
    )


def _sanitize(value: str) -> str:
    size = len(value)
    if size == 6:
        return value
    if size == 7 and value[0] == '#':
        return value[1:]
    if size == 8 and value[:2].lower() == '0x':
        return value[2:]
    raise ValueError(f'Not an hex color {value}')


def _normalize(value: str) -> float:
    return int(value, base=16) / 255

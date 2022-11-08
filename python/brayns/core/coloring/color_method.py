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

from dataclasses import dataclass

from brayns.utils import Color4


@dataclass
class ColorMethod:
    """Store information to color a model using different techniques.

    Use one of the factory methods to instanciate it.

    Primitive are identified from 0 in the order they have been loaded.

    IDs can be concatenated using commas for sequences and dashes for ranges.

    Example: '1,3,5-7,9'

    Supported color methods depend on the model type.

    :param name: Color method name.
    :type name: str
    :param colors: Mapping to retreive color from method value.
    :type name: dict[str, Color4]
    """

    name: str
    colors: dict[str, Color4]

    @staticmethod
    def single_color(color: Color4) -> ColorMethod:
        """Color model by uniform color."""
        return ColorMethod('solid', {'color': color})

    @staticmethod
    def by_geometry(colors: dict[str, Color4]) -> ColorMethod:
        """Color model by geometry ID (str)."""
        return ColorMethod('primitive', colors)

    @staticmethod
    def by_triangle(colors: dict[str, Color4]) -> ColorMethod:
        """Color model by triangle ID (str)."""
        return ColorMethod('triangle', colors)

    @staticmethod
    def by_vertex(colors: dict[str, Color4]) -> ColorMethod:
        """Color model by vertex ID (str)."""
        return ColorMethod('vertex', colors)

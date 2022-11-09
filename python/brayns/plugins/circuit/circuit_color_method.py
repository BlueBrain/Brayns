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

from brayns.core import ColorMethod
from brayns.utils import Color4


class CircuitColorMethod:
    """Available coloring methods for circuits."""

    @staticmethod
    def id(colors: dict[str, Color4]) -> ColorMethod:
        """Color model by element ID."""
        return ColorMethod('element id', colors)

    @staticmethod
    def etype(colors: dict[str, Color4]) -> ColorMethod:
        """Color model by e-type."""
        return ColorMethod('etype', colors)

    @staticmethod
    def mtype(colors: dict[str, Color4]) -> ColorMethod:
        """Color model by m-type."""
        return ColorMethod('mtype', colors)

    @staticmethod
    def layer(colors: dict[str, Color4]) -> ColorMethod:
        """Color model by layer."""
        return ColorMethod('layer', colors)

    @staticmethod
    def region(colors: dict[str, Color4]) -> ColorMethod:
        """Color model by region."""
        return ColorMethod('region', colors)

    @staticmethod
    def hemisphere(colors: dict[str, Color4]) -> ColorMethod:
        """Color model by hemisphere."""
        return ColorMethod('hemisphere', colors)

    @staticmethod
    def morphology(colors: dict[str, Color4]) -> ColorMethod:
        """Color model by morphology."""
        return ColorMethod('morphology', colors)

    @staticmethod
    def morphology_class(colors: dict[str, Color4]) -> ColorMethod:
        """Color model by morphology class."""
        return ColorMethod('morphology class', colors)

    @staticmethod
    def morphology_section(colors: dict[str, Color4]) -> ColorMethod:
        """Color model by morphology section."""
        return ColorMethod('morphology section', colors)

    @staticmethod
    def synapse_class(colors: dict[str, Color4]) -> ColorMethod:
        """Color model by synapse class."""
        return ColorMethod('synapse class', colors)

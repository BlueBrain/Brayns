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

from abc import ABC, abstractmethod
from dataclasses import dataclass, field
from typing import Any, TypeVar

from brayns.utils import Color4

T = TypeVar('T', bound='Geometry')


@dataclass
class Geometry(ABC):
    """Base class of all geometry types.

    The color attribute is white by default and is not in the constructor, use
    with_color to chain the color settings with the construction.

    :param color: Geometry color, defaults to white.
    :type color: Color4
    """

    color: Color4 = field(default=Color4.white, init=False)

    @classmethod
    @property
    @abstractmethod
    def method(cls) -> str:
        """JSON-RPC method to add geometries of the derived type.

        :return: JSON-RPC method.
        :rtype: str
        """
        pass

    @abstractmethod
    def get_additional_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        pass

    def with_color(self: T, color: Color4) -> T:
        """Helper method to quickly change the color of the geometry.

        Example: ``sphere = brayns.Sphere(1).with_color(my_color)``.

        :param color: Geometry color.
        :type color: Color4
        :return: self
        :rtype: type(self)
        """
        self.color = color
        return self

    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            'geometry': self.get_additional_properties(),
            'color': list(self.color),
        }

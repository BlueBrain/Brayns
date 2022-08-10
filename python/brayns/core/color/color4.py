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

from collections.abc import Iterator
from dataclasses import dataclass, replace
from typing import TypeVar

from color3 import Color3

T = TypeVar('T', bound='Color4')


@dataclass(frozen=True, order=True)
class Color4(Color3):
    """Color with RGBA normalized components.

    Color4 are Color3 and can be manipulated as such but with alpha channel.

    Opaque is alpha = 1 and fully transparent is alpha = 0.

    Color3 standard colors, if used from Color4, will have an alpha of 1.

    :param a: Alpha component, defaults to 1.0.
    :type a: float, optional
    """

    a: float = 1.0

    @classmethod
    def from_color3(cls: type[T], color: Color3, alpha: float = 1.0) -> T:
        """Helper to build a Color4 from a Color3.

        :param color: Color3 to convert.
        :type color: Color3
        :param alpha: Alpha channel, defaults to 1.0
        :type alpha: float, optional
        :return: Color4 converted.
        :rtype: Color4
        """
        return cls(color.r, color.g, color.b, alpha)

    def __iter__(self) -> Iterator[float]:
        """Iterate over the color.

        :yield: Color components.
        :rtype: Iterator[float]
        """
        yield from super().__iter__()
        yield self.a

    @property
    def transparent(self) -> Color4:
        """Create an identical color but with alpha = 0.

        :return: Color but fully transparent.
        :rtype: Color4
        """
        return replace(self, a=0.0)

    @property
    def opaque(self) -> Color4:
        """Create an identical color but with alpha = 1.

        :return: Color but fully opaque.
        :rtype: Color4
        """
        return replace(self, a=1.0)

    @property
    def without_alpha(self) -> Color3:
        """Helper class to create a Color3 from a Color4.

        :return: Color3 converted.
        :rtype: Color3
        """
        return Color3(self.r, self.g, self.b)

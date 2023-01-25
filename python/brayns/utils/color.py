# Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

from typing import TypeVar

from .vector import Vector

T = TypeVar("T", bound="Color3")


class Color3(Vector[float]):
    """Color with RGB normalized components.

    Color3 are vectors and can be manipulated as such.

    :param r: Red component 0-1.
    :type r: float
    :param g: Green component 0-1.
    :type g: float
    :param b: Blue component 0-1.
    :type b: float
    """

    @classmethod
    @property
    def black(cls: type[T]) -> T:
        """Create a black color.

        :return: Black color [0, 0, 0].
        :rtype: Color3
        """
        return cls()

    @classmethod
    @property
    def white(cls: type[T]) -> T:
        """Create a white color.

        :return: White color [1, 1, 1].
        :rtype: Color3
        """
        return cls(1.0, 1.0, 1.0)

    @classmethod
    @property
    def red(cls: type[T]) -> T:
        """Create a red color.

        :return: red color [1, 0, 0].
        :rtype: Color3
        """
        return cls(1.0, 0.0, 0.0)

    @classmethod
    @property
    def green(cls: type[T]) -> T:
        """Create a green color.

        :return: green color [0, 1, 0].
        :rtype: Color3
        """
        return cls(0.0, 1.0, 0.0)

    @classmethod
    @property
    def blue(cls: type[T]) -> T:
        """Create a blue color.

        :return: blue color [0, 0, 1].
        :rtype: Color3
        """
        return cls(0.0, 0.0, 1.0)

    @classmethod
    @property
    def bbp_background(cls: type[T]) -> T:
        """Create a color of the BBP standard background (~blue).

        :return: BBP background color [0.004, 0.016, 0.102].
        :rtype: Color3
        """
        return cls(0.004, 0.016, 0.102)

    def __new__(cls: type[T], r: float = 0.0, g: float = 0.0, b: float = 0.0) -> T:
        return super().__new__(cls, r, g, b)

    @property
    def r(self) -> float:
        return self[0]

    @property
    def g(self) -> float:
        return self[1]

    @property
    def b(self) -> float:
        return self[2]


class Color4(Color3):
    """Color with RGBA normalized components.

    Color4 are Color3 and can be manipulated as such but with alpha channel.

    Opaque is alpha = 1 and fully transparent is alpha = 0.

    Color3 standard colors, if used from Color4, will have an alpha of 1.

    :param a: Alpha component, defaults to 1.0.
    :type a: float, optional
    """

    @staticmethod
    def from_color3(color: Color3, alpha: float = 1.0) -> Color4:
        """Helper to build a Color4 from a Color3.

        :param color: Color3 to convert.
        :type color: Color3
        :param alpha: Alpha channel, defaults to 1.0
        :type alpha: float, optional
        :return: Color4 converted.
        :rtype: Color4
        """
        return Color4(color.r, color.g, color.b, alpha)

    def __new__(
        cls, r: float = 0.0, g: float = 0.0, b: float = 0.0, a: float = 1.0
    ) -> Color4:
        return Vector[float].__new__(cls, r, g, b, a)

    @property
    def a(self) -> float:
        return self[3]

    @property
    def transparent(self) -> Color4:
        """Create an identical color but with alpha = 0.

        :return: Color but fully transparent.
        :rtype: Color4
        """
        return Color4(self.r, self.g, self.b, a=0.0)

    @property
    def opaque(self) -> Color4:
        """Create an identical color but with alpha = 1.

        :return: Color but fully opaque.
        :rtype: Color4
        """
        return Color4(self.r, self.g, self.b, a=1.0)

    @property
    def without_alpha(self) -> Color3:
        """Helper class to create a Color3 from a Color4.

        :return: Color3 converted.
        :rtype: Color3
        """
        return Color3(self.r, self.g, self.b)


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
        _normalize(value[0:2]), _normalize(value[2:4]), _normalize(value[4:6])
    )


def _sanitize(value: str) -> str:
    size = len(value)
    if size == 6:
        return value
    if size == 7 and value[0] == "#":
        return value[1:]
    if size == 8 and value[:2].lower() == "0x":
        return value[2:]
    raise ValueError(f"Not an hex color {value}")


def _normalize(value: str) -> float:
    return int(value, base=16) / 255

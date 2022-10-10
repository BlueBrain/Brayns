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

from typing import TypeVar

from ..vector import Vector

T = TypeVar('T', bound='Color3')


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

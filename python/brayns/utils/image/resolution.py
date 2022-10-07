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
from dataclasses import dataclass

from ..vector import Vector


@dataclass(frozen=True, order=True)
class Resolution(Vector[int]):
    """Image resolution.

    Resolution is a Vector of integers and can be handled as such.

    :param width: Image width in pixels.
    :type width: int
    :param height: Image height in pixels.
    :type height: int
    """

    width: int
    height: int

    @classmethod
    @property
    def full_hd(cls) -> Resolution:
        """Create a full HD (1920x1080) resolution.

        :return: Full HD resolution.
        :rtype: Resolution
        """
        return cls(1920, 1080)

    @classmethod
    @property
    def ultra_hd(cls) -> Resolution:
        """Create a 4K (3840x2160) resolution.

        :return: Ultra HD resolution.
        :rtype: Resolution
        """
        return 2 * cls.full_hd

    @classmethod
    @property
    def production(cls) -> Resolution:
        """Create a production (15360x8640) resolution.

        :return: Production resolution.
        :rtype: Resolution
        """
        return 8 * cls.full_hd

    def __post_init__(self) -> None:
        if not all(i > 0 for i in self):
            raise ValueError(f'Invalid resolution: {self}')

    def __iter__(self) -> Iterator[int]:
        """Iterate over width and height.

        :yield: Width and height components.
        :rtype: Iterator[int]
        """
        yield self.width
        yield self.height

    @property
    def aspect_ratio(self) -> float:
        """Get aspect ratio.

        :return: Width / height.
        :rtype: float
        """
        return self.width / self.height

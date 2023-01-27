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

from enum import Enum
from pathlib import Path

from .vector import Vector


class ImageFormat(Enum):
    """Enumeration of all supported image format.

    Values are the image format file extension without the dot.

    :param JPEG: JPEG format, smaller but lose quality.
    :param PNG: PNG format, bigger but lossless.
    """

    JPEG = "jpg"
    PNG = "png"


def parse_image_format(filename: str | Path) -> ImageFormat:
    """Parse the image format from a file path using its extension.

    Supports both string and Path input.

    :param filename: Image file path.
    :type filename: str | Path
    :return: Image format.
    :rtype: ImageFormat
    """
    path = Path(filename) if isinstance(filename, str) else filename
    extension = path.suffix[1:]
    extension = extension.lower()
    extension = extension.strip()
    return ImageFormat(extension)


class Resolution(Vector[int]):
    """Image resolution.

    Resolution is a Vector of integers and can be handled as such.

    :param width: Image width in pixels.
    :type width: int
    :param height: Image height in pixels.
    :type height: int
    """

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

    def __new__(cls, width: int, height: int) -> Resolution:
        if width <= 0 or height <= 0:
            raise ValueError(f"Invalid resolution: {width}x{height}")
        return super().__new__(cls, width, height)

    @property
    def width(self) -> int:
        return self[0]

    @property
    def height(self) -> int:
        return self[1]

    @property
    def aspect_ratio(self) -> float:
        """Get aspect ratio.

        :return: Width / height.
        :rtype: float
        """
        return self.width / self.height

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

import pathlib

from .image_format import ImageFormat


def parse_image_format(filename: str | pathlib.Path) -> ImageFormat:
    """Parse the image format from a file path using its extension.

    Supports both string and pathlib.Path input.

    :param filename: Image file path.
    :type filename: str | pathlib.Path
    :return: Image format.
    :rtype: ImageFormat
    """
    path = pathlib.Path(filename) if isinstance(filename, str) else filename
    extension = path.suffix[1:]
    extension = extension.lower()
    extension = extension.strip()
    return ImageFormat(extension)

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

from dataclasses import dataclass


@dataclass
class ImageInfo:
    """Result of an image rendering with status and encoded data.

    If nothing has been downloaded, data is empty.

    :param accumulation: Current accumulation after render.
    :type accumulation: int
    :param max_accumulation: Accumulation limit to stop rendering.
    :type max_accumulation: int
    :param data: Encoded image data, can be empty.
    :type data: bytes
    """

    accumulation: int
    max_accumulation: int
    data: bytes

    @property
    def full_quality(self) -> bool:
        """Check if max accumulation has been reached.

        :return: True if image is full quality.
        :rtype: bool
        """
        return self.accumulation == self.max_accumulation

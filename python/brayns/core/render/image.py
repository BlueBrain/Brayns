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


@dataclass
class Image:
    """Image sent by the backend with its current state.

    If the backend state doesn't require a render (accumulation =
    max_accumulation and no changes since last render), the image
    has no data and cannot be saved.

    However, accumulation settings are always provided.

    :param accumulation: Number of frames currently accumulated.
    :type accumulation: int
    :param max_accumulation: Maximum accumulation frames used by the instance.
    :type max_accumulation: int
    :param data: Image data encoded in request format, can be empty.
    :type data: bytes
    """

    accumulation: int
    max_accumulation: int
    data: bytes

    @property
    def received(self) -> bool:
        """Check if the image has been received.

        :return: True if image has been sent by the backend.
        :rtype: bool
        """
        return bool(self.data)

    @property
    def finished(self) -> bool:
        """Check if the max accumulation has been reached.

        :return: True if image has been rendered with max_accumulation samples.
        :rtype: bool
        """
        return self.accumulation == self.max_accumulation

    def save(self, path: str) -> None:
        """Save the image at given path.

        :param path: Output file path (must match render_image format).
        :type path: str
        :raises RuntimeError: Image was not sent by the backend.
        """
        if not self.received:
            raise RuntimeError('Image was not sent by the renderer')
        with open(path, 'wb') as file:
            file.write(self.data)

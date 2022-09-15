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

from dataclasses import dataclass, field
from typing import Any

from brayns.network import JsonRpcMessage
from brayns.utils import Resolution


@dataclass
class Application(JsonRpcMessage):
    """Store the application parameters of an instance.

    :param plugins: List of plugins loaded in the instance.
    :type plugins: list[str]
    :param resolution: Streaming resolution (main framebuffer).
    :type resolution: Resolution
    :param jpeg_quality: JPEG stream quality (0=lowest, 100=highest).
    :type jpeg_quality: int
    """

    plugins: list[str] = field(default_factory=list)
    resolution: Resolution = Resolution.full_hd
    jpeg_quality: int = 100

    def update(self, obj: dict[str, Any]) -> None:
        """Low level API to deserialize from JSON."""
        self.plugins = obj['plugins']
        self.resolution = Resolution(*obj['viewport'])
        self.jpeg_quality = obj['jpeg_quality']

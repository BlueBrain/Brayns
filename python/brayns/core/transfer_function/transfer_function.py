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
from typing import Any

from brayns.core.common.color4 import Color4
from brayns.core.transfer_function.value_range import ValueRange


@dataclass
class TransferFunction:

    value_range: ValueRange
    colors: list[Color4]

    @staticmethod
    def deserialize(message: dict[str, Any]) -> TransferFunction:
        return TransferFunction(
            value_range=ValueRange(*message['range']),
            colors=[
                Color4(*color)
                for color in message['colors']
            ]
        )

    def serialize(self) -> dict[str, Any]:
        return {
            'range': list(self.value_range),
            'colors': [
                list(color)
                for color in self.colors
            ]
        }

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
from typing import Optional


@dataclass(frozen=True)
class BbpCells:

    density: Optional[float] = None
    targets: Optional[list[str]] = None
    gids: Optional[list[int]] = None

    @staticmethod
    def all() -> 'BbpCells':
        return BbpCells.from_density(1.0)

    @staticmethod
    def from_density(density: float) -> 'BbpCells':
        return BbpCells(density=density)

    @staticmethod
    def from_targets(targets: list[str], density: float = 1.0) -> 'BbpCells':
        return BbpCells(density=density, targets=targets)

    @staticmethod
    def from_gids(gids: list[int]) -> 'BbpCells':
        return BbpCells(gids=gids)

    def serialize(self) -> dict:
        message = {}
        if self.density is not None:
            message['percentage'] = self.density
        if self.targets is not None:
            message['targets'] = self.targets
        if self.gids is not None:
            message['gids'] = self.gids
        return message

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


@dataclass
class SonataNodes:

    density: Optional[float] = None
    names: Optional[list[str]] = None
    ids: Optional[list[int]] = None

    @staticmethod
    def all() -> 'SonataNodes':
        return SonataNodes.from_density(1.0)

    @staticmethod
    def from_density(density: float) -> 'SonataNodes':
        return SonataNodes(density=density)

    @staticmethod
    def from_names(names: list[str], density: float = 1.0) -> 'SonataNodes':
        return SonataNodes(density=density, names=names)

    @staticmethod
    def from_ids(ids: list[int]) -> 'SonataNodes':
        return SonataNodes(ids=ids)

    def serialize(self) -> dict:
        message = {}
        if self.density is not None:
            message['node_percentage'] = self.density
        if self.names is not None:
            message['node_sets'] = self.names
        if self.ids is not None:
            message['node_ids'] = self.ids
        return message

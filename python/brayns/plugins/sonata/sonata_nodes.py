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
class SonataNodes:
    """Specify how to load SONATA nodes.

    Use one of the factory methods to create this object.
    """

    density: float | None = None
    names: list[str] | None = None
    ids: list[int] | None = None

    @staticmethod
    def all() -> SonataNodes:
        """Load all nodes."""
        return SonataNodes.from_density(1.0)

    @staticmethod
    def default() -> SonataNodes:
        """Load a reasonable proportion of nodes."""
        return SonataNodes.from_density(0.01)

    @staticmethod
    def from_density(density: float) -> SonataNodes:
        """Load a proportion of all nodes (0 = none, 1 = all)."""
        return SonataNodes(density=density)

    @staticmethod
    def from_names(names: list[str], density: float = 1.0) -> SonataNodes:
        """Load from node group names with optional density."""
        return SonataNodes(density=density, names=names)

    @staticmethod
    def from_ids(ids: list[int]) -> SonataNodes:
        """Load nodes with given IDs."""
        return SonataNodes(ids=ids)

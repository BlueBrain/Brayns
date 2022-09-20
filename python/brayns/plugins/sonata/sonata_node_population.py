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

from ..morphology import Morphology
from .sonata_edge_population import SonataEdgePopulation
from .sonata_nodes import SonataNodes
from .sonata_report import SonataReport


@dataclass
class SonataNodePopulation:
    """SONATA node population parameters.

    :param name: Population name.
    :type name: str
    :param nodes: Nodes to load, defaults to 0.01% of all nodes.
    :type nodes: SonataNodes, optional
    :param report: Report to load, defaults to None.
    :type report: SonataReport | None, optional
    :param edges: Edges to load, defaults to None.
    :type edges: list[SonataEdgePopulation] | None, optional
    :param morphology: How to load the morphologies, default constructed.
    :type morphology: Morphology, optional
    :param vasculature_radius_multiplier: Vasculature scale, defaults to 1.
    :type vasculature_radius_multiplier: float, optional
    """

    name: str
    nodes: SonataNodes = field(default_factory=SonataNodes.default)
    report: SonataReport | None = None
    edges: list[SonataEdgePopulation] | None = None
    morphology: Morphology = field(default_factory=Morphology)
    vasculature_radius_multiplier: float = 1.0

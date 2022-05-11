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

from brayns.plugins.sonata.sonata_edge_population import SonataEdgePopulation
from brayns.plugins.sonata.sonata_nodes import SonataNodes
from brayns.plugins.sonata.sonata_report import SonataReport


@dataclass
class SonataNodePopulation:

    name: str
    nodes: SonataNodes = SonataNodes.from_density(0.01)
    report: Optional[SonataReport] = None
    edges: Optional[list[SonataEdgePopulation]] = None
    neuron_radius_multiplier: float = 1.0
    vasculature_radius_multiplier: float = 1.0
    load_soma: bool = True
    load_axon: bool = False
    load_dendrites: bool = False

    def serialize(self) -> dict:
        message = {
            'node_population': self.name,
            'neuron_morphology_parameters': {
                'radius_multiplier': self.neuron_radius_multiplier,
                'load_soma': self.load_soma,
                'load_axon': self.load_axon,
                'load_dendrites': self.load_dendrites
            },
            'vasculature_geometry_parameters': {
                'radius_multiplier': self.vasculature_radius_multiplier
            }
        }
        message.update(self.nodes.serialize())
        if self.report is not None:
            message.update(self.report.serialize())
        if self.edges is not None:
            message['edge_populations'] = [
                edge.serialize()
                for edge in self.edges
            ]
        return message

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

from brayns.core.model.model_loader import ModelLoader
from brayns.plugins.bbp.bbp_cells import BbpCells
from brayns.plugins.bbp.bbp_report import BbpReport


@dataclass
class BbpLoader(ModelLoader):

    cells: BbpCells = BbpCells.all()
    report: Optional[BbpReport] = None
    radius_multiplier: float = 1.0
    load_soma: bool = True
    load_axon: bool = False
    load_dendrites: bool = False
    load_afferent_synapses: bool = False
    load_efferent_synapses: bool = False

    @staticmethod
    def for_soma_only() -> 'BbpLoader':
        return BbpLoader(
            cells=BbpCells.from_density(0.1),
            radius_multiplier=10.0
        )

    @staticmethod
    def for_morphology() -> 'BbpLoader':
        return BbpLoader(
            cells=BbpCells.from_density(0.001),
            load_dendrites=True
        )

    @classmethod
    @property
    def name(cls) -> str:
        return 'BBP loader'

    @property
    def properties(self) -> dict:
        properties = {
            'neuron_morphology_parameters': {
                'radius_multiplier': self.radius_multiplier,
                'load_soma': self.load_soma,
                'load_axon': self.load_axon,
                'load_dendrites': self.load_dendrites
            },
            'load_afferent_synapses': self.load_afferent_synapses,
            'load_efferent_synapses': self.load_efferent_synapses
        }
        properties.update(self.cells.serialize())
        if self.report is not None:
            properties.update(self.report.serialize())
        return properties

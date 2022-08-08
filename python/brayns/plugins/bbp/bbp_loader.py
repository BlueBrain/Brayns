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

from brayns.core.model.model_loader import ModelLoader
from brayns.plugins.bbp.bbp_cells import BbpCells
from brayns.plugins.bbp.bbp_report import BbpReport
from brayns.plugins.common.morphology_parameters import MorphologyParameters


@dataclass
class BbpLoader(ModelLoader):

    cells: BbpCells = BbpCells.all()
    report: BbpReport | None = None
    morphology: MorphologyParameters = field(
        default_factory=MorphologyParameters
    )
    load_afferent_synapses: bool = False
    load_efferent_synapses: bool = False

    @classmethod
    @property
    def name(cls) -> str:
        return 'BBP loader'

    @property
    def properties(self) -> dict[str, Any]:
        properties: dict[str, Any] = {
            'load_afferent_synapses': self.load_afferent_synapses,
            'load_efferent_synapses': self.load_efferent_synapses
        }
        properties.update(self.cells.serialize())
        properties['neuron_morphology_parameters'] = self.morphology.serialize()
        if self.report is not None:
            properties.update(self.report.serialize())
        return properties

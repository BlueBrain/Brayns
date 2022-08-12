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

from brayns.core import Loader

from ..morphology import Morphology
from .bbp_cells import BbpCells
from .bbp_report import BbpReport


@dataclass
class BbpLoader(Loader):
    """BlueConfig / CircuitConfig loader.

    Use this loader to load circuit files with BBP internal format.

    :param cells: Cells to load, defaults to all.
    :type cells: BbpCells, optional
    :param report: Report to load, defaults to None.
    :type report: BbpReport | None, optional
    :param morphology: Specifies how to load the circuit morphologies.
    :type morphology: Morphology, optional
    :param load_afferent_synapses: Wether to load afferent, defaults to False.
    :type load_afferent_synapses: bool, optional
    :param load_efferent_synapses: Wether to load efferent, defaults to False.
    :type load_efferent_synapses: bool, optional
    """

    cells: BbpCells = BbpCells.all()
    report: BbpReport | None = None
    morphology: Morphology = field(default_factory=Morphology)
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

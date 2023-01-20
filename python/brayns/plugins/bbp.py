# Copyright (c) 2015-2023 EPFL/Blue Brain Project
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
from enum import Enum
from typing import Any, ClassVar

from brayns.core import Loader

from .morphology import Morphology, serialize_morphology


@dataclass
class BbpCells:
    """BlueConfig cells selection.

    Use one of the factory methods to create this object.
    """

    density: float | None = None
    targets: list[str] | None = None
    gids: list[int] | None = None

    @staticmethod
    def all() -> BbpCells:
        """Select all cells from a circuit."""
        return BbpCells.from_density(1.0)

    @staticmethod
    def from_density(density: float) -> BbpCells:
        """Select only a proportion of cells (0 = None, 1 = all)."""
        return BbpCells(density=density)

    @staticmethod
    def from_targets(targets: list[str], density: float = 1.0) -> BbpCells:
        """Select only the cells from a given target.

        The density of cells to load per target can also be specified.
        """
        return BbpCells(density=density, targets=targets)

    @staticmethod
    def from_gids(gids: list[int]) -> BbpCells:
        """Select cells using their GIDs."""
        return BbpCells(gids=gids)


class BbpReportType(Enum):
    """All available BBP report types."""

    NONE = "none"
    SPIKES = "spikes"
    COMPARTMENT = "compartment"


@dataclass
class BbpReport:
    """BlueConfig report selection.

    Use one of the factory methods to create this object.
    """

    type: BbpReportType
    name: str | None = None
    spike_transition_time: float | None = None

    @staticmethod
    def none() -> BbpReport:
        """No reports to load."""
        return BbpReport(
            type=BbpReportType.NONE,
        )

    @staticmethod
    def spikes(spike_transition_time: float = 1.0) -> BbpReport:
        """Spike report with optional transition time.

        time unit of transition depends on the report but it can be used to make
        this transition faster or slower (ie 2 = twice slower).
        """
        return BbpReport(
            type=BbpReportType.SPIKES,
            spike_transition_time=spike_transition_time,
        )

    @staticmethod
    def compartment(name: str) -> BbpReport:
        """Compartment report with given name."""
        return BbpReport(
            type=BbpReportType.COMPARTMENT,
            name=name,
        )


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

    BLUE_CONFIG: ClassVar[str] = "BlueConfig"
    CIRCUIT_CONFIG: ClassVar[str] = "CircuitConfig"

    cells: BbpCells = field(default_factory=BbpCells.all)
    report: BbpReport | None = None
    morphology: Morphology = field(default_factory=Morphology)
    load_afferent_synapses: bool = False
    load_efferent_synapses: bool = False

    @classmethod
    @property
    def name(cls) -> str:
        return "BBP loader"

    def get_properties(self) -> dict[str, Any]:
        return _serialize_loader(self)


def _serialize_loader(loader: BbpLoader) -> dict[str, Any]:
    message: dict[str, Any] = {
        "load_afferent_synapses": loader.load_afferent_synapses,
        "load_efferent_synapses": loader.load_efferent_synapses,
        **_serialize_cells(loader.cells),
        "neuron_morphology_parameters": serialize_morphology(loader.morphology),
    }
    if loader.report is not None:
        report = _serialize_report(loader.report)
        message.update(report)
    return message


def _serialize_cells(cells: BbpCells) -> dict[str, Any]:
    message = dict[str, Any]()
    if cells.density is not None:
        message["percentage"] = cells.density
    if cells.targets is not None:
        message["targets"] = cells.targets
    if cells.gids is not None:
        message["gids"] = cells.gids
    return message


def _serialize_report(report: BbpReport) -> dict[str, Any]:
    message: dict[str, Any] = {
        "report_type": report.type.value,
    }
    if report.name is not None:
        message["report_name"] = report.name
    if report.spike_transition_time is not None:
        message["spike_transition_time"] = report.spike_transition_time
    return message

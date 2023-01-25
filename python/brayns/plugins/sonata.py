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


class SonataReportType(Enum):
    """All available SONATA report types."""

    NONE = "none"
    SPIKES = "spikes"
    COMPARTMENT = "compartment"
    SUMMATION = "summation"
    SYNAPSE = "synapse"
    BLOODFLOW_PRESSURE = "bloodflow_pressure"
    BLOODFLOW_SPEED = "bloodflow_speed"
    BLOODFLOW_RADII = "bloodflow_radii"


@dataclass
class SonataReport:
    """Describe a SONATA report to load.

    Use one of the factory methods to create this object.
    """

    type: SonataReportType
    name: str | None = None
    spike_transition_time: float | None = None

    @staticmethod
    def none() -> SonataReport:
        """No report to load."""
        return SonataReport(
            type=SonataReportType.NONE,
        )

    @staticmethod
    def spikes(spike_transition_time: float = 1.0) -> SonataReport:
        """Spike report with transition time (units from report)."""
        return SonataReport(
            type=SonataReportType.SPIKES, spike_transition_time=spike_transition_time
        )

    @staticmethod
    def compartment(name: str) -> SonataReport:
        """Compartment report with given name."""
        return SonataReport(
            type=SonataReportType.COMPARTMENT,
            name=name,
        )

    @staticmethod
    def summation(name: str) -> SonataReport:
        """Simulation report with given name."""
        return SonataReport(
            type=SonataReportType.SUMMATION,
            name=name,
        )

    @staticmethod
    def synapse(name: str) -> SonataReport:
        """Synapse report with given name."""
        return SonataReport(
            type=SonataReportType.SYNAPSE,
            name=name,
        )

    @staticmethod
    def bloodflow_pressure(name: str) -> SonataReport:
        """Bloodflow pressure report with given name."""
        return SonataReport(
            type=SonataReportType.BLOODFLOW_PRESSURE,
            name=name,
        )

    @staticmethod
    def bloodflow_speed(name: str) -> SonataReport:
        """Bloodflow speed report with given name."""
        return SonataReport(
            type=SonataReportType.BLOODFLOW_SPEED,
            name=name,
        )

    @staticmethod
    def bloodflow_radii(name: str) -> SonataReport:
        """Bloodflow radii report with given name."""
        return SonataReport(
            type=SonataReportType.BLOODFLOW_RADII,
            name=name,
        )


@dataclass
class SonataEdgePopulation:
    """SONATA edge population parameters.

    :param name: Population name.
    :type name: str
    :param afferent: If these edges are afferent or efferent ones.
    :type afferent: bool
    :param afferent: If these edges are afferent ones.
    :type afferent: bool
    :param density: Proportion of edges to load, defaults to 1.
    :type density: float, optional
    :param radius: Edge radii, defaults to 2.
    :type radius: float, optional
    :param report: Report name, defaults to None.
    :type report: str | None, optional
    """

    name: str
    afferent: bool
    density: float = 1.0
    radius: float = 2.0
    report: str | None = None


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


@dataclass
class SonataLoader(Loader):
    """Loader for SONATA circuit files.

    :param node_populations: Nodes to load.
    :type node_populations: list[SonataNodePopulation]
    """

    JSON: ClassVar[str] = "json"

    node_populations: list[SonataNodePopulation]

    @classmethod
    @property
    def name(cls) -> str:
        return "SONATA loader"

    def get_properties(self) -> dict[str, Any]:
        return _serialize_loader(self)


def _serialize_loader(loader: SonataLoader) -> dict[str, Any]:
    return {
        "node_population_settings": [
            _serialize_node_population(node_population)
            for node_population in loader.node_populations
        ],
    }


def _serialize_node_population(population: SonataNodePopulation) -> dict[str, Any]:
    message: dict[str, Any] = {
        "node_population": population.name,
        "vasculature_geometry_parameters": {
            "radius_multiplier": population.vasculature_radius_multiplier,
        },
        **_serialize_nodes(population.nodes),
        "neuron_morphology_parameters": serialize_morphology(population.morphology),
    }
    if population.report is not None:
        report = _serialize_report(population.report)
        message.update(report)
    if population.edges is not None:
        message["edge_populations"] = [
            _serialize_edge_population(edge) for edge in population.edges
        ]
    return message


def _serialize_nodes(nodes: SonataNodes) -> dict[str, Any]:
    message = dict[str, Any]()
    if nodes.density is not None:
        message["node_percentage"] = nodes.density
    if nodes.names is not None:
        message["node_sets"] = nodes.names
    if nodes.ids is not None:
        message["node_ids"] = nodes.ids
    return message


def _serialize_report(report: SonataReport) -> dict[str, Any]:
    message: dict[str, Any] = {
        "report_type": report.type.value,
    }
    if report.name is not None:
        message["report_name"] = report.name
    if report.spike_transition_time is not None:
        message["spike_transition_time"] = report.spike_transition_time
    return message


def _serialize_edge_population(population: SonataEdgePopulation) -> dict[str, Any]:
    message: dict[str, Any] = {
        "edge_population": population.name,
        "load_afferent": population.afferent,
        "edge_percentage": population.density,
        "radius": population.radius,
    }
    if population.report is not None:
        message["edge_report_name"] = population.report
    return message

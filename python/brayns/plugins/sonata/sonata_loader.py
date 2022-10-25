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
from typing import Any, ClassVar

from brayns.core import Loader

from ..morphology import serialize_morphology
from .sonata_edge_population import SonataEdgePopulation
from .sonata_node_population import SonataNodePopulation
from .sonata_nodes import SonataNodes
from .sonata_report import SonataReport


@dataclass
class SonataLoader(Loader):
    """Loader for SONATA circuit files.

    :param node_populations: Nodes to load.
    :type node_populations: list[SonataNodePopulation]
    """

    JSON: ClassVar[str] = 'json'

    node_populations: list[SonataNodePopulation]

    @classmethod
    @property
    def name(cls) -> str:
        return 'SONATA loader'

    def get_properties(self) -> dict[str, Any]:
        return _serialize_loader(self)


def _serialize_loader(loader: SonataLoader) -> dict[str, Any]:
    return {
        'node_population_settings': [
            _serialize_node_population(node_population)
            for node_population in loader.node_populations
        ],
    }


def _serialize_node_population(population: SonataNodePopulation) -> dict[str, Any]:
    message: dict[str, Any] = {
        'node_population': population.name,
        'vasculature_geometry_parameters': {
            'radius_multiplier': population.vasculature_radius_multiplier,
        },
        **_serialize_nodes(population.nodes),
        'neuron_morphology_parameters': serialize_morphology(population.morphology),
    }
    if population.report is not None:
        report = _serialize_report(population.report)
        message.update(report)
    if population.edges is not None:
        message['edge_populations'] = [
            _serialize_edge_population(edge)
            for edge in population.edges
        ]
    return message


def _serialize_nodes(nodes: SonataNodes) -> dict[str, Any]:
    message = dict[str, Any]()
    if nodes.density is not None:
        message['node_percentage'] = nodes.density
    if nodes.names is not None:
        message['node_sets'] = nodes.names
    if nodes.ids is not None:
        message['node_ids'] = nodes.ids
    return message


def _serialize_report(report: SonataReport) -> dict[str, Any]:
    message: dict[str, Any] = {
        'report_type': report.type.value,
    }
    if report.name is not None:
        message['report_name'] = report.name
    if report.spike_transition_time is not None:
        message['spike_transition_time'] = report.spike_transition_time
    return message


def _serialize_edge_population(population: SonataEdgePopulation) -> dict[str, Any]:
    message: dict[str, Any] = {
        'edge_population': population.name,
        'load_afferent': population.afferent,
        'edge_percentage': population.density,
        'radius': population.radius,
    }
    if population.report is not None:
        message['edge_report_name'] = population.report
    return message

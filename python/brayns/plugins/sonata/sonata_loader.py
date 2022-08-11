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
from typing import Any

from brayns.core import Loader

from .sonata_node_population import SonataNodePopulation


@dataclass
class SonataLoader(Loader):
    """Loader for SONATA circuit files.

    :param node_populations: Nodes to load.
    :type node_populations: list[SonataNodePopulation]
    :param simulation_config: Override configuration file, defaults to None.
    :type simulation_config: str | None
    """

    node_populations: list[SonataNodePopulation]
    simulation_config: str | None = None

    @classmethod
    @property
    def name(cls) -> str:
        return 'SONATA loader'

    @property
    def properties(self) -> dict[str, Any]:
        properties: dict[str, Any] = {
            'node_population_settings': [
                node_population.serialize()
                for node_population in self.node_populations
            ]
        }
        if self.simulation_config is not None:
            properties['simulation_config_path'] = self.simulation_config
        return properties

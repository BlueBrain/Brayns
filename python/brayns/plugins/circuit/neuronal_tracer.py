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

from brayns.network import Instance
from brayns.utils import Color4


@dataclass
class NeuronalTracer:
    """Helper class to trace neuron connections.

    :param source_cells: List of source cells by ID.
    :type source_cells: list[int]
    :param target_cells: List of target cells by ID.
    :type target_cells: list[int]
    :param source_color: Color of the source cells.
    :type source_color: Color4
    :param connected_color: Color of the cells connected to source.
    :type connected_color: Color4
    :param non_connected_color: Color of the cells not connected to source.
    :type non_connected_color: Color4
    """

    source_cells: list[int]
    target_cells: list[int]
    source_color: Color4
    connected_color: Color4
    non_connected_color: Color4

    def trace_anterograde(self, instance: Instance, model_id: int) -> None:
        """Color cells of given circuit.

        :param instance: Instance.
        :type instance: Instance
        :param model_id: Circuit model ID.
        :type model_id: int
        """
        params = {
            'model_id': model_id,
            'cell_gids': self.source_cells,
            'target_cell_gids': self.target_cells,
            'source_cell_color': list(self.source_color),
            'connected_cells_color': list(self.connected_color),
            'non_connected_cells_color': list(self.non_connected_color),
        }
        instance.request('trace-anterograde', params)

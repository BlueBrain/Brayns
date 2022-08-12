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

from brayns.instance import Instance
from brayns.utils import Color4

from .cell_id import CellId


def color_circuit_by_id(
    instance: Instance,
    model_id: int,
    colors: dict[CellId, Color4],
) -> list[int]:
    """Color a circuit from a mapping cell ID -> Color.

    :param instance: Instance.
    :type instance: Instance
    :param model_id: Circuit model ID.
    :type model_id: int
    :param colors: Color mappings Cells -> Color.
    :type colors: dict[CellId, Color4]
    :return: List of GIDs that were not colored.
    :rtype: list[int]
    """
    params = {
        'model_id': model_id,
        'color_info': [
            {
                'variable': id.value,
                'color': list(color)
            }
            for id, color in colors.items()
        ]
    }
    return instance.request('color-circuit-by-id', params)

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

from brayns.network import Instance
from brayns.utils import Color4


def color_model(instance: Instance, model_id: int, method: str, colors: dict[str, Color4]) -> None:
    """Color a model by the given method.

    This function needs a method name and a mapping to get a color from a
    method value.

    For example to color a circuit by GID the mapping can be:

    .. code-block:: python

        colors = {
            '1': brayns.Color4.red,
            '2': brayns.Color4.blue,
        }

    Where 1 and 2 are the method values (here GIDs) mapped to the color that
    must be applied to them.

    Supported methods depend on the plugins loaded and the model type. See
    ``get_color_methods`` and ``get_color_values`` for more details.

    When coloring by ID (GIDs, triangle, geometry, ...) the IDs start at zero
    and are incremented in the order of the loading. They can be concatenated
    in a single value using comma and dashes.

    Example: '0,2,4-6,8'.

    :param instance: Instance.
    :type instance: Instance
    :param model_id: ID of the model to color.
    :type model_id: int
    :param method: Coloring method to use.
    :type method: str
    :param colors: Color to use per method value.
    :type colors: dict[str, Color4]
    """
    params = {
        'id': model_id,
        'method': method,
        'values': {
            key: list(value)
            for key, value in colors.items()
        },
    }
    instance.request('color-model', params)

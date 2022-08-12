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

from .color_method import ColorMethod


def color_circuit_by_method(
    instance: Instance,
    model_id: int,
    method: ColorMethod,
    colors: dict[str, Color4],
) -> None:
    """Color a circuit using a given coloring method.

    Use a mapping from method value (str) to color.

    Available methods and method values can be queried for a given model using
    `get_color_methods` and `get_color_method_values`.

    :param instance: Instance.
    :type instance: Instance
    :param model_id: Circuit model ID.
    :type model_id: int
    :param method: Coloring method.
    :type method: ColorMethod
    :param colors: Mapping method value -> Color.
    :type colors: dict[str, Color4]
    """
    params = {
        'model_id': model_id,
        'method': method.value,
        'color_info': [
            {
                'variable': value,
                'color': list(color)
            }
            for value, color in colors.items()
        ]
    }
    instance.request('color-circuit-by-method', params)

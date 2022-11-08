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
    """Color a circuit by the given method using given color map.

    The color map gives the color to apply for a given method value.

    Available coloring methods depend on the model type (mesh, circuit, etc).

    See ``get_color_methods`` and ``get_color_method_values`` for more details.

    :param instance: Instance.
    :type instance: Instance
    :param model_id: ID of the model to color.
    :type model_id: int
    :param method: Coloring method to use.
    :type method: str
    :param colors: Color map from method value to color.
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

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

from .color_method import ColorMethod


def color_model(instance: Instance, model_id: int, method: ColorMethod) -> None:
    """Color a model by the given method.

    Color method is usually a mapping from method value to color.

    For example to color a circuit by GID the method colors can be {
        '1': brayns.Color4.red,
        '2': brayns.Color4.blue,
    }

    Where 1 and 2 are the method values (here GIDs) mapped to the color that
    must be applied to them.

    Supported methods depend on the plugins loaded in the service instance and
    the model type. See ``get_color_methods`` and ``get_color_method_values``
    for more details.

    :param instance: Instance.
    :type instance: Instance
    :param model_id: ID of the model to color.
    :type model_id: int
    :param method: Coloring method to use.
    :type method: ColorMethod
    """
    params = {
        'id': model_id,
        'method': method.name,
        'values': {
            key: list(value)
            for key, value in method.colors.items()
        },
    }
    instance.request('color-model', params)

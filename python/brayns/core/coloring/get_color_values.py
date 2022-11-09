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


def get_color_values(instance: Instance, model_id: int, method: str) -> list[str]:
    """Get available color values for a given method and model.

    Color values are the name of the elements to color. For example, the color
    method 'layer' has values ['1', '2', '3'] if the model has 3 layers. 

    :param instance: Instance.
    :type instance: Instance
    :param model_id: Model ID.
    :type model_id: int
    :param method: Coloring method name.
    :type method: str
    :return: Available color values.
    :rtype: list[str]
    """
    params = {
        'id': model_id,
        'method': method,
    }
    return instance.request('get-color-values', params)

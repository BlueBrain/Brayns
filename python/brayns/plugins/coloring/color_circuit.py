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

from typing import Any

from brayns.network import Instance
from brayns.utils import Color4


def color_circuit(instance: Instance, model_id: int, color: Color4) -> None:
    """Color a circuit by the given color.

    :param instance: Instance.
    :type instance: Instance
    :param model_id: Circuit model ID.
    :type model_id: int
    :param color: Circuit color.
    :type color: Color4
    """
    params = _serialize_color(model_id, color)
    instance.request('color-circuit-by-single-color', params)


def _serialize_color(model_id: int, color: Color4) -> dict[str, Any]:
    return {
        'model_id': model_id,
        'color': list(color)
    }

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

from .color_ramp import ColorRamp
from .serialize_color_ramp import serialize_color_ramp


def set_color_ramp(instance: Instance, model_id: int, ramp: ColorRamp) -> None:
    """Set the current color ramp of the given model.

    :param instance: Instance.
    :type instance: Instance
    :param model_id: Model ID.
    :type model_id: int
    :param color_ramp: Color ramp.
    :type color_ramp: ColorRamp
    """
    params = {
        'id': model_id,
        'color_ramp': serialize_color_ramp(ramp),
    }
    instance.request('set-color-ramp', params)

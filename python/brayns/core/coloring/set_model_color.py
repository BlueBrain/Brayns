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

from .color_method import ColorMethod
from .color_model import color_model


def set_model_color(instance: Instance, model_id: int, color: Color4) -> None:
    """Shortcut to color a model using a single color.

    :param instance: Instance.
    :type instance: Instance
    :param model_id: ID of the model to color.
    :type model_id: int
    :param color: Color to apply on model.
    :type color: Color4
    """
    color_model(instance, model_id, ColorMethod.SOLID, {'color': color})

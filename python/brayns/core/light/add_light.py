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

from .light import Light
from ..model import Model, deserialize_model


def add_light(instance: Instance, light: Light) -> Model:
    """Add a given light to an instance and return its model.

    :param instance: Instance.
    :type instance: Instance
    :param light: Light to add.
    :type light: Light
    :return: Light model.
    :rtype: Model
    """
    name = light.name
    params = light.get_properties()
    result = instance.request(f'add-light-{name}', params)
    return deserialize_model(result)

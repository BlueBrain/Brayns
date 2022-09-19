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

from .material import Material


def set_material(instance: Instance, model_id: int, material: Material) -> None:
    """Apply the given material to the given model.

    :param instance: Instance.
    :type instance: Instance
    :param model_id: Model ID.
    :type model_id: int
    :param material: Material to apply on model.
    :type material: Material
    """
    name = material.name
    params = {
        'model_id': model_id,
        'material': material.get_properties(),
    }
    instance.request(f'set-material-{name}', params)

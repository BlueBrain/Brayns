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


def set_circuit_thickness(instance: Instance, model_id: int, radius_multiplier: float) -> None:
    """Multiply the radius of all primitives of a circuit by given factor.

    :param instance: Instance.
    :type instance: Instance
    :param model_id: Circuit model ID.
    :type model_id: int
    :param radius_multiplier: Scaling factor for capsules and spheres.
    :type radius_multiplier: float
    """
    params = {
        'model_id': model_id,
        'radius_multiplier': radius_multiplier,
    }
    instance.request('set-circuit-thickness', params)

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


def enable_simulation(instance: Instance, model_id: int, enabled: bool) -> None:
    """Enable the simulation coloring for the given model.

    If enabled, the colors of the model are the one of the simulation at the
    current frame (see ``get_simulation`` and ``set_simulation_frame``).

    If disabled, the colors of the model are set manually or by default.

    :param instance: Instance.
    :type instance: Instance
    :param model_id: Model ID.
    :type model_id: int
    :param enabled: Simulation coloring enabled for given model.
    :type enabled: bool
    """
    params = {
        'model_id': model_id,
        'enabled': enabled,
    }
    instance.request('enable-simulation', params)

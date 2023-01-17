# Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

from .simulation import Simulation
from .time_unit import TimeUnit


def get_simulation(instance: Instance) -> Simulation:
    """Retreive the current simulation state of an instance.

    :param instance: Instance.
    :type instance: Instance
    :return: Current simulation state.
    :rtype: Simulation
    """
    result = instance.request("get-simulation-parameters")
    return _deserialize_simulation(result)


def _deserialize_simulation(message: dict[str, Any]) -> Simulation:
    return Simulation(
        start_frame=message["start_frame"],
        end_frame=message["end_frame"],
        current_frame=message["current"],
        delta_time=message["dt"],
        time_unit=TimeUnit(message["unit"]),
    )

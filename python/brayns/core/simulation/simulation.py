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

from dataclasses import dataclass
from typing import Optional

from brayns.core.simulation.time_unit import TimeUnit
from brayns.instance.instance import Instance


@dataclass
class Simulation:

    start_frame: int
    end_frame: int
    current_frame: int
    delta_time: float
    time_unit: TimeUnit

    @staticmethod
    def from_instance(instance: Instance) -> 'Simulation':
        result = instance.request('get-animation-parameters')
        return Simulation.deserialize(result)

    @staticmethod
    def deserialize(message: dict) -> 'Simulation':
        return Simulation(
            start_frame=message['start_frame'],
            end_frame=message['end_frame'],
            current_frame=message['current'],
            delta_time=message['dt'],
            time_unit=TimeUnit(message['unit'])
        )

    @staticmethod
    def set_current_frame(instance: Instance, index: int) -> None:
        params = {'current': index}
        instance.request('set-animation-parameters', params)

    @staticmethod
    def enable(instance: Instance, model_id: int, enabled: bool) -> None:
        params = {
            'model_id': model_id,
            'enabled': enabled
        }
        instance.request('enable-simulation', params)

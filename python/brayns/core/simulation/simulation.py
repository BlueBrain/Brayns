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

from __future__ import annotations

from dataclasses import dataclass
from typing import Any

from brayns.core.simulation.time_unit import TimeUnit


@dataclass
class Simulation:

    start_frame: int
    end_frame: int
    current_frame: int
    delta_time: float
    time_unit: TimeUnit

    @staticmethod
    def deserialize(message: dict[str, Any]) -> Simulation:
        return Simulation(
            start_frame=message['start_frame'],
            end_frame=message['end_frame'],
            current_frame=message['current'],
            delta_time=message['dt'],
            time_unit=TimeUnit(message['unit'])
        )

    @property
    def frame_count(self) -> int:
        return self.end_frame - self.start_frame

    @property
    def duration(self) -> float:
        return self.frame_count * self.delta_time

    @property
    def fps(self) -> float:
        return 1 / self.delta_time * self.time_unit.per_second

    def clamp(self, frame: int) -> int:
        return min(max(frame, self.start_frame), self.end_frame)

    def get_frame(self, time: float) -> int:
        return round(time / self.delta_time)

    def get_time(self, frame: int) -> float:
        return frame * self.delta_time

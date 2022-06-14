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
from typing import Iterator

from brayns.core.simulation.simulation import Simulation


@dataclass
class MovieFrames:

    fps: float = 25.0
    slowing_factor: float = 1.0
    start_frame: int = 0
    end_frame: int = -1

    def get_indices(self, simulation: Simulation) -> list[int]:
        start_frame = self._get_frame(simulation, self.start_frame)
        end_frame = self._get_frame(simulation, self.end_frame)
        step = self._get_step(simulation)
        return list(self._generate_frames(start_frame, end_frame, step))

    def _get_start_frame(self, simulation: Simulation) -> int:
        return simulation.clamp(self.start_frame)

    def _get_end_frame(self, simulation: Simulation) -> int:
        end_frame = self.end_frame
        if end_frame < 0:
            end_frame += simulation.end_frame
        return simulation.clamp(end_frame)

    def _get_frame(self, simulation: Simulation, frame: int) -> int:
        if frame < 0:
            frame += simulation.end_frame
        return simulation.clamp(frame)

    def _get_step(self, simulation: Simulation) -> float:
        return simulation.fps / self.fps / self.slowing_factor

    def _generate_frames(self, start_frame: int, end_frame: int, step: float) -> Iterator[int]:
        i = float(start_frame)
        while i <= end_frame:
            yield int(i)
            i += step

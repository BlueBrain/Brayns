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
        return list(range(start_frame, end_frame + 1, step))

    def _get_frame(self, simulation: Simulation, frame: int) -> int:
        if frame < 0:
            frame += simulation.end_frame
        return simulation.clamp(frame)

    def _get_step(self, simulation: Simulation) -> int:
        return int(simulation.fps / self.fps / self.slowing_factor)

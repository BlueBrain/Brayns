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

from brayns.core.simulation import Simulation


@dataclass
class MovieFrames:
    """Helper class to generate frame indices.

    Use the movie specification (FPS, slow motion, frame range) to generate
    the indices of the simulation frames which needs to be rendered.

    Slowing factor is compared to real time (2 = twice slower).

    Start and end frames are clamped to the simulation limits and are threated
    like Python indices (ie -N is len - N).

    The time step generated is constant so if the FPS ratio between the movie
    and the simulation is not an integer, it will be truncated.

    :param fps: Movie FPS.
    :type fps: float
    :param slowing_factor: Slowing factor.
    :type slowing_factor: float
    :param start_frame: First frame of the movie.
    :type start_frame: int
    :param end_frame: Last frame of the movie.
    :type end_frame: int
    """

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

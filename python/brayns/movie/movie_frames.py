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

from collections.abc import Iterator
from dataclasses import dataclass

from brayns.core import Simulation


@dataclass
class MovieFrames:
    """Helper class to generate frame indices.

    Use the movie specification (FPS, slow motion, frame range) to generate
    the indices of the simulation frames which needs to be rendered.

    Slowing factor is compared to real time (2 = twice slower).

    Start and end frames are clamped to the simulation limits and are threated
    like Python indices (i.e. -1 is simulation.end_frame).

    The index step is computed as simulation_fps / fps / slowing_factor. If it
    is smaller than 1, then some frames will be duplicated to match the target
    FPS.

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
        start = _get_frame(simulation, self.start_frame)
        end = _get_frame(simulation, self.end_frame)
        step = _get_step(simulation, self)
        return list(_yield_indices(start, end, step))


def _get_frame(simulation: Simulation, frame: int) -> int:
    if frame < 0:
        frame += simulation.end_frame + 1
    return simulation.clamp(frame)


def _get_step(simulation: Simulation, frames: MovieFrames) -> float:
    return simulation.fps / frames.fps / frames.slowing_factor


def _yield_indices(start: int, end: int, step: float) -> Iterator[int]:
    current = float(start)
    while True:
        if current > end:
            return
        yield round(current)
        current += step

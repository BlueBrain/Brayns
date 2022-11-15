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

from .time_unit import TimeUnit


@dataclass
class Simulation:
    """Simulation state.

    :param start_frame: Index of the first frame of the simulation.
    :type start_frame: int
    :param end_frame: Index of the last frame of the simulation.
    :type end_frame: int
    :param current_frame: Index of the current frame of the simulation.
    :type current_frame: int
    :param delta_time: Delta time in ``time_unit`` between two frames.
    :type delta_time: float
    :param time_unit: Time unit, always milliseconds.
    :type time_unit: TimeUnit
    """

    start_frame: int
    end_frame: int
    current_frame: int
    delta_time: float
    time_unit: TimeUnit

    @property
    def frame_count(self) -> int:
        """Number of frames in the simulation.

        :return: Frame count.
        :rtype: int
        """
        return self.end_frame - self.start_frame + 1

    @property
    def duration(self) -> float:
        """Simulation duration in ``time_unit``.

        :return: Duration.
        :rtype: float
        """
        return (self.end_frame - self.start_frame) * self.delta_time

    @property
    def fps(self) -> float:
        """Simulation FPS.

        :return: FPS.
        :rtype: float
        """
        return 1 / self.delta_time * self.time_unit.per_second

    def clamp(self, frame: int) -> int:
        """Clamp given frame index inside simulation limits.

        :param frame: Frame index.
        :type frame: int
        :return: Clamped frame index.
        :rtype: int
        """
        return min(max(frame, self.start_frame), self.end_frame)

    def get_frame(self, time: float) -> int:
        """Convert timestamp in ``time_unit`` to frame index.

        Result is not clamped to simulation limits.

        :param time: Timestep in ``time_unit``.
        :type time: float
        :return: Frame index.
        :rtype: int
        """
        return round(time / self.delta_time)

    def get_time(self, frame: int) -> float:
        """Convert frame index to a timestep in ``time_unit``.

        Result is not clamped to simulation limits.

        :param frame: Frame index.
        :type frame: int
        :return: Timestep in ``time_unit``.
        :rtype: float
        """
        return frame * self.delta_time

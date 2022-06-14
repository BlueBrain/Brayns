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

import unittest

from brayns.core.movie.movie_frames import MovieFrames
from brayns.core.simulation.simulation import Simulation
from brayns.core.simulation.time_unit import TimeUnit


class TestMovieFrames(unittest.TestCase):

    def test_get_indices(self) -> None:
        frames = MovieFrames(
            fps=10,
            slowing_factor=10,
            start_frame=5000,
            end_frame=-4000
        )
        simulation = Simulation(
            start_frame=0,
            end_frame=10000,
            current_frame=2,
            delta_time=0.1,
            time_unit=TimeUnit.MILLISECOND
        )
        ref = list(range(5000, 6100, 100))
        indices = frames.get_indices(simulation)
        self.assertEqual(indices, ref)


if __name__ == '__main__':
    unittest.main()

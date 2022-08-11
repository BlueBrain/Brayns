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

import brayns

from .mock_simulation import MockSimulation


class TestSimulation(unittest.TestCase):

    def test_deserialize(self) -> None:
        test = brayns.Simulation.deserialize(MockSimulation.message)
        self.assertEqual(test, MockSimulation.simulation)

    def test_frame_count(self) -> None:
        self.assertEqual(MockSimulation.simulation.frame_count, 10)

    def test_duration(self) -> None:
        self.assertEqual(MockSimulation.simulation.duration, 1)

    def test_fps(self) -> None:
        self.assertEqual(MockSimulation.simulation.fps, 10000)

    def test_clamp(self) -> None:
        self.assertEqual(MockSimulation.simulation.clamp(-1), 0)
        self.assertEqual(MockSimulation.simulation.clamp(0), 0)
        self.assertEqual(MockSimulation.simulation.clamp(1), 1)
        self.assertEqual(MockSimulation.simulation.clamp(10), 10)
        self.assertEqual(MockSimulation.simulation.clamp(11), 10)

    def test_get_frame(self) -> None:
        self.assertEqual(MockSimulation.simulation.get_frame(0), 0)
        self.assertEqual(MockSimulation.simulation.get_frame(0.1), 1)
        self.assertEqual(MockSimulation.simulation.get_frame(0.5), 5)

    def test_get_time(self) -> None:
        self.assertEqual(MockSimulation.simulation.get_time(0), 0)
        self.assertEqual(MockSimulation.simulation.get_time(1), 0.1)
        self.assertEqual(MockSimulation.simulation.get_time(5), 0.5)


if __name__ == '__main__':
    unittest.main()

# Copyright (c) 2015-2024 EPFL/Blue Brain Project
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
from typing import Any

import brayns
from tests.mock_instance import MockInstance


class TestSimulation(unittest.TestCase):
    def test_get_simulation(self) -> None:
        instance = MockInstance(self.mock_simulation_message())
        test = brayns.get_simulation(instance)
        self.assertEqual(test, self.mock_simulation())
        self.assertEqual(instance.method, "get-simulation-parameters")
        self.assertIsNone(instance.params)

    def test_set_simulation_frame(self) -> None:
        instance = MockInstance()
        brayns.set_simulation_frame(instance, 5)
        self.assertEqual(instance.method, "set-simulation-parameters")
        self.assertEqual(instance.params, {"current": 5})

    def test_enable_simulation(self) -> None:
        instance = MockInstance()
        brayns.enable_simulation(instance, 0, True)
        self.assertEqual(instance.method, "enable-simulation")
        self.assertEqual(instance.params, {"model_id": 0, "enabled": True})

    def test_frame_count(self) -> None:
        simulation = self.mock_simulation()
        self.assertEqual(simulation.frame_count, 11)

    def test_duration(self) -> None:
        simulation = self.mock_simulation()
        self.assertEqual(simulation.duration, 1)

    def test_fps(self) -> None:
        simulation = self.mock_simulation()
        self.assertEqual(simulation.fps, 10000)

    def test_clamp(self) -> None:
        simulation = self.mock_simulation()
        self.assertEqual(simulation.clamp(-1), 0)
        self.assertEqual(simulation.clamp(0), 0)
        self.assertEqual(simulation.clamp(1), 1)
        self.assertEqual(simulation.clamp(10), 10)
        self.assertEqual(simulation.clamp(11), 10)

    def test_get_frame(self) -> None:
        simulation = self.mock_simulation()
        self.assertEqual(simulation.get_frame(0), 0)
        self.assertEqual(simulation.get_frame(0.1), 1)
        self.assertEqual(simulation.get_frame(0.5), 5)

    def test_get_time(self) -> None:
        simulation = self.mock_simulation()
        self.assertEqual(simulation.get_time(0), 0)
        self.assertEqual(simulation.get_time(1), 0.1)
        self.assertEqual(simulation.get_time(5), 0.5)

    def mock_simulation(self) -> brayns.Simulation:
        return brayns.Simulation(
            start_frame=0,
            end_frame=10,
            current_frame=5,
            delta_time=0.1,
            time_unit=brayns.TimeUnit.MILLISECOND,
        )

    def mock_simulation_message(self) -> dict[str, Any]:
        return {
            "start_frame": 0,
            "end_frame": 10,
            "current": 5,
            "dt": 0.1,
            "unit": "ms",
        }

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

from brayns.core.simulation.simulation import Simulation
from brayns.core.simulation.time_unit import TimeUnit
from tests.instance.mock_instance import MockInstance


class TestSimulation(unittest.TestCase):

    def setUp(self) -> None:
        self._simulation = Simulation(
            start_frame=0,
            end_frame=10,
            current_frame=5,
            delta_time=0.1,
            time_unit=TimeUnit.MILLISECOND
        )
        self._message = {
            'start_frame': 0,
            'end_frame': 10,
            'current': 5,
            'dt': 0.1,
            'unit': 'ms'
        }

    def test_from_instance(self) -> None:
        instance = MockInstance(self._message)
        test = Simulation.from_instance(instance)
        self.assertEqual(test, self._simulation)
        self.assertEqual(instance.method, 'get-simulation-parameters')
        self.assertEqual(instance.params, None)

    def test_deserialize(self) -> None:
        test = Simulation.deserialize(self._message)
        self.assertEqual(test, self._simulation)

    def test_set_current_frame(self) -> None:
        instance = MockInstance()
        Simulation.set_current_frame(instance, 5)
        self.assertEqual(instance.method, 'set-simulation-parameters')
        self.assertEqual(instance.params, {'current': 5})

    def test_enable(self) -> None:
        instance = MockInstance()
        Simulation.enable(instance, 0, True)
        self.assertEqual(instance.method, 'enable-simulation')
        self.assertEqual(instance.params, {
            'model_id': 0,
            'enabled': True
        })

    def test_frame_count(self) -> None:
        self.assertEqual(self._simulation.frame_count, 10)

    def test_duration(self) -> None:
        self.assertEqual(self._simulation.duration, 1)

    def test_fps(self) -> None:
        self.assertEqual(self._simulation.fps, 10000)

    def test_clamp(self) -> None:
        self.assertEqual(self._simulation.clamp(-1), 0)
        self.assertEqual(self._simulation.clamp(0), 0)
        self.assertEqual(self._simulation.clamp(1), 1)
        self.assertEqual(self._simulation.clamp(10), 10)
        self.assertEqual(self._simulation.clamp(11), 10)

    def test_get_frame(self) -> None:
        self.assertEqual(self._simulation.get_frame(0), 0)
        self.assertEqual(self._simulation.get_frame(0.1), 1)
        self.assertEqual(self._simulation.get_frame(0.5), 5)

    def test_get_time(self) -> None:
        self.assertEqual(self._simulation.get_time(0), 0)
        self.assertEqual(self._simulation.get_time(1), 0.1)
        self.assertEqual(self._simulation.get_time(5), 0.5)


if __name__ == '__main__':
    unittest.main()

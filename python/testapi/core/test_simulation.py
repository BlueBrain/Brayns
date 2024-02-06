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

import brayns
from testapi.loading import load_sonata_circuit
from testapi.simple_test_case import SimpleTestCase


class TestSimulation(SimpleTestCase):
    def test_get_simulation(self) -> None:
        load_sonata_circuit(self, report=True)
        test = brayns.get_simulation(self.instance)
        self.assertEqual(test.start_frame, 0)
        self.assertEqual(test.end_frame, 2)
        self.assertEqual(test.current_frame, 0)
        self.assertAlmostEqual(test.delta_time, 1)
        self.assertEqual(test.time_unit, brayns.TimeUnit.MILLISECOND)

    def test_set_simulation_frame(self) -> None:
        load_sonata_circuit(self, report=True)
        brayns.set_simulation_frame(self.instance, 2)
        simulation = brayns.get_simulation(self.instance)
        self.assertEqual(simulation.current_frame, 2)

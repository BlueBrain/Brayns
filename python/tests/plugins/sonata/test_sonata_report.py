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


class TestSonataNodes(unittest.TestCase):

    def test_none(self) -> None:
        test = brayns.SonataReport.none()
        self.assertEqual(test.type, brayns.SonataReportType.NONE)
        self.assertIsNone(test.name)
        self.assertIsNone(test.spike_transition_time)

    def test_spikes(self) -> None:
        test = brayns.SonataReport.spikes(0.5)
        self.assertEqual(test.type, brayns.SonataReportType.SPIKES)
        self.assertIsNone(test.name)
        self.assertEqual(test.spike_transition_time, 0.5)

    def test_compartment(self) -> None:
        test = brayns.SonataReport.compartment('test')
        self.assertEqual(test.type, brayns.SonataReportType.COMPARTMENT)
        self.assertEqual(test.name, 'test')
        self.assertIsNone(test.spike_transition_time)

    def test_summation(self) -> None:
        test = brayns.SonataReport.summation('test')
        self.assertEqual(test.type, brayns.SonataReportType.SUMMATION)
        self.assertEqual(test.name, 'test')
        self.assertIsNone(test.spike_transition_time)

    def test_synapse(self) -> None:
        test = brayns.SonataReport.synapse('test')
        self.assertEqual(test.type, brayns.SonataReportType.SYNAPSE)
        self.assertEqual(test.name, 'test')
        self.assertIsNone(test.spike_transition_time)

    def test_bloodflow_pressure(self) -> None:
        test = brayns.SonataReport.bloodflow_pressure('test')
        self.assertEqual(test.type, brayns.SonataReportType.BLOODFLOW_PRESSURE)
        self.assertEqual(test.name, 'test')
        self.assertIsNone(test.spike_transition_time)

    def test_bloodflow_speed(self) -> None:
        test = brayns.SonataReport.bloodflow_speed('test')
        self.assertEqual(test.type, brayns.SonataReportType.BLOODFLOW_SPEED)
        self.assertEqual(test.name, 'test')
        self.assertIsNone(test.spike_transition_time)

    def test_bloodflow_radii(self) -> None:
        test = brayns.SonataReport.bloodflow_radii('test')
        self.assertEqual(test.type, brayns.SonataReportType.BLOODFLOW_RADII)
        self.assertEqual(test.name, 'test')
        self.assertIsNone(test.spike_transition_time)

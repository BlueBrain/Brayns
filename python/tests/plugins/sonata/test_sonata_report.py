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

from brayns.plugins.sonata.sonata_report import SonataReport


class TestSonataNodes(unittest.TestCase):

    def test_none(self) -> None:
        test = SonataReport.none()
        self.assertEqual(test.type, 'none')
        self.assertIsNone(test.name)
        self.assertIsNone(test.spike_transition_time)

    def test_spikes(self) -> None:
        test = SonataReport.spikes(0.5)
        self.assertEqual(test.type, 'spikes')
        self.assertIsNone(test.name)
        self.assertEqual(test.spike_transition_time, 0.5)

    def test_compartment(self) -> None:
        test = SonataReport.compartment('test')
        self.assertEqual(test.type, 'compartment')
        self.assertEqual(test.name, 'test')
        self.assertIsNone(test.spike_transition_time)

    def test_summation(self) -> None:
        test = SonataReport.summation('test')
        self.assertEqual(test.type, 'summation')
        self.assertEqual(test.name, 'test')
        self.assertIsNone(test.spike_transition_time)

    def test_synapse(self) -> None:
        test = SonataReport.synapse('test')
        self.assertEqual(test.type, 'synapse')
        self.assertEqual(test.name, 'test')
        self.assertIsNone(test.spike_transition_time)

    def test_bloodflow_pressure(self) -> None:
        test = SonataReport.bloodflow_pressure('test')
        self.assertEqual(test.type, 'bloodflow_pressure')
        self.assertEqual(test.name, 'test')
        self.assertIsNone(test.spike_transition_time)

    def test_bloodflow_speed(self) -> None:
        test = SonataReport.bloodflow_speed('test')
        self.assertEqual(test.type, 'bloodflow_speed')
        self.assertEqual(test.name, 'test')
        self.assertIsNone(test.spike_transition_time)

    def test_bloodflow_radii(self) -> None:
        test = SonataReport.bloodflow_radii('test')
        self.assertEqual(test.type, 'bloodflow_radii')
        self.assertEqual(test.name, 'test')
        self.assertIsNone(test.spike_transition_time)

    def test_serialize(self) -> None:
        test = SonataReport.bloodflow_pressure('test')
        ref = {
            'report_type': 'bloodflow_pressure',
            'report_name': 'test'
        }
        self.assertEqual(test.serialize(), ref)


if __name__ == '__main__':
    unittest.main()

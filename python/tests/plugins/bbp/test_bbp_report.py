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

from brayns.plugins.bbp.bbp_report import BbpReport


class TestBbpReport(unittest.TestCase):

    def test_none(self) -> None:
        report = BbpReport.none()
        self.assertEqual(report.type, 'none')
        self.assertIsNone(report.name)
        self.assertIsNone(report.spike_transition_time)

    def test_spikes(self) -> None:
        report = BbpReport.spikes()
        self.assertEqual(report.type, 'spikes')
        self.assertIsNone(report.name)
        self.assertEqual(report.spike_transition_time, 1.0)

    def test_spikes_with_time(self) -> None:
        spike_transition_time = 0.5
        report = BbpReport.spikes(spike_transition_time)
        self.assertEqual(report.type, 'spikes')
        self.assertIsNone(report.name)
        self.assertEqual(report.spike_transition_time, spike_transition_time)

    def test_compartment(self) -> None:
        name = 'test'
        report = BbpReport.compartment(name)
        self.assertEqual(report.type, 'compartment')
        self.assertEqual(report.name, name)
        self.assertIsNone(report.spike_transition_time)

    def test_serialize(self) -> None:
        test = BbpReport.compartment('test')
        ref = {
            'report_type': 'compartment',
            'report_name': 'test'
        }
        self.assertEqual(test.serialize(), ref)


if __name__ == '__main__':
    unittest.main()

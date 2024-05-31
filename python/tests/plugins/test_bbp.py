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

import brayns


class TestBbpCells(unittest.TestCase):
    def test_all(self) -> None:
        cells = brayns.BbpCells.all()
        self.assertEqual(cells, brayns.BbpCells.from_density(1))

    def test_from_density(self) -> None:
        density = 0.5
        cells = brayns.BbpCells.from_density(density)
        self.assertEqual(cells.density, density)
        self.assertIsNone(cells.targets)
        self.assertIsNone(cells.gids)

    def test_from_targets(self) -> None:
        targets = ["1", "2", "3"]
        cells = brayns.BbpCells.from_targets(targets)
        self.assertEqual(cells.density, 1.0)
        self.assertEqual(cells.targets, targets)
        self.assertIsNone(cells.gids)

    def test_from_targets_and_density(self) -> None:
        targets = ["1", "2", "3"]
        density = 0.5
        cells = brayns.BbpCells.from_targets(targets, density)
        self.assertEqual(cells.density, density)
        self.assertEqual(cells.targets, targets)
        self.assertIsNone(cells.gids)

    def test_from_gids(self) -> None:
        gids = [1, 2, 3]
        cells = brayns.BbpCells.from_gids(gids)
        self.assertIsNone(cells.density)
        self.assertIsNone(cells.targets)
        self.assertEqual(cells.gids, gids)


class TestBbpReport(unittest.TestCase):
    def test_none(self) -> None:
        report = brayns.BbpReport.none()
        self.assertEqual(report.type, brayns.BbpReportType.NONE)
        self.assertIsNone(report.name)
        self.assertIsNone(report.spike_transition_time)

    def test_spikes(self) -> None:
        report = brayns.BbpReport.spikes()
        self.assertEqual(report.type, brayns.BbpReportType.SPIKES)
        self.assertIsNone(report.name)
        self.assertEqual(report.spike_transition_time, 1.0)

    def test_spikes_with_time(self) -> None:
        spike_transition_time = 0.5
        report = brayns.BbpReport.spikes(spike_transition_time)
        self.assertEqual(report.type, brayns.BbpReportType.SPIKES)
        self.assertIsNone(report.name)
        self.assertEqual(report.spike_transition_time, spike_transition_time)

    def test_compartment(self) -> None:
        name = "test"
        report = brayns.BbpReport.compartment(name)
        self.assertEqual(report.type, brayns.BbpReportType.COMPARTMENT)
        self.assertEqual(report.name, name)
        self.assertIsNone(report.spike_transition_time)


class TestBbpLoader(unittest.TestCase):
    def test_name(self) -> None:
        self.assertEqual(brayns.BbpLoader.name, "BBP loader")

    def test_get_properties(self) -> None:
        loader = brayns.BbpLoader(
            cells=brayns.BbpCells.from_targets(["1", "2"], 0.5),
            report=brayns.BbpReport.compartment("test"),
            morphology=brayns.Morphology(
                radius_multiplier=3,
                load_soma=True,
                load_axon=True,
                load_dendrites=True,
                geometry_type=brayns.GeometryType.ORIGINAL,
                resampling=2.0,
                subsampling=1,
            ),
            load_afferent_synapses=True,
            load_efferent_synapses=True,
        )
        self.assertEqual(
            loader.get_properties(),
            {
                "percentage": 0.5,
                "targets": ["1", "2"],
                "report_type": "compartment",
                "report_name": "test",
                "load_afferent_synapses": True,
                "load_efferent_synapses": True,
                "neuron_morphology_parameters": {
                    "radius_multiplier": 3.0,
                    "load_soma": True,
                    "load_axon": True,
                    "load_dendrites": True,
                    "geometry_type": "original",
                    "resampling": 2.0,
                    "subsampling": 1,
                    "growth": 1,
                },
            },
        )

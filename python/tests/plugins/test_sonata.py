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


class TestSonataNodes(unittest.TestCase):
    def test_all(self) -> None:
        test = brayns.SonataNodes.all()
        self.assertEqual(test.density, 1)
        self.assertIsNone(test.names)
        self.assertIsNone(test.ids)

    def test_default(self) -> None:
        test = brayns.SonataNodes.default()
        ref = brayns.SonataNodes.from_density(0.01)
        self.assertEqual(test, ref)

    def test_from_density(self) -> None:
        density = 0.3
        test = brayns.SonataNodes.from_density(density)
        self.assertEqual(test.density, density)
        self.assertIsNone(test.names)
        self.assertIsNone(test.ids)

    def test_from_names(self) -> None:
        names = ["test1", "test2"]
        density = 0.3
        test = brayns.SonataNodes.from_names(names, density)
        self.assertEqual(test.density, density)
        self.assertEqual(test.names, names)
        self.assertIsNone(test.ids)

    def test_from_ids(self) -> None:
        ids = [1, 2, 3]
        test = brayns.SonataNodes.from_ids(ids)
        self.assertIsNone(test.density)
        self.assertIsNone(test.names)
        self.assertEqual(test.ids, ids)


class TestSonataReport(unittest.TestCase):
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
        test = brayns.SonataReport.compartment("test")
        self.assertEqual(test.type, brayns.SonataReportType.COMPARTMENT)
        self.assertEqual(test.name, "test")
        self.assertIsNone(test.spike_transition_time)

    def test_summation(self) -> None:
        test = brayns.SonataReport.summation("test")
        self.assertEqual(test.type, brayns.SonataReportType.SUMMATION)
        self.assertEqual(test.name, "test")
        self.assertIsNone(test.spike_transition_time)

    def test_synapse(self) -> None:
        test = brayns.SonataReport.synapse("test")
        self.assertEqual(test.type, brayns.SonataReportType.SYNAPSE)
        self.assertEqual(test.name, "test")
        self.assertIsNone(test.spike_transition_time)

    def test_bloodflow_pressure(self) -> None:
        test = brayns.SonataReport.bloodflow_pressure("test")
        self.assertEqual(test.type, brayns.SonataReportType.BLOODFLOW_PRESSURE)
        self.assertEqual(test.name, "test")
        self.assertIsNone(test.spike_transition_time)

    def test_bloodflow_speed(self) -> None:
        test = brayns.SonataReport.bloodflow_speed("test")
        self.assertEqual(test.type, brayns.SonataReportType.BLOODFLOW_SPEED)
        self.assertEqual(test.name, "test")
        self.assertIsNone(test.spike_transition_time)

    def test_bloodflow_radii(self) -> None:
        test = brayns.SonataReport.bloodflow_radii("test")
        self.assertEqual(test.type, brayns.SonataReportType.BLOODFLOW_RADII)
        self.assertEqual(test.name, "test")
        self.assertIsNone(test.spike_transition_time)


class TestSonataLoader(unittest.TestCase):
    def test_name(self) -> None:
        self.assertEqual(brayns.SonataLoader.name, "SONATA loader")

    def test_properties(self) -> None:
        loader = brayns.SonataLoader(
            node_populations=[
                brayns.SonataNodePopulation(
                    name="test1",
                    nodes=brayns.SonataNodes.from_ids([1, 2, 3]),
                    report=brayns.SonataReport.compartment("report1"),
                    edges=[
                        brayns.SonataEdgePopulation(
                            name="edges1",
                            afferent=True,
                            density=0.5,
                            radius=3,
                            report="report2",
                        ),
                        brayns.SonataEdgePopulation("edges2", afferent=False),
                    ],
                    morphology=brayns.Morphology(),
                    vasculature_radius_multiplier=2,
                ),
                brayns.SonataNodePopulation("test2"),
            ]
        )
        self.assertEqual(
            loader.get_properties(),
            {
                "node_population_settings": [
                    {
                        "node_population": "test1",
                        "node_ids": [1, 2, 3],
                        "report_type": "compartment",
                        "report_name": "report1",
                        "edge_populations": [
                            {
                                "edge_population": "edges1",
                                "load_afferent": True,
                                "edge_percentage": 0.5,
                                "radius": 3,
                                "edge_report_name": "report2",
                            },
                            {
                                "edge_population": "edges2",
                                "load_afferent": False,
                                "edge_percentage": 1,
                                "radius": 2,
                            },
                        ],
                        "neuron_morphology_parameters": {
                            "radius_multiplier": 1,
                            "load_soma": True,
                            "load_axon": False,
                            "load_dendrites": False,
                            "geometry_type": "smooth",
                            "resampling": 2.0,
                            "subsampling": 1,
                        },
                        "vasculature_geometry_parameters": {
                            "radius_multiplier": 2,
                        },
                    },
                    {
                        "node_population": "test2",
                        "node_percentage": 0.01,
                        "neuron_morphology_parameters": {
                            "radius_multiplier": 1,
                            "load_soma": True,
                            "load_axon": False,
                            "load_dendrites": False,
                            "geometry_type": "smooth",
                            "resampling": 2.0,
                            "subsampling": 1,
                        },
                        "vasculature_geometry_parameters": {
                            "radius_multiplier": 1,
                        },
                    },
                ],
            },
        )

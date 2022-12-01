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


class TestSonataLoader(unittest.TestCase):

    def test_name(self) -> None:
        self.assertEqual(brayns.SonataLoader.name, 'SONATA loader')

    def test_properties(self) -> None:
        loader = brayns.SonataLoader(
            node_populations=[
                brayns.SonataNodePopulation(
                    name='test1',
                    nodes=brayns.SonataNodes.from_ids([1, 2, 3]),
                    report=brayns.SonataReport.compartment('report1'),
                    edges=[
                        brayns.SonataEdgePopulation(
                            name='edges1',
                            afferent=True,
                            density=0.5,
                            radius=3,
                            report='report2',
                        ),
                        brayns.SonataEdgePopulation('edges2', afferent=False),
                    ],
                    morphology=brayns.Morphology(),
                    vasculature_radius_multiplier=2,
                ),
                brayns.SonataNodePopulation('test2'),
            ]
        )
        self.assertEqual(loader.get_properties(), {
            'node_population_settings': [
                {
                    'node_population': 'test1',
                    'node_ids': [1, 2, 3],
                    'report_type': 'compartment',
                    'report_name': 'report1',
                    'edge_populations': [
                        {
                            'edge_population': 'edges1',
                            'load_afferent': True,
                            'edge_percentage': 0.5,
                            'radius': 3,
                            'edge_report_name': 'report2',
                        },
                        {
                            'edge_population': 'edges2',
                            'load_afferent': False,
                            'edge_percentage': 1,
                            'radius': 2,
                        },
                    ],
                    'neuron_morphology_parameters': {
                        'radius_multiplier': 1,
                        'load_soma': True,
                        'load_axon': False,
                        'load_dendrites': False,
                        'geometry_type': 'smooth',
                        'resampling': 2.0,
                        'subsampling': 1,
                    },
                    'vasculature_geometry_parameters': {
                        'radius_multiplier': 2,
                    },
                },
                {
                    'node_population': 'test2',
                    'node_percentage': 0.01,
                    'neuron_morphology_parameters': {
                        'radius_multiplier': 1,
                        'load_soma': True,
                        'load_axon': False,
                        'load_dendrites': False,
                        'geometry_type': 'smooth',
                        'resampling': 2.0,
                        'subsampling': 1,
                    },
                    'vasculature_geometry_parameters': {
                        'radius_multiplier': 1,
                    }
                }
            ],
        })

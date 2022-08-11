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


class TestSonataNodePopulation(unittest.TestCase):

    def test_serialize(self) -> None:
        test = brayns.SonataNodePopulation(
            name='test',
            nodes=brayns.SonataNodes.from_density(0.1),
            report=brayns.SonataReport.bloodflow_pressure('report'),
            edges=[
                brayns.SonataEdgePopulation('test1', afferent=True),
                brayns.SonataEdgePopulation('test2', afferent=False)
            ],
            morphology=brayns.Morphology(
                radius_multiplier=3,
                load_soma=False,
                load_axon=True,
                load_dendrites=True,
                geometry_type=brayns.GeometryType.ORIGINAL
            ),
            vasculature_radius_multiplier=4
        )
        ref = {
            'node_population': 'test',
            'node_percentage': 0.1,
            'report_type': 'bloodflow_pressure',
            'report_name': 'report',
            'edge_populations': [
                {
                    'edge_population': 'test1',
                    'load_afferent': True,
                    'edge_percentage': 1,
                    'radius': 2
                },
                {
                    'edge_population': 'test2',
                    'load_afferent': False,
                    'edge_percentage': 1,
                    'radius': 2
                }
            ],
            'neuron_morphology_parameters': {
                'radius_multiplier': 3,
                'load_soma': False,
                'load_axon': True,
                'load_dendrites': True,
                'geometry_type': 'original'
            },
            'vasculature_geometry_parameters': {
                'radius_multiplier': 4
            }
        }
        self.assertEqual(test.serialize(), ref)


if __name__ == '__main__':
    unittest.main()

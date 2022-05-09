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

from brayns.plugins.sonata.sonata_loader import SonataLoader
from brayns.plugins.sonata.sonata_node_population import SonataNodePopulation


class TestSonataLoader(unittest.TestCase):

    def test_name(self) -> None:
        self.assertEqual(SonataLoader.name, 'SONATA loader')

    def test_properties(self) -> None:
        loader = SonataLoader(
            node_populations=[
                SonataNodePopulation('test1'),
                SonataNodePopulation('test2')
            ],
            simulation_config='test'
        )
        properties = {
            'node_population_settings': [
                {
                    'node_population': 'test1',
                    'node_percentage': 0.01,
                    'neuron_morphology_parameters': {
                        'radius_multiplier': 1,
                        'load_soma': True,
                        'load_axon': False,
                        'load_dendrites': False
                    },
                    'vasculature_geometry_parameters': {
                        'radius_multiplier': 1
                    }
                },
                {
                    'node_population': 'test2',
                    'node_percentage': 0.01,
                    'neuron_morphology_parameters': {
                        'radius_multiplier': 1,
                        'load_soma': True,
                        'load_axon': False,
                        'load_dendrites': False
                    },
                    'vasculature_geometry_parameters': {
                        'radius_multiplier': 1
                    }
                }
            ],
            'simulation_config_path': 'test'
        }
        self.assertEqual(loader.properties, properties)


if __name__ == '__main__':
    unittest.main()

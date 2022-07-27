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

from brayns.plugins.bbp.bbp_cells import BbpCells
from brayns.plugins.bbp.bbp_loader import BbpLoader
from brayns.plugins.bbp.bbp_report import BbpReport
from brayns.plugins.common.morphology_geometry_type import MorphologyGeometryType
from brayns.plugins.common.morphology_parameters import MorphologyParameters


class TestBbpLoader(unittest.TestCase):

    def test_name(self) -> None:
        self.assertEqual(BbpLoader.name, 'BBP loader')

    def test_properties(self) -> None:
        loader = BbpLoader(
            cells=BbpCells.from_targets(['tests'], 0.5),
            report=BbpReport.compartment('test'),
            morphology=MorphologyParameters(
                radius_multiplier=3,
                load_soma=True,
                load_axon=True,
                load_dendrites=True,
                geometry_type=MorphologyGeometryType.ORIGINAL
            ),
            load_afferent_synapses=True,
            load_efferent_synapses=True
        )
        properties = {
            'percentage': 0.5,
            'targets': ['tests'],
            'report_type': 'compartment',
            'report_name': 'test',
            'load_afferent_synapses': True,
            'load_efferent_synapses': True,
            'neuron_morphology_parameters': {
                'radius_multiplier': 3.0,
                'load_soma': True,
                'load_axon': True,
                'load_dendrites': True,
                'geometry_type': 'original'
            }
        }
        self.assertEqual(loader.properties, properties)


if __name__ == '__main__':
    unittest.main()

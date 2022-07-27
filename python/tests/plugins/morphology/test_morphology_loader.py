# Copyright (c) 2015-2022 EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

from brayns.plugins.common import MorphologyGeometryType
from brayns.plugins.common import MorphologyParameters
from brayns.plugins.morphology import MorphologyLoader


class TestMorphologyLoader(unittest.TestCase):

    def test_name(self) -> None:
        self.assertEqual(MorphologyLoader.name, 'Neuron Morphology loader')

    def test_properties(self) -> None:
        loader = MorphologyLoader(
            morphology=MorphologyParameters(
                radius_multiplier=3,
                load_soma=True,
                load_axon=True,
                load_dendrites=True,
                geometry_type=MorphologyGeometryType.ORIGINAL
            ))
        properties = {
            'radius_multiplier': 3.0,
            'load_soma': True,
            'load_axon': True,
            'load_dendrites': True,
            'geometry_type': 'original'
        }
        self.assertEqual(loader.properties, properties)


if __name__ == '__main__':
    unittest.main()

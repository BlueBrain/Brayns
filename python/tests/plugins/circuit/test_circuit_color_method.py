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


class TestCircuitColorMethod(unittest.TestCase):

    @classmethod
    @property
    def colors(cls) -> dict[str, brayns.Color4]:
        return {
            '1': brayns.Color4.red,
            '2': brayns.Color4.blue,
        }

    def test_id(self) -> None:
        test = brayns.CircuitColorMethod.id(self.colors)
        self.assertEqual(test.name, 'element id')
        self.assertEqual(test.colors, self.colors)

    def test_etype(self) -> None:
        test = brayns.CircuitColorMethod.etype(self.colors)
        self.assertEqual(test.name, 'etype')
        self.assertEqual(test.colors, self.colors)

    def test_mtype(self) -> None:
        test = brayns.CircuitColorMethod.mtype(self.colors)
        self.assertEqual(test.name, 'mtype')
        self.assertEqual(test.colors, self.colors)

    def test_layer(self) -> None:
        test = brayns.CircuitColorMethod.layer(self.colors)
        self.assertEqual(test.name, 'layer')
        self.assertEqual(test.colors, self.colors)

    def test_region(self) -> None:
        test = brayns.CircuitColorMethod.region(self.colors)
        self.assertEqual(test.name, 'region')
        self.assertEqual(test.colors, self.colors)

    def test_hemisphere(self) -> None:
        test = brayns.CircuitColorMethod.hemisphere(self.colors)
        self.assertEqual(test.name, 'hemisphere')
        self.assertEqual(test.colors, self.colors)

    def test_morphology(self) -> None:
        test = brayns.CircuitColorMethod.morphology(self.colors)
        self.assertEqual(test.name, 'morphology')
        self.assertEqual(test.colors, self.colors)

    def test_morphology_class(self) -> None:
        test = brayns.CircuitColorMethod.morphology_class(self.colors)
        self.assertEqual(test.name, 'morphology class')
        self.assertEqual(test.colors, self.colors)

    def test_morphology_section(self) -> None:
        test = brayns.CircuitColorMethod.morphology_section(self.colors)
        self.assertEqual(test.name, 'morphology section')
        self.assertEqual(test.colors, self.colors)

    def test_synapse_class(self) -> None:
        test = brayns.CircuitColorMethod.synapse_class(self.colors)
        self.assertEqual(test.name, 'synapse class')
        self.assertEqual(test.colors, self.colors)


if __name__ == '__main__':
    unittest.main()

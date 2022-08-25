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


class TestMorphologyCli(unittest.TestCase):

    def test_parse(self) -> None:
        test = brayns.MorphologyCli(
            radius_multiplier=1,
            constant_radius=False,
            load_soma=True,
            load_dendrites=True,
            load_axon=True,
        )
        args = [
            '--radius_multiplier',
            '2',
            '--constant_radius',
            'true',
            '--load_soma',
            'false',
            '--load_dendrites',
            'false',
            '--load_axon',
            'false',
        ]
        test.parse(args)
        self.assertEqual(test.radius_multiplier, 2)
        self.assertEqual(test.constant_radius, True)
        self.assertEqual(test.load_soma, False)
        self.assertEqual(test.load_dendrites, False)
        self.assertEqual(test.load_axon, False)

    def test_create_morphology(self) -> None:
        cli = brayns.MorphologyCli(
            radius_multiplier=1,
            constant_radius=False,
            load_soma=True,
            load_dendrites=True,
            load_axon=True,
        )
        test = cli.create_morphology()
        ref = brayns.Morphology(
            radius_multiplier=1,
            load_soma=True,
            load_dendrites=True,
            load_axon=True,
            geometry_type=brayns.GeometryType.SMOOTH,
        )
        self.assertEqual(test, ref)

    def test_create_morphology_constant_radius(self) -> None:
        cli = brayns.MorphologyCli(
            constant_radius=True,
        )
        test = cli.create_morphology()
        ref = brayns.Morphology(
            radius_multiplier=10,
            geometry_type=brayns.GeometryType.CONSTANT_RADII,
        )
        self.assertEqual(test, ref)


if __name__ == '__main__':
    unittest.main()

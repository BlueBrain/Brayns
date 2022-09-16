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


class TestCapsule(unittest.TestCase):

    def test_method(self) -> None:
        self.assertEqual(brayns.Capsule.method, 'add-capsules')

    def test_get_properties(self) -> None:
        capsule = brayns.Capsule(
            start_point=brayns.Vector3.zero,
            start_radius=0,
            end_point=brayns.Vector3.one,
            end_radius=1,
        )
        test = capsule.get_additonal_properties()
        self.assertEqual(test, {
            'p0': [0, 0, 0],
            'r0': 0,
            'p1': [1, 1, 1],
            'r1': 1,
        })


if __name__ == '__main__':
    unittest.main()

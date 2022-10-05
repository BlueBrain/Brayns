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


class TestOrthographicProjection(unittest.TestCase):

    def test_name(self) -> None:
        test = brayns.OrthographicProjection.name
        ref = 'orthographic'
        self.assertEqual(test, ref)

    def test_look_at(self) -> None:
        aspect_ratio = 2
        target = brayns.Bounds(-brayns.Vector3.one, brayns.Vector3.one)
        projection = brayns.OrthographicProjection()
        test = projection.look_at(target, aspect_ratio)
        self.assertEqual(test.position, 2 * brayns.Vector3.forward)
        self.assertEqual(test.target, brayns.Vector3.zero)
        self.assertEqual(test.up, brayns.Vector3.up)
        self.assertEqual(projection.height, target.height)

    def test_get_properties(self) -> None:
        projection = brayns.OrthographicProjection(3)
        test = projection.get_properties()
        self.assertEqual(test, {'height': 3})

    def test_update_properties(self) -> None:
        test = brayns.OrthographicProjection()
        test.update_properties({'height': 3})
        self.assertEqual(test.height, 3)


if __name__ == '__main__':
    unittest.main()

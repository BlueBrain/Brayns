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
from tests.mock_bounds import MockBounds


class TestLookAt(unittest.TestCase):

    def test_look_at(self) -> None:
        target = brayns.Bounds(
            min=brayns.Vector3.zero,
            max=brayns.Vector3(4, 1, 3),
        )
        projection = brayns.OrthographicProjection()
        test = brayns.look_at(
            target,
            aspect_ratio=2,
            rotation=brayns.CameraRotation.left,
            projection=projection,
        )
        view = test.view
        ref = target.center + 2 * brayns.Axis.left
        self.assertEqual(view.position, ref)
        self.assertEqual(view.target, target.center)
        self.assertEqual(view.up, brayns.Axis.up)
        self.assertAlmostEqual(projection.height, 1.5)

    def test_look_at_default(self) -> None:
        target = brayns.Bounds(
            min=-brayns.Vector3.one,
            max=brayns.Vector3.one,
        )
        test = brayns.look_at(target)
        view = test.view
        self.assertAlmostEqual(view.distance, 3)
        self.assertEqual(view.target, target.center)
        self.assertEqual(view.up, brayns.Axis.up)


if __name__ == '__main__':
    unittest.main()

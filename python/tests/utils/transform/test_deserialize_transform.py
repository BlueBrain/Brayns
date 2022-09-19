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

from brayns.utils import deserialize_transform
from tests.mock_transform import MockTransform


class TestDeserializeTransform(unittest.TestCase):

    def test_deserialize_transform(self) -> None:
        test = deserialize_transform(MockTransform.message)
        ref = MockTransform.transform
        self.assertEqual(test.translation, ref.translation)
        self.assertEqual(test.scale, ref.scale)
        q1 = test.rotation.quaternion
        q2 = ref.rotation.quaternion
        self.assertAlmostEqual(q1.x, q2.x)
        self.assertAlmostEqual(q1.y, q2.y)
        self.assertAlmostEqual(q1.z, q2.z)
        self.assertAlmostEqual(q1.w, q2.w)


if __name__ == '__main__':
    unittest.main()

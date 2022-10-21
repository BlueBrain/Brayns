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
from tests.mock_instance import MockInstance


class TestRenderImage(unittest.TestCase):

    def test_render_image(self) -> None:
        result = {
            'accumulation': 1,
            'max_accumulation': 2,
        }
        data = b'123'
        instance = MockInstance(result, data)
        test = brayns.render_image(instance)
        self.assertEqual(instance.params, {
            'send': True,
            'force': False,
            'format': 'jpg',
            'jpeg_quality': 100,
        })
        self.assertEqual(test.accumulation, 1)
        self.assertEqual(test.max_accumulation, 2)
        self.assertEqual(test.data, data)
        self.assertTrue(test.received)

    def test_render_image_params(self) -> None:
        result = {
            'accumulation': 1,
            'max_accumulation': 2,
        }
        instance = MockInstance(result)
        test = brayns.render_image(
            instance,
            send=False,
            force=False,
            format=brayns.ImageFormat.PNG,
            jpeg_quality=60,
        )
        self.assertEqual(instance.params, {
            'send': False,
            'force': False,
            'format': 'png',
        })
        self.assertEqual(test.accumulation, 1)
        self.assertEqual(test.max_accumulation, 2)
        self.assertFalse(test.data)
        self.assertFalse(test.received)


if __name__ == '__main__':
    unittest.main()

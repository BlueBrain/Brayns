# Copyright (c) 2015-2024 EPFL/Blue Brain Project
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


class TestImage(unittest.TestCase):
    def test_download(self) -> None:
        result = {
            "accumulation": 1,
            "max_accumulation": 2,
        }
        data = b"123"
        instance = MockInstance(result, data)
        image = brayns.Image(
            accumulate=False,
            force_download=False,
            jpeg_quality=60,
        )
        test = image.download(instance, brayns.ImageFormat.JPEG, render=False)
        self.assertEqual(
            instance.params,
            {
                "send": True,
                "force": False,
                "accumulate": False,
                "format": "jpg",
                "jpeg_quality": 60,
                "render": False,
            },
        )
        self.assertEqual(test.accumulation, 1)
        self.assertEqual(test.max_accumulation, 2)
        self.assertEqual(test.data, data)
        self.assertFalse(test.full_quality)

    def test_render(self) -> None:
        result = {
            "accumulation": 2,
            "max_accumulation": 2,
        }
        instance = MockInstance(result)
        image = brayns.Image()
        test = image.render(instance)
        self.assertEqual(
            instance.params,
            {
                "send": False,
                "force": False,
                "accumulate": True,
                "render": True,
            },
        )
        self.assertEqual(test.accumulation, 2)
        self.assertEqual(test.max_accumulation, 2)
        self.assertFalse(test.data)
        self.assertTrue(test.full_quality)

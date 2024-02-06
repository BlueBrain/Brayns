# Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

import brayns
from tests.mock_instance import MockInstance


class TestFramebuffer(unittest.TestCase):
    def test_set_framebuffer(self) -> None:
        instance = MockInstance()
        framebuffer = brayns.ProgressiveFramebuffer(10)
        brayns.set_framebuffer(instance, framebuffer)
        self.assertEqual(instance.method, "set-framebuffer-progressive")
        self.assertEqual(
            instance.params,
            {
                "scale": 10,
            },
        )

    def test_progressive_framebuffer(self) -> None:
        self.assertEqual(brayns.ProgressiveFramebuffer.name, "progressive")
        test = brayns.ProgressiveFramebuffer(10)
        self.assertEqual(test.get_properties(), {"scale": 10})

    def test_static_framebuffer(self) -> None:
        self.assertEqual(brayns.StaticFramebuffer.name, "static")

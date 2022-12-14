# Copyright (c) 2015-2022 EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# Responsible Author: nadir.romanguerrero@epfl.ch
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


class TestBoundedPlane(unittest.TestCase):
    def test_method(self) -> None:
        self.assertEqual(brayns.BoundedPlane.method, "add-bounded-planes")

    def test_get_properties(self) -> None:
        bounded_plane = brayns.BoundedPlane(
            brayns.PlaneEquation(0, 0, 1, 0),
            bounds=brayns.Bounds(
                min=brayns.Vector3.zero,
                max=brayns.Vector3.one,
            ),
        )
        test = bounded_plane.get_additional_properties()
        self.assertEqual(
            test,
            {
                "coefficients": [0, 0, 1, 0],
                "bounds": {
                    "min": [0, 0, 0],
                    "max": [1, 1, 1],
                },
            },
        )

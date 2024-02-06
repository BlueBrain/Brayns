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


class TestOpacityCurve(unittest.TestCase):
    def test_apply(self) -> None:
        curve = brayns.OpacityCurve(
            [
                brayns.ControlPoint(0, 0.1),
                brayns.ControlPoint(0.5, 0.2),
                brayns.ControlPoint(1, 0.3),
            ]
        )
        colors = [
            brayns.Color3(1, 0, 0),
            brayns.Color3(0, 1, 0),
            brayns.Color3(0, 0, 1),
        ]
        ref = [
            brayns.Color4(1, 0, 0, 0.1),
            brayns.Color4(0, 1, 0, 0.2),
            brayns.Color4(0, 0, 1, 0.3),
        ]
        test = curve.apply(colors)
        self.assertEqual(test, ref)

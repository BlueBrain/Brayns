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

from brayns.core.color.color3 import Color3
from brayns.core.color.color4 import Color4
from brayns.core.transfer_function.control_point import ControlPoint
from brayns.core.transfer_function.opacity_curve import OpacityCurve


class TestOpacityCurve(unittest.TestCase):

    def test_apply(self) -> None:
        curve = OpacityCurve([
            ControlPoint(0, 0.1),
            ControlPoint(0.5, 0.2),
            ControlPoint(1, 0.3),
        ])
        colors = [
            Color3(1, 0, 0),
            Color3(0, 1, 0),
            Color3(0, 0, 1),
        ]
        ref = [
            Color4(1, 0, 0, 0.1),
            Color4(0, 1, 0, 0.2),
            Color4(0, 0, 1, 0.3),
        ]
        test = curve.apply(colors)
        self.assertEqual(test, ref)


if __name__ == '__main__':
    unittest.main()

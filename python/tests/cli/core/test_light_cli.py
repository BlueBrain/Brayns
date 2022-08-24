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


class TestLightCli(unittest.TestCase):

    def test_parse(self) -> None:
        test = brayns.LightCli(
            color=brayns.Color3.red,
            intensity=2,
            rotation=brayns.Rotation.identity,
        )
        args = [
            '--light_color',
            '0',
            '1',
            '0',
            '--light_intensity',
            '3',
            '--light_rotation',
            '25',
            '25',
            '25',
        ]
        euler = brayns.Vector3(25, 25, 25)
        rotation = brayns.Rotation.from_euler(euler, degrees=True)
        test.parse(args)
        self.assertEqual(test.color, brayns.Color3.green)
        self.assertEqual(test.intensity, 3)
        self.assertEqual(test.rotation, rotation)

    def test_create_light(self) -> None:
        cli = brayns.LightCli()
        direction = brayns.Vector3.one
        test = cli.create_light(direction)
        ref = brayns.DirectionalLight(
            color=cli.color,
            intensity=cli.intensity,
            direction=direction,
        )
        self.assertEqual(test, ref)


if __name__ == '__main__':
    unittest.main()

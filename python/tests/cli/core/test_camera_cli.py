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


class TestCameraCli(unittest.TestCase):

    def test_parse(self) -> None:
        test = brayns.CameraCli(
            name=brayns.OrthographicCamera.name,
            available=[brayns.OrthographicCamera],
            translation=brayns.Vector3.one,
            rotation=brayns.Rotation.identity,
        )
        args = [
            '--camera_type',
            'orthographic',
            '--camera_translation',
            '0',
            '1',
            '0',
            '--camera_rotation',
            '25',
            '25',
            '25',
        ]
        euler = brayns.Vector3(25, 25, 25)
        rotation = brayns.Rotation.from_euler(euler, degrees=True)
        test.parse(args)
        self.assertEqual(test.name, 'orthographic')
        self.assertEqual(test.translation, brayns.Vector3.up)
        self.assertEqual(test.rotation, rotation)

    def test_create_camera(self) -> None:
        cli = brayns.CameraCli(
            name=brayns.OrthographicCamera.name
        )
        bounds = brayns.Bounds(-brayns.Vector3.one, brayns.Vector3.one)
        test = cli.create_camera(bounds)
        ref = brayns.OrthographicCamera(bounds.height)
        self.assertEqual(test, ref)

    def test_create_view(self) -> None:
        euler = brayns.Vector3(0, 90, 0)
        cli = brayns.CameraCli(
            translation=brayns.Vector3.one,
            rotation=brayns.Rotation.from_euler(euler, degrees=True),
        )
        bounds = brayns.Bounds(-brayns.Vector3.one, brayns.Vector3.one)
        camera = brayns.PerspectiveCamera()
        test = cli.create_view(camera, bounds)
        ref = camera.get_front_view(bounds)
        ref.position += cli.translation
        ref.position = cli.rotation.apply(ref.position, center=ref.target)
        self.assertEqual(test, ref)


if __name__ == '__main__':
    unittest.main()

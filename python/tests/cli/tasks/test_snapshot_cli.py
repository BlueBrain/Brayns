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


class TestSnapshotCli(unittest.TestCase):

    def test_parse(self) -> None:
        test = brayns.SnapshotCli(
            name='Test',
            description='This is a test',
            path='test',
            resolution=brayns.Resolution.ultra_hd,
            service=brayns.ServiceCli(),
            loader=brayns.MeshLoaderCli(),
            light=brayns.LightCli(),
            camera=brayns.CameraCli(),
            renderer=brayns.RendererCli(),
            save_as='test2',
            frame=0,
        )
        args = [
            '--path',
            'path',
            '--save_as',
            'save',
            '--resolution',
            '1920',
            '1080',
            '--frame',
            '25',
        ]
        test.parse(args)
        self.assertEqual(test.path, 'path')
        self.assertEqual(test.save_as, 'save')
        self.assertEqual(test.resolution, brayns.Resolution.full_hd)
        self.assertEqual(test.frame, 25)


if __name__ == '__main__':
    unittest.main()

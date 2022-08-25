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


class TestFrameExporterCli(unittest.TestCase):

    def test_parse(self) -> None:
        test = brayns.FrameExporterCli(
            save_as='test',
            frames_folder='test2',
            frames_format=brayns.ImageFormat.PNG,
            ffmpeg_executable='test3',
            frames=brayns.MovieFramesCli(),
        )
        args = [
            '--path',
            'path',
            '--save_as',
            'save',
            '--frames_folder',
            'folder',
            '--frames_format',
            'jpg',
            '--ffmpeg_executable',
            'exe',
            '--fps',
            '10',
        ]
        test.parse(args)
        self.assertEqual(test.path, 'path')
        self.assertEqual(test.save_as, 'save')
        self.assertEqual(test.frames_folder, 'folder')
        self.assertEqual(test.frames_format, brayns.ImageFormat.JPEG)
        self.assertEqual(test.ffmpeg_executable, 'exe')
        self.assertEqual(test.frames.fps, 10)


if __name__ == '__main__':
    unittest.main()

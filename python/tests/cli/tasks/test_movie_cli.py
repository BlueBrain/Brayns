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


class TestMovieCli(unittest.TestCase):

    def test_parse(self) -> None:
        test = brayns.MovieCli(
            save_as='test',
            frames_folder='test1',
            frames_format=brayns.ImageFormat.PNG,
            fps=10,
            ffmpeg_executable='test2',
        )
        args = [
            '--save_as',
            'save',
            '--frames_folder',
            'folder',
            '--frames_format',
            'jpg',
            '--fps',
            '5',
            '--ffmpeg_executable',
            'exe',
        ]
        test.parse(args)
        self.assertEqual(test.save_as, 'save')
        self.assertEqual(test.frames_folder, 'folder')
        self.assertEqual(test.frames_format, brayns.ImageFormat.JPEG)
        self.assertEqual(test.fps, 5)
        self.assertEqual(test.ffmpeg_executable, 'exe')

    def test_create_movie(self) -> None:
        cli = brayns.MovieCli(
            frames_folder='test',
        )
        test = cli.create_movie()
        ref = brayns.Movie(
            frames_folder=cli.frames_folder,
            frames_format=cli.frames_format,
            fps=cli.fps,
            ffmpeg_executable=cli.ffmpeg_executable,
        )
        self.assertEqual(test, ref)


if __name__ == '__main__':
    unittest.main()

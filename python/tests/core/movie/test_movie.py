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

from brayns.core.image.resolution import Resolution
from brayns.core.movie.movie import Movie
from brayns.core.image.image_format import ImageFormat


class TestMovie(unittest.TestCase):

    def test_get_command_line(self) -> None:
        movie = Movie(
            frames_folder='folder',
            frames_format=ImageFormat.PNG,
            fps=30,
            resolution=Resolution.full_hd,
            bitrate=64000,
            encoder='encoder',
            pixel_format='pixel',
            ffmpeg_executable='test',
        )
        args = movie.get_command_line('path')
        ref = [
            'test',
            '-y',
            '-framerate',
            '30',
            '-i',
            'folder/%05d.png',
            '-vf',
            'fps=30,format=pixel',
            '-s',
            '1920x1080',
            '-b:v',
            '64000',
            '-c',
            'encoder',
            'path',
        ]
        self.assertEqual(args, ref)


if __name__ == '__main__':
    unittest.main()

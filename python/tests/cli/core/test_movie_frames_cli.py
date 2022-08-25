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
        test = brayns.MovieFramesCli(
            fps=20,
            slowing_factor=2,
            start_frame=1,
            end_frame=2,
        )
        args = [
            '--fps',
            '10',
            '--slowing_factor',
            '3',
            '--start_frame',
            '4',
            '--end_frame',
            '-4',
        ]
        test.parse(args)
        self.assertEqual(test.fps, 10)
        self.assertEqual(test.slowing_factor, 3)
        self.assertEqual(test.start_frame, 4)
        self.assertEqual(test.end_frame, -4)

    def test_create_frames(self) -> None:
        cli = brayns.MovieFramesCli()
        test = cli.create_frames()
        ref = brayns.MovieFrames(
            fps=cli.fps,
            slowing_factor=cli.slowing_factor,
            start_frame=cli.start_frame,
            end_frame=cli.end_frame,
        )
        self.assertEqual(test, ref)


if __name__ == '__main__':
    unittest.main()

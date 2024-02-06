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

import tempfile
from pathlib import Path

import brayns
from testapi.render import validate_file
from testapi.simple_test_case import SimpleTestCase


class TestMovie(SimpleTestCase):
    def test_save(self) -> None:
        frames = self.asset_folder / "frames" / "%05d.png"
        movie = brayns.Movie(
            frames_pattern=str(frames),
            fps=1,
            ffmpeg_executable=self.ffmpeg,
        )
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "movie.mp4"
            movie.save(str(path))
            ref = self.asset_folder / "movie.mp4"
            validate_file(path, ref)

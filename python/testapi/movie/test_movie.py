# Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

import pathlib
import tempfile

import brayns
from testapi.image_validator import ImageValidator
from testapi.simple_test_case import SimpleTestCase


class TestMovie(SimpleTestCase):
    @property
    def input(self) -> pathlib.Path:
        return self.folder / "frames" / "%05d.png"

    @property
    def ref(self) -> pathlib.Path:
        return self.folder / "movie.mp4"

    def test_save(self) -> None:
        movie = brayns.Movie(
            frames_pattern=str(self.input),
            fps=1,
            ffmpeg_executable=self.ffmpeg,
        )
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / "movie.mp4"
            movie.save(str(path))
            validator = ImageValidator()
            validator.validate_file(path, self.ref)

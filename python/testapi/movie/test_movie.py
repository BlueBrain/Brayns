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

import pathlib

import brayns

from ..image_validator import ImageValidator
from ..simple_test_case import SimpleTestCase


class TestMovie(SimpleTestCase):

    @property
    def input(self) -> pathlib.Path:
        return self.asset_folder / 'frames'

    @property
    def output(self) -> pathlib.Path:
        folder = pathlib.Path(__file__).parent
        return folder / 'movie.mp4'

    @property
    def ref(self) -> pathlib.Path:
        return self.asset_folder / 'movie.mp4'

    def test_save(self) -> None:
        movie = brayns.Movie(
            frames_folder=str(self.input),
            fps=1,
            ffmpeg_executable=self.ffmpeg
        )
        movie.save(str(self.output))
        validator = ImageValidator()
        validator.validate_file(self.output, self.ref)

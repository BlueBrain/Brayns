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

from __future__ import annotations

import argparse
from dataclasses import dataclass

from brayns.movie import Movie
from brayns.utils import ImageFormat

from ..cli import Cli


@dataclass
class MovieCli(Cli):

    save_as: str = ''
    frames_folder: str = ''
    frames_format: ImageFormat = ImageFormat.PNG
    fps: float = 25.0
    ffmpeg_executable: str = 'ffmpeg'

    def register(self, parser: argparse.ArgumentParser) -> None:
        parser.add_argument(
            '--save_as',
            required=True,
            metavar='PATH',
            help='Path to save movie',
        )
        parser.add_argument(
            '--frames_folder',
            required=True,
            metavar='PATH',
            help='Folder with movie frames already rendered',
        )
        parser.add_argument(
            '--frames_format',
            choices=[format.value for format in ImageFormat],
            required=True,
            help='Encoding of the movie frames',
        )
        parser.add_argument(
            '--fps',
            type=float,
            required=True,
            metavar='RATE',
            help='Movie FPS, should be the same as for export',
        )
        parser.add_argument(
            '--ffmpeg_executable',
            default=self.ffmpeg_executable,
            metavar='PATH',
            help='FFMPEG binary executable to use to make the movie',
        )

    def load(self, args: argparse.Namespace) -> None:
        self.save_as = args.save_as
        self.frames_folder = args.frames_folder
        self.frames_format = ImageFormat(args.frames_format)
        self.fps = args.fps
        self.ffmpeg_executable = args.ffmpeg_executable

    def run(self) -> None:
        movie = self.create_movie()
        movie.save(self.save_as)

    def create_movie(self) -> Movie:
        return Movie(
            frames_folder=self.frames_folder,
            frames_format=self.frames_format,
            fps=self.fps,
            ffmpeg_executable=self.ffmpeg_executable,
        )

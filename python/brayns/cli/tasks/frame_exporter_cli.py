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
from dataclasses import dataclass, field

from brayns.core import FrameExporter, KeyFrame, get_simulation
from brayns.movie import Movie
from brayns.utils import ImageFormat

from ..core import MovieFramesCli
from .render_cli import RenderCli
from .render_command import RenderCommand


@dataclass
class FrameExporterCli(RenderCli):

    save_as: str = ''
    frames_folder: str = ''
    frames_format: ImageFormat = ImageFormat.PNG
    ffmpeg_executable: str = 'ffmpeg'
    frames: MovieFramesCli = field(default_factory=MovieFramesCli)

    def register_additional_args(self, parser: argparse.ArgumentParser) -> None:
        parser.add_argument(
            '--save_as',
            type=str,
            default=self.save_as,
            metavar='PATH',
            help='Path to save the movie, no movies generated if empty',
        )
        parser.add_argument(
            '--frames_folder',
            type=str,
            default=self.frames_folder,
            metavar='PATH',
            help='Folder to save exported frames, no export if empty',
        )
        parser.add_argument(
            '--frames_format',
            type=str,
            choices=[format.value for format in ImageFormat],
            default=self.frames_format.value,
            metavar='NAME',
            help='Encoding of the exported frames',
        )
        parser.add_argument(
            '--ffmpeg_executable',
            type=str,
            default=self.ffmpeg_executable,
            metavar='PATH',
            help='Path to FFMPEG executable',
        )
        self.frames.register(parser)

    def load_additional_args(self, args: argparse.Namespace) -> None:
        self.save_as = args.save_as
        self.frames_folder = args.frames_folder
        self.frames_format = ImageFormat(args.frames_format)
        self.ffmpeg_executable = args.ffmpeg_executable
        self.frames.load(args)

    def render(self, command: RenderCommand) -> None:
        if not self.frames_folder and not self.save_as:
            raise ValueError('Both export and movie are disabled')
        if self.frames_folder:
            self._export_frames(command)
        if self.save_as:
            self._make_movie()

    def _export_frames(self, command: RenderCommand) -> None:
        exporter = FrameExporter(
            frames=self._create_frames(command),
            format=self.frames_format,
            resolution=self.resolution,
            camera=command.camera,
            renderer=command.renderer,
        )
        exporter.export_frames(command.instance, self.frames_folder)

    def _create_frames(self, command: RenderCommand) -> list[KeyFrame]:
        frames = self.frames.create_frames()
        simulation = get_simulation(command.instance)
        indices = frames.get_indices(simulation)
        return KeyFrame.from_indices(indices, command.view)

    def _make_movie(self) -> None:
        movie = Movie(
            frames_folder=self.frames_folder,
            frames_format=self.frames_format,
            fps=self.frames.fps,
            ffmpeg_executable=self.ffmpeg_executable,
        )
        movie.save(self.save_as)

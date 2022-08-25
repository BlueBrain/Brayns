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

from brayns.movie import MovieFrames

from ..cli import Cli


@dataclass
class MovieFramesCli(Cli):

    fps: float = 25.0
    slowing_factor: float = 1.0
    start_frame: int = 0
    end_frame: int = -1

    def register(self, parser: argparse.ArgumentParser) -> None:
        parser.add_argument(
            '--fps',
            type=float,
            default=self.fps,
            metavar='RATE',
            help='Movie FPS',
        )
        parser.add_argument(
            '--slowing_factor',
            type=float,
            default=self.slowing_factor,
            metavar='FACTOR',
            help='Slow motion factor (2 = twice slower)',
        )
        parser.add_argument(
            '--start_frame',
            type=int,
            default=self.start_frame,
            metavar='INDEX',
            help='Index of the first frame of the movie',
        )
        parser.add_argument(
            '--end_frame',
            type=int,
            default=self.end_frame,
            metavar='INDEX',
            help='Index of the last frame of the movie',
        )

    def load(self, args: argparse.Namespace) -> None:
        self.fps = args.fps
        self.slowing_factor = args.slowing_factor
        self.start_frame = args.start_frame
        self.end_frame = args.end_frame

    def create_frames(self) -> MovieFrames:
        return MovieFrames(
            fps=self.fps,
            slowing_factor=self.slowing_factor,
            start_frame=self.start_frame,
            end_frame=self.end_frame,
        )

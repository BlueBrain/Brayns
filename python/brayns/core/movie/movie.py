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
import subprocess
from collections import deque
from dataclasses import dataclass
from typing import Optional

from brayns.core.common.resolution import Resolution
from brayns.core.movie.movie_error import MovieError
from brayns.core.snapshot.image_format import ImageFormat


@dataclass
class Movie:

    frames_folder: str
    frames_format: ImageFormat = ImageFormat.PNG
    fps: float = 25.0
    resolution: Optional[Resolution] = None
    bitrate: Optional[int] = None
    encoder: Optional[str] = None
    pixel_format: Optional[str] = 'yuv420p'
    ffmpeg_executable: str = 'ffmpeg'

    def save(self, path: str) -> str:
        args = self.get_command_line(path)
        return _run_process(args)

    def get_command_line(self, path: str) -> list[str]:
        return [
            self.ffmpeg_executable,
            *_get_global_options(),
            *_get_input_options(self),
            *_get_input(self.frames_folder, self.frames_format),
            *_get_output_options(self),
            path
        ]


def _run_process(args: list[str]) -> str:
    try:
        process = _create_process(args)
    except OSError as e:
        raise MovieError(str(e))
    lines = deque[str](process.stdout, maxlen=1000)
    code = process.wait()
    logs = ''.join(lines)
    if code != 0:
        raise MovieError(f'ffmpeg call failed (see logs)', code, logs)
    return logs


def _create_process(args: list[str]) -> subprocess.Popen:
    return subprocess.Popen(
        args=args,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        env={'AV_LOG_FORCE_NOCOLOR': '1'}
    )


def _get_global_options() -> list[str]:
    return [
        '-y'
    ]


def _get_input_options(movie: Movie) -> list[str]:
    return [
        '-framerate',
        str(movie.fps)
    ]


def _get_input(folder: str, format: ImageFormat) -> list[str]:
    return [
        '-i',
        _get_pattern(folder, format)
    ]


def _get_pattern(folder: str, format: ImageFormat) -> str:
    folder = pathlib.Path(folder)
    pattern = f'%05d.{format.value}'
    return str(folder / pattern)


def _get_output_options(movie: Movie) -> list[str]:
    args = [
        '-vf',
        _get_video_filters(movie)
    ]
    if movie.resolution is not None:
        width, height = movie.resolution
        args.append('-s')
        args.append(f'{width:d}x{height:d}')
    if movie.bitrate is not None:
        args.append('-b')
        args.append(str(movie.bitrate))
    if movie.encoder is not None:
        args.append('-c')
        args.append(movie.encoder)
    return args


def _get_video_filters(movie: Movie) -> str:
    filters = [
        f'fps={movie.fps}'
    ]
    if movie.pixel_format is not None:
        filters.append(f'format={movie.pixel_format}')
    return ','.join(filters)

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

from __future__ import annotations

import os
import subprocess
from collections import deque
from dataclasses import dataclass
from typing import IO, cast

from brayns.utils import Error, Resolution


@dataclass
class MovieError(Error):
    """Exception raised if an error occurs when making a movie.

    :param reason: Description of what happened.
    :type reason: str
    :param code: Error code (FFMPEG output code).
    :type code: int
    :param logs: FFMPEG logs if any.
    :type logs: str
    """

    reason: str
    code: int = 0
    logs: str = ""

    def __str__(self) -> str:
        return self.reason


@dataclass
class Movie:
    """Holds all the necessary information to generate a movie.

    Movies are generated using FFMPEG executable, this class only generates the
    command line and runs it. Customm FFMPEG executable can be specified if it
    cannot be found in the system PATH.

    Frames are selected using a filename pattern. This one depends on the naming
    used for the export. The frame index is specified in the pattern using the C
    printf format (~%d). See FFMPEG input command line (-i) for more details.

    Encoder settings are chosen by FFMPEG if not specified except for pixel
    format which is chosen for maximum compatibility with players.

    All the frames selected are used to generate the movie so its duration will
    be frame_count / FPS.

    :param frames_pattern: Frames filename pattern.
    :type frames_pattern: str
    :param fps: Movie FPS, should be the same as for export.
    :type fps: float
    :param resolution: Movie resolution, defaults to frames resolution.
    :type resolution: Resolution | None, optional
    :param bitrate: Encoding bitrate, defaults to FFMPEG choice.
    :type bitrate: int | None, optional
    :param encoder: Encoder name, defaults to deduced from output file.
    :type encoder: str | None, optional
    :param pixel_format: Pixel format name, defaults to 'yuv420p'.
    :type pixel_format: str | None, optional
    :param ffmpeg_executable: FFMPEG executable path, defaults to 'ffmpeg'.
    :type ffmpeg_executable: str, optional
    """

    frames_pattern: str
    fps: float
    resolution: Resolution | None = None
    bitrate: int | None = None
    encoder: str | None = None
    pixel_format: str | None = "yuv420p"
    ffmpeg_executable: str = "ffmpeg"

    def save(self, path: str) -> str:
        """Save the movie under the given path.

        Simply run get_command_line(path).

        :param path: Movie output path.
        :type path: str
        :return: FFMPEG logs for debugging.
        :rtype: str
        """
        args = self.get_command_line(path)
        return _run_process(args)

    def get_command_line(self, path: str) -> list[str]:
        """Generate FFMPEG command line from members and given path.

        :param path: Path to save the movie.
        :type path: str
        :return: FFMPEG command line.
        :rtype: list[str]
        """
        return [
            self.ffmpeg_executable,
            *_get_global_options(),
            *_get_input_options(self),
            *_get_input(self),
            *_get_output_options(self),
            path,
        ]


def _run_process(args: list[str]) -> str:
    with _create_process(args) as process:
        stream = cast(IO[str], process.stdout)
        lines = deque[str](stream, maxlen=1000)
        code = process.wait()
    logs = "".join(lines)
    if code != 0:
        raise MovieError("ffmpeg call failed (see logs)", code, logs)
    return logs


def _create_process(args: list[str]) -> subprocess.Popen[str]:
    try:
        return subprocess.Popen(
            args=args,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            env=os.environ | {"AV_LOG_FORCE_NOCOLOR": "1"},
        )
    except OSError as e:
        raise MovieError(f"Failed to start ffmpeg process {str(e)}")


def _get_global_options() -> list[str]:
    return ["-y"]


def _get_input_options(movie: Movie) -> list[str]:
    return ["-framerate", str(movie.fps)]


def _get_input(movie: Movie) -> list[str]:
    return [
        "-i",
        movie.frames_pattern,
    ]


def _get_output_options(movie: Movie) -> list[str]:
    args = ["-vf", _get_video_filters(movie)]
    if movie.resolution is not None:
        width, height = movie.resolution
        args.append("-s")
        args.append(f"{width:d}x{height:d}")
    if movie.bitrate is not None:
        args.append("-b:v")
        args.append(str(movie.bitrate))
    if movie.encoder is not None:
        args.append("-c")
        args.append(movie.encoder)
    return args


def _get_video_filters(movie: Movie) -> str:
    filters = [f"fps={movie.fps}"]
    if movie.pixel_format is not None:
        filters.append(f"format={movie.pixel_format}")
    return ",".join(filters)

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

from dataclasses import dataclass, field

from brayns.utils import Resolution

from .log_level import LogLevel
from .plugin import Plugin
from .process import Process
from .ssl_server_context import SslServerContext


@dataclass
class Launcher:
    """Used to start a braynsService instance from Python.

    Use a braynsService executable to start a subprocess. By default it looks
    for a 'braynsService' binary in the PATH but it can be overriden.

    URI is the websocket server URI (ip:port) use 0.0.0.0 as wildcard to allow
    connections from any machine.

    SSL server settings can be specified using optional certificate, key, CA and
    password.

    Log level and default streaming parameters can also be specified.

    Custom environment variables can also be set for the subprocess, for example
    to override the PATH and load specific libraries.

    :param uri: Service URI with format 'host:port'.
    :type uri: str
    :param ssl_context: SSL context if secure, defaults to None.
    :type ssl_context: SslServerContext | None, optional
    :param log_level: Process log level, defaults to LogLevel.WARN.
    :type log_level: LogLevel, optional
    :param resolution: Streaming resolution, defaults to full HD.
    :type resolution: Resolution, optional
    :param jpeg_quality: Streaming JPEG quality, defaults to 100.
    :type jpeg_quality: int, optional
    :param plugins: Plugins to load, defaults to all built-in plugins.
    :type plugins: list[str], optional
    :param executable: Path of the braynsService executable.
    :type executable: str
    :param env: Subprocess environment variables, default to empty.
    :type env: dict[str, str], optional
    """

    uri: str
    ssl_context: SslServerContext | None = None
    log_level: LogLevel = LogLevel.WARN
    resolution: Resolution = Resolution.full_hd
    jpeg_quality: int = 100
    plugins: list[str] = field(default_factory=Plugin.all_values)
    executable: str = 'braynsService'
    env: dict[str, str] = field(default_factory=dict)

    def get_command_line(self) -> list[str]:
        """Build the command line to start braynsService.

        :return: Command line arguments.
        :rtype: list[str]
        """
        args = [
            self.executable,
            '--uri',
            self.uri,
            '--log-level',
            self.log_level.value,
            '--window-size',
            f'{self.resolution.width} {self.resolution.height}',
            '--jpeg-quality',
            str(self.jpeg_quality)
        ]
        args.extend(
            item
            for plugin in self.plugins
            for item in ('--plugin', plugin)
        )
        if self.ssl_context is not None:
            args.append('--secure')
            args.append('true')
            ssl_args = self.ssl_context.get_command_line()
            args.extend(ssl_args)
        return args

    def start(self) -> Process:
        """Start a new process for a braynsService instance.

        Return the process which runs the instance.

        :return: Instance process.
        :rtype: Process
        """
        args = self.get_command_line()
        return Process(args, self.env)

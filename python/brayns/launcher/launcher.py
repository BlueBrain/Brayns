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

from dataclasses import dataclass, field
from typing import Optional

from brayns.launcher.log_level import LogLevel
from brayns.launcher.plugin import Plugin
from brayns.launcher.process import Process
from brayns.launcher.ssl_context import SslContext


@dataclass
class Launcher:

    executable: str
    uri: str = 'localhost:5000'
    loglevel: LogLevel = LogLevel.WARN
    plugins: list[Plugin] = field(default_factory=lambda: list(Plugin))
    ssl: Optional[SslContext] = None
    env: dict[str, str] = field(default_factory=dict)

    def get_command_line(self) -> list[str]:
        args = [
            self.executable,
            '--uri',
            self.uri,
            '--log-level',
            self.loglevel.value
        ]
        args.extend(
            item
            for plugin in self.plugins
            for item in ('--plugin', plugin.value)
        )
        if self.ssl is not None:
            args.append('--secure')
            args.append('true')
            ssl_args = self.ssl.get_command_line()
            args.extend(ssl_args)
        return args

    def start(self) -> Process:
        args = self.get_command_line()
        return Process(args, self.env)

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

import subprocess
from typing import Optional


class Process:

    @staticmethod
    def from_command_line(args: list[str], cwd: Optional[str] = None, env: Optional[dict] = None) -> 'Process':
        process = subprocess.Popen(
            args=args,
            cwd=cwd,
            env=env,
            text=True,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )
        return Process(process)

    def __init__(self, process: subprocess.Popen) -> None:
        self._process = process
        self._logs = ''

    def __enter__(self) -> 'Process':
        return self

    def __exit__(self, *_) -> None:
        self.terminate()

    @property
    def alive(self) -> bool:
        return self._process.poll() is None

    @property
    def logs(self) -> str:
        return self._logs

    def terminate(self) -> None:
        self._process.terminate()
        stdout, _ = self._process.communicate()
        self.logs = stdout

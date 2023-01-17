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
import threading
from collections import deque
from typing import IO, cast


class Process:
    """Process running a braynsService backend.

    The process must be stopped using terminate() once done with it, otherwise
    it will run forever. Use context manager (ie with process: ...) to avoid
    mistakes.

    The last lines of the process logs can be retreived using ``logs``.
    """

    def __init__(self, args: list[str], env: dict[str, str]) -> None:
        self._process = subprocess.Popen(
            args=args,
            env=os.environ | env,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
        )
        self._thread = threading.Thread(target=self._poll)
        self._logs = deque[str](maxlen=1000)
        self._lock = threading.RLock()
        self._thread.start()

    def __enter__(self) -> Process:
        """Context manager, just return self."""
        return self

    def __exit__(self, *_) -> None:
        """Context manager, call ``stop()``."""
        self.stop()

    @property
    def alive(self) -> bool:
        """Check if the process is still running."""
        return self._process.poll() is None

    @property
    def logs(self) -> str:
        """Get the last logs (stdout + stderr) from the process."""
        with self._lock:
            return "".join(self._logs)

    def stop(self) -> None:
        """Stop the backend service by terminating the process.

        Must be called through the context manager (i.e. 'with') or manually to
        avoid the process to run forever.
        """
        self._process.terminate()
        self._process.wait()
        stdin = cast(IO[str], self._process.stdin)
        stdin.close()
        stdout = cast(IO[str], self._process.stdout)
        stdout.close()
        self._thread.join()

    def _poll(self) -> None:
        stdout = cast(IO[str], self._process.stdout)
        for line in stdout:
            with self._lock:
                self._logs.append(line)

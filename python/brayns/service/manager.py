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

from ..network import Instance
from .process import Process


class Manager:
    """Wrapper to manage a ``Process`` connected to an ``Instance``.

    Contains a process running braynService and an instance connected to it.

    Once done with this object, ``stop()`` must be called to disconnect the
    instance and kill the process, using the context manager or by hand.
    """

    def __init__(self, process: Process, instance: Instance) -> None:
        self._process = process
        self._instance = instance

    def __enter__(self) -> Manager:
        """Context manager enter.

        :return: Self.
        :rtype: Manager
        """
        return self

    def __exit__(self, *_) -> None:
        """Call stop when exiting the context manager."""
        self.stop()

    @property
    def process(self) -> Process:
        """Process running braynsService.

        :return: braynsService Process.
        :rtype: Process
        """
        return self._process

    @property
    def instance(self) -> Instance:
        """Client instance connected to ``process``.

        :return: Connected instance.
        :rtype: Instance
        """
        return self._instance

    def stop(self) -> None:
        """Disconnect the instance and kill the process."""
        self._instance.disconnect()
        self._process.stop()

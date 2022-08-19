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

from dataclasses import dataclass

from brayns.network import Instance

from .process import Process


@dataclass
class Service:
    """Wrapper around a ``Process`` connected to an ``Instance``.

    Contains a running process and an instance connected to it.

    Once done with this object, ``stop()`` must be called to disconnect the
    instance and kill the process using the context manager (with service) or
    manually (service.stop()).
    """

    process: Process
    instance: Instance

    def __enter__(self) -> Service:
        """Context manager enter.

        :return: Self.
        :rtype: Service
        """
        return self

    def __exit__(self, *_) -> None:
        """Call stop when exiting the context manager."""
        self.stop()

    def stop(self) -> None:
        """Disconnect the instance and kill the process."""
        self.instance.disconnect()
        self.process.stop()

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

from typing import NamedTuple

from brayns.network import Connector, Instance

from .process import Process
from .service import Service


class Manager(NamedTuple):
    """Wrapper to manage a ``Process`` connected to an ``Instance``.

    Contains a process running braynService and an instance connected to it.

    Once done with this object, ``stop()`` must be called to disconnect the
    instance and kill the process, using the context manager or by hand.

    :param process: Process running braynsService.
    :type process: Process
    :param instance: Instance connected to ``process``.
    :type instance: Instance
    """

    process: Process
    instance: Instance

    def __enter__(self) -> Manager:
        """Context manager enter, just return self."""
        return self

    def __exit__(self, *_) -> None:
        """Call stop when exiting the context manager."""
        self.stop()

    def stop(self) -> None:
        """Disconnect the instance and kill the process."""
        self.instance.disconnect()
        self.process.stop()


def start(service: Service, connector: Connector) -> Manager:
    """Start a braynsService instance and connect to it.

    Return the service process and the connected instance (see ``Manager``).

    :param service: Service specifications.
    :type service: Service
    :param connector: Connection specifications.
    :type connector: Connector
    :return: Running process and connected instance.
    :rtype: Manager
    """
    process = service.start()
    try:
        instance = connector.connect()
    except BaseException:
        process.stop()
        raise
    return Manager(process, instance)

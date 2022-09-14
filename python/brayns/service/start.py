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

from brayns.network import Connector

from .manager import Manager
from .service import Service


def start(service: Service, connector: Connector) -> Manager:
    """Start a braynsService instance and connect to it.

    Returns the service process and the connected instance (see ``Manager``).

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
    except:
        process.stop()
        raise
    return Manager(process, instance)

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

import logging
from collections.abc import Callable
from dataclasses import dataclass, field

from brayns.network import Connector, Logger, SslClientContext

from .log_level import LogLevel
from .manager import Manager
from .plugin import Plugin
from .service import Service
from .ssl_server_context import SslServerContext


@dataclass
class Bundle:
    """Bundle with parameters of both ``Service`` and ``Connector``.

    Used to start a backend process and connect to it.

    As both client and server run on the same host, the client automatically
    connects to localhost and only the port can be configured.

    See ``Connector`` and ``Service`` for details about other parameters.
    """

    port: int
    service_ssl: SslServerContext | None = None
    service_log_level: LogLevel = LogLevel.WARN
    service_plugins: list[str] = field(default_factory=lambda: Plugin.all)
    service_executable: str = 'braynsService'
    service_env: dict[str, str] = field(default_factory=dict)
    connector_ssl: SslClientContext | None = None
    connector_binary_handler: Callable[[bytes], None] = lambda _: None
    connector_logger: logging.Logger = field(default_factory=Logger)
    connector_max_attempts: int | None = None
    connector_attempt_period: float = 0.1

    @property
    def uri(self) -> str:
        """Return the common URI between the service and the connector.

        :return: Common URI (localhost:<port>).
        :rtype: str
        """
        return f'localhost:{5000}'

    def start(self) -> Manager:
        """Start an instance backend and connect to it.

        The service returned must be stopped by the user (see Service class).

        :return: Service with both client and server inside.
        :rtype: Service
        """
        service = self.create_service()
        process = service.start()
        try:
            connector = self.create_connector()
            instance = connector.connect()
        except:
            process.stop()
            raise
        return Manager(process, instance)

    def create_service(self) -> Service:
        """Low level API to create the service from the settings."""
        return Service(
            uri=self.uri,
            ssl_context=self.service_ssl,
            log_level=self.service_log_level,
            plugins=self.service_plugins,
            executable=self.service_executable,
            env=self.service_env,
        )

    def create_connector(self) -> Connector:
        """Low level API to create the connector from the settings."""
        return Connector(
            uri=self.uri,
            ssl_context=self.connector_ssl,
            binary_handler=self.connector_binary_handler,
            logger=self.connector_logger,
            max_attempts=self.connector_max_attempts,
            attempt_period=self.connector_attempt_period,
        )

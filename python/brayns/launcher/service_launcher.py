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

from .launcher import Launcher
from .log_level import LogLevel
from .plugin import Plugin
from .service import Service
from .ssl_server_context import SslServerContext


@dataclass
class ServiceLauncher:
    """Helper class to combine ``Launcher`` and ``Connector``.

    Used to start both braynsService subprocess (server) and connect to it as an
    instance (client).

    As both client and server run on the same host, the client automatically
    connects to localhost and only the port can be configured.

    See ``Connector`` and ``Launcher`` for details about other parameters.
    """

    port: int
    client_ssl: SslClientContext | None = None
    client_binary_handler: Callable[[bytes], None] = lambda _: None
    client_logger: logging.Logger = field(default_factory=Logger)
    server_ssl: SslServerContext | None = None
    server_log_level: LogLevel = LogLevel.WARN
    server_plugins: list[str] = field(default_factory=Plugin.get_all_values)
    server_executable: str = 'braynsService'
    server_env: dict[str, str] = field(default_factory=dict)

    @property
    def uri(self) -> str:
        """Return the common URI between the client and server.

        :return: Common URI (localhost:<port>).
        :rtype: str
        """
        return f'localhost:{5000}'

    def start(self) -> Service:
        """Start an instance backend and connect to it.

        The service returned must be stopped by the user (see Service class).

        :return: Service with both client and server inside.
        :rtype: Service
        """
        launcher = self.create_launcher()
        server = launcher.start()
        try:
            connector = self.create_connector()
            client = connector.connect()
        except:
            server.stop()
            raise
        return Service(server, client)

    def create_launcher(self) -> Launcher:
        """Low level API to create the launcher from the settings."""
        return Launcher(
            uri=self.uri,
            ssl_context=self.server_ssl,
            log_level=self.server_log_level,
            plugins=self.server_plugins,
            executable=self.server_executable,
            env=self.server_env,
        )

    def create_connector(self) -> Connector:
        """Low level API to create the connector from the settings."""
        return Connector(
            uri=self.uri,
            ssl_context=self.client_ssl,
            binary_handler=self.client_binary_handler,
            logger=self.client_logger,
            max_attempts=None,
        )

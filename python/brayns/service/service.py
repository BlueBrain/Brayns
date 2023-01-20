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

from dataclasses import dataclass, field
from enum import Enum

from .process import Process


@dataclass
class SslServerContext:
    """Server SSL context.

    Can be used to start a braynsService instance with SSL enabled. Optional
    parameters such as the server private key, certificate and trusted CAs can
    be specified here.
    """

    private_key_file: str | None = None
    private_key_passphrase: str | None = None
    certificate_file: str | None = None
    ca_location: str | None = None


class LogLevel(Enum):
    """Server (braynsService backend) log level."""

    TRACE = "trace"
    DEBUG = "debug"
    INFO = "info"
    WARN = "warn"
    ERROR = "error"
    CRITICAL = "critical"
    OFF = "off"


class Plugin(Enum):
    """All built-in plugins for braynsService.

    Plugins are loaded when the backend instance is started and cannot be
    changed afterward.

    The value is the name of the plugin dynamic library (.so).
    """

    CIRCUIT_EXPLORER = "braynsCircuitExplorer"
    ATLAS_EXPLORER = "braynsAtlasExplorer"
    CYLINDRIC_CAMERA = "braynsCylindricCamera"
    DTI = "braynsDTI"
    MOLECULE_EXPLORER = "braynsMoleculeExplorer"

    @classmethod
    @property
    def all(cls) -> list[str]:
        """Shortcut to get all the plugin names."""
        return [plugin.value for plugin in Plugin]


@dataclass
class Service:
    """Class used to start a braynsService subprocess.

    Use a braynsService executable to start a subprocess. By default it looks
    for a 'braynsService' binary in the PATH but it can be changed.

    URI is the websocket server URI (ip:port). Use 0.0.0.0 as wildcard to allow
    connections from any machine.

    SSL server settings can be specified using optional certificate, key, CA and
    password.

    The backend log level can also be specified using ``log_level``.

    Custom environment variables can also be set for the subprocess, for example
    to override the PATH and load specific libraries.

    :param uri: Service URI with format 'host:port'.
    :type uri: str
    :param ssl_context: SSL context if secure, defaults to None.
    :type ssl_context: SslServerContext | None, optional
    :param max_clients: Max simultaneous connections authorized, defaults to 1.
    :type max_clients: int, optional
    :param log_level: Process log level, defaults to LogLevel.WARN.
    :type log_level: LogLevel, optional
    :param plugins: Plugins to load, defaults to all built-in plugins.
    :type plugins: list[str], optional
    :param executable: braynsService executable, defaults to 'braynService'.
    :type executable: str, optional
    :param env: Subprocess environment variables, default to empty.
    :type env: dict[str, str], optional
    """

    uri: str
    ssl_context: SslServerContext | None = None
    max_clients: int = 1
    log_level: LogLevel = LogLevel.WARN
    plugins: list[str] = field(default_factory=lambda: Plugin.all)
    executable: str = "braynsService"
    env: dict[str, str] = field(default_factory=dict)

    def get_command_line(self) -> list[str]:
        """Build the command line to start braynsService.

        :return: Command line arguments.
        :rtype: list[str]
        """
        args = [
            self.executable,
            "--uri",
            self.uri,
            "--max-clients",
            str(self.max_clients),
            "--log-level",
            self.log_level.value,
            *_get_plugins_args(self.plugins),
        ]
        if self.ssl_context is not None:
            args.append("--secure")
            args.append("true")
            ssl_args = _get_ssl_args(self.ssl_context)
            args.extend(ssl_args)
        return args

    def start(self) -> Process:
        """Start a new process for a braynsService backend.

        Return the process which runs the service.

        :return: Service process.
        :rtype: Process
        """
        args = self.get_command_line()
        return Process(args, self.env)


def _get_plugins_args(plugins: list[str]) -> list[str]:
    return [arg for plugin in plugins for arg in ("--plugin", plugin)]


def _get_ssl_args(context: SslServerContext) -> list[str]:
    args = list[str]()
    if context.private_key_file is not None:
        args.append("--private-key-file")
        args.append(context.private_key_file)
    if context.private_key_passphrase is not None:
        args.append("--private-key-passphrase")
        args.append(context.private_key_passphrase)
    if context.certificate_file is not None:
        args.append("--certificate-file")
        args.append(context.certificate_file)
    if context.ca_location is not None:
        args.append("--ca-location")
        args.append(context.ca_location)
    return args

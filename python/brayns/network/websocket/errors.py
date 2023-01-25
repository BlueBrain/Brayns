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

from dataclasses import dataclass

from brayns.utils import Error


@dataclass
class WebSocketError(Error):
    """Base class of all network exceptions.

    :param reason: Short description of what happened.
    :type reason: str
    """

    reason: str

    def __str__(self) -> str:
        """Display the reason when printed."""
        return self.reason


class ConnectionClosedError(WebSocketError):
    """Raised when doing operation on a disconnected instance.

    Happens after the connection to an instance.
    """


class InvalidServerCertificateError(WebSocketError):
    """Raised when the server certificate is rejected by the client.

    SSL specific error happening during the connection to an instance.
    """


class ProtocolError(WebSocketError):
    """Raised when a websocket protocol error occurs.

    Usually happens during the connection to an instance.
    """


class ServiceUnavailableError(WebSocketError):
    """Raised when braynsService instance is not available at given URI.

    Occurs when the backend instance is not ready or started.

    Can be used to try multiple connections to wait for an instance to be ready.
    """

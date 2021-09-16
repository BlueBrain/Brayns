# Copyright (c) 2021 EPFL/Blue Brain Project
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

"""Exception thrown when a JSON-RPC error is received."""

from typing import Any


class ReplyError(Exception):
    """JSON-RPC error message."""

    @staticmethod
    def from_dict(error: dict):
        """Build the error from the parsed JSON message.

        Args:
            error (dict): JSON-RPC error message ("error")

        Returns:
            ReplyError: error object built from the JSON.
        """
        return ReplyError(
            code=error.get('code'),
            message=error.get('message'),
            data=error.get('data')
        )

    def __init__(
        self,
        code: int,
        message: str,
        data: Any = None
    ) -> None:
        """Create a JSON-RPC error reply.

        Args:
            code (int): error code.
            message (str): error description.
            data (Any, optional): additional error data. Defaults to None.
        """
        self.code = code
        self.message = message
        self.data = data

    def __str__(self) -> str:
        """String representation of the error.

        Returns:
            str: error description.
        """
        return self.message

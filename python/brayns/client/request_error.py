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

from typing import Any


class RequestError(Exception):
    """Exception raised when a request fails."""

    def __init__(
        self,
        message: str,
        code: int = 0,
        data: Any = None
    ) -> None:
        """Initialize exception with error message data.

        :param message: JSON-RPC error description.
        :type message: str
        :param code: JSON-RPC error code.
        :type code: int
        :param data: additional optional error data.
        :type data: Any
        """
        self.message = message
        self.code = code
        self.data = data

    def __str__(self) -> str:
        """Format the error.

        :return: formatted error message with code and data.
        :rtype: str
        """
        message = f'{self.message} ({self.code})'
        if self.data is None:
            return message
        return f'{message}: {self.data!r}'

    def __eq__(self, other: 'RequestError') -> bool:
        return (
            self.code == other.code
            and self.message == other.message
            and self.data == other.data
        )

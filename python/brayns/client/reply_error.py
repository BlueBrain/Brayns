# Copyright 2015-2024 Blue Brain Project/EPFL
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


class ReplyError(Exception):
    """Exception thrown when an error reply is received."""

    @staticmethod
    def from_dict(error: dict):
        """Create an exception using the "error" field of the error reply.

        :param error: message error body
        :type error: dict
        :return: new exception object
        :rtype: ReplyError
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
        """Init exception attributes.

        :param code: error code.
        :type code: int
        :param message: error description
        :type message: str
        :param data: additional error data, defaults to None
        :type data: Any, optional
        """
        self.code = code
        self.message = message
        self.data = data

    def __str__(self) -> str:
        """Display it using the error message.

        :return: self.message
        :rtype: str
        """
        return self.message

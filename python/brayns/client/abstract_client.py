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

from abc import ABC, abstractmethod
from typing import Any, Union


class AbstractClient(ABC):
    """Interface to implement to connect to the renderer."""

    @abstractmethod
    def request(
        self,
        method: str,
        params: Any = None,
        request_id: Union[int, str] = 0,
        timeout: Union[float, None] = None,
    ) -> Any:
        """Send a JSON-RPC request to the renderer.

        :param method: method name
        :type method: str
        :param params: request params, defaults to None
        :type params: Any, optional
        :param request_id: request ID, defaults to 0
        :type request_id: Union[int, str], optional
        :param timeout: optional timeout in seconds, defaults to None
        :type timeout: Union[float, None], optional
        :return: request result.
        :rtype: Any
        """

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

"""Abstract class to define an interface for Brayns client."""

from abc import ABC, abstractmethod
from typing import Any, Union


class AbstractClient(ABC):
    """Interface representing a client to connect to a Brayns renderer."""

    @abstractmethod
    def request(
        self,
        method: str,
        params: Any = None,
        request_id: Union[int, str] = 0,
        timeout: Union[float, None] = None,
    ) -> Any:
        """Send a request to the connected Brayns renderer.

        Args:
            method (str): JSON-RPC method name.
            params (Any, optional): JSON-RPC params. Defaults to None.
            request_id (Union[int, str], optional): JSON-RPC request ID.
                Defaults to 0 if a single request is run at a time.
            timeout (Union[None, float], optional): max time in seconds to wait
                for the reply or None if infinite. Defaults to None.

        Returns:
            Any: JSON-RPC reply result.
        """

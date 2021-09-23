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

from typing import Any, Tuple, Union

from ..api import api_builder
from ..utils import image

from .abstract_client import AbstractClient
from .json_rpc_client import JsonRpcClient
from .request import Request


class Client(AbstractClient):
    """Brayns client implementation to connect to the renderer."""

    def __init__(
        self,
        uri: str,
        secure: bool = False,
        cafile: Union[str, None] = None
    ) -> None:
        """Connect to the renderer using the given settings.

        :param uri: Renderer URI in format host:port
        :type uri: str
        :param secure: True if SSL is enabled, defaults to False
        :type secure: bool, optional
        :param cafile: CA used to authenticate the renderer, defaults to None
        :type cafile: Union[str, None], optional
        """
        self._client = JsonRpcClient()
        self._client.connect(uri, secure, cafile)
        api_builder.build_api(self)

    def __enter__(self) -> None:
        """Allow using Brayns client in context manager."""
        return self

    def __exit__(self, *_) -> None:
        """Disconnect from Brayns renderer when exiting context manager."""
        self.disconnect()

    def disconnect(self) -> None:
        """Disconnect the client from the renderer.

        The client should not be used anymore after disconnection.
        """
        self._client.disconnect()

    def request(
        self,
        method: str,
        params: Any = None,
        request_id: Union[int, str] = 0,
        timeout: Union[float, None] = None,
    ) -> Any:
        """Send a JSON-RPC request to the renderer.

        Raise a ReplyError if an error message is received.
        Raise a Timeout error if a timeout is specified and reached.

        :param method: method name
        :type method: str
        :param params: request params, defaults to None
        :type params: Any, optional
        :param request_id: request ID, defaults to 0
        :type request_id: Union[int, str], optional
        :param timeout: request timeout in seconds, defaults to None
        :type timeout: Union[float, None], optional
        :return: reply result
        :rtype: Any
        """
        request = Request(method, params, request_id)
        self._client.send(request)
        if request.is_notification():
            return None
        return self._client.get_reply(
            request,
            timeout
        ).get_result()

    def image(
        self,
        size: Tuple[int, int],
        format: str = 'jpg',
        animation_parameters: Union[dict, None] = None,
        camera: Union[dict, None] = None,
        renderer: Union[dict, None] = None,
        quality: Union[int, None] = None,
        samples_per_pixel: Union[int, None] = None
    ) -> image.Image:
        """Take a snapshot from the renderer.

        :param size: viewport width and height
        :type size: Tuple[int, int]
        :param format: image format, defaults to 'jpg'
        :type format: str, optional
        :param animation_parameters: animation parameters used, defaults to None
        :type animation_parameters: Union[dict, None], optional
        :param camera: camera parameters used, defaults to None
        :type camera: Union[dict, None], optional
        :param renderer: renderer parameters to use, defaults to None
        :type renderer: Union[dict, None], optional
        :param quality: image quality 0-100, defaults to None
        :type quality: Union[int, None], optional
        :param samples_per_pixel: SPP, defaults to None
        :type samples_per_pixel: Union[int, None], optional
        :return: PIL image object
        :rtype: image.Image
        """
        return image.convert_snapshot_response_to_PIL(
            self.snapshot(**{
                key: value
                for key, value in locals().items()
                if key != 'self'
            })
        )

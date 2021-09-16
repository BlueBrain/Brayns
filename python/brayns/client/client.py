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

"""Client implementation to connect to a Brayns renderer."""

from typing import Any, Tuple, Union

from brayns.api import api_builder
from brayns.utils import image

from .abstract_client import AbstractClient
from .json_rpc_client import JsonRpcClient
from .request import Request


class Client(AbstractClient):
    """Brayns client."""

    def __init__(
        self,
        uri: str,
        secure: bool = False,
        cafile: Union[str, None] = None
    ) -> None:
        """Connect to a Brayns renderer.

        Raise an Exception from the underlying socket API in case of failure.

        Args:
            uri (str): Brayns renderer URI (host:port).
            secure (bool, optional): enable SSL. Defaults to False.
            cafile (Union[str, None], optional): provide a custom certification
                authority to authenticate the server. Defaults to None.
        """
        self._client = JsonRpcClient()
        self._client.connect(uri, secure, cafile)
        api_builder.build_api(self)

    def __enter__(self) -> None:
        """Allow using Brayns client in context manager."""

    def __exit__(self, *args) -> None:
        """Disconnect from Brayns renderer when exiting context manager."""
        self.disconnect()

    def disconnect(self) -> None:
        """Disconnect the client from the renderer.

        Should not be used after disconnection.
        """
        self._client.disconnect()

    def request(
        self,
        method: str,
        params: Any = None,
        request_id: Union[int, str] = 0,
        timeout: Union[float, None] = None,
    ) -> Any:
        """Send a request to the connected Brayns renderer.

        Raise a ReplyError when a JSON-RPC error is received.

        Raise a TimeoutError if the timeout is not None and reached.

        Args:
            method (str): JSON-RPC method name.
            params (Any, optional): JSON-RPC params. Defaults to None.
            request_id (Union[int, str], optional): JSON-RPC ID. Defaults to 0.
            timeout (Union[None, float], optional): max time to wait for the
                reply. Defaults to None.

        Returns:
            Any: JSON-RPC result as parsed JSON (ie dict or list).
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
        """Request a snapshot and return a PIL image.

        See snapshot entrypoint / method for more details.

        Args:
            size (Tuple[int, int]): Viewport width and height.
            format (str, optional): Image format. Defaults to 'jpg'.
            animation_parameters (Union[dict, None], optional): Animation parameters.
                Defaults to None if left as default.
            camera (Union[dict, None], optional): Camera parameters.
                Defaults to None if left as default.
            renderer (Union[dict, None], optional): Renderer parameters.
                Defaults to None if left as default.
            quality (Union[int, None], optional): Image quality.
                Defaults to None if left as default.
            samples_per_pixel (Union[int, None], optional): Samples per pixel.
                Defaults to None if left as default.

        Returns:
            image.Image: PIL image.
        """
        return image.convert_snapshot_response_to_PIL(
            self.snapshot(**{
                key: value
                for key, value in locals().items()
                if key != 'self'
            })
        )

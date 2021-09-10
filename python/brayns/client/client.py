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
from .request import Request
from .json_rpc_client import JsonRpcClient


class Client(AbstractClient):

    def __init__(
        self,
        uri: str,
        secure: bool = False,
        cafile: Union[str, None] = None
    ) -> None:
        self._client = JsonRpcClient()
        self._client.connect(uri, secure, cafile)
        api_builder.build_api(self)

    def __del__(self) -> None:
        self._client.disconnect()

    def request(
        self,
        method: str,
        params: Any = None,
        request_id: Union[int, str] = 0,
        timeout: Union[None, float] = None,
    ) -> Any:
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
        animation_parameters: dict = None,
        camera: dict = None,
        renderer: dict = None,
        quality: int = None,
        samples_per_pixel: int = None
    ) -> image.Image:
        """
        Request a snapshot from Brayns and return a PIL image.

        :param tuple size: (width,height) of the resulting image
        :param str format: image type as recognized by FreeImage
        :param object animation_parameters: animation params to use instead of current params
        :param object camera: camera to use instead of current camera
        :param int quality: compression quality between 1 (worst) and 100 (best)
        :param object renderer: renderer to use instead of current renderer
        :param int samples_per_pixel: samples per pixel to increase render quality
        :return: the PIL image of the current rendering, None on error obtaining the image
        :rtype: :py:class:`~PIL.Image.Image`
        """
        return image.convert_snapshot_response_to_PIL(
            self.snapshot(**{
                key: value
                for key, value in locals().items()
                if key != 'self'
            })
        )
